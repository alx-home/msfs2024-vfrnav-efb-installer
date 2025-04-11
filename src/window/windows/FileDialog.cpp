#include "../FileDialog.h"

#include <array>
#include <combaseapi.h>
#include <exception>
#include <filesystem>
#include <json/json.h>
#include <mutex>
#include <objbase.h>
#include <promise/promise.h>
#include <ShObjIdl_core.h>
#include <string_view>
#include <string>
#include <thread>
#include <unordered_map>
#include <webview/detail/utility/string.h>
#include <windows/Env.h>
#include <winnt.h>

namespace dialog {
struct exception : std::exception {
   explicit exception(std::string_view msg)
      : std::exception{msg.data()} {
   }
};

namespace {

enum class Type {
   File,
   Folder
};

class FileDialog {
public:
   using Fdialog = std::unique_ptr<IFileOpenDialog, void (*)(IFileOpenDialog*)>;

   FileDialog()
      : fdialog_{
           []() constexpr {
              // CREATE FileOpenDialog OBJECT
              IFileOpenDialog* fdialog_ptr;
              if (HRESULT result = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fdialog_ptr)); FAILED(result)) {
                 throw exception{"FileDialog::CoCreateInstance error: " + std::to_string(result)};
              }

              return Fdialog{
                 fdialog_ptr,
                 [](IFileOpenDialog* fdialog) {
                    fdialog->Release();
                 }
              };
           }()
        } {
   }

   void setFileTypes(std::span<COMDLG_FILTERSPEC> types) {
      fdialog_->SetFileTypes(types.size(), types.data());
   }

   void addOptions(FILEOPENDIALOGOPTIONS option) {
      DWORD dwFlags;
      if (HRESULT result = fdialog_->GetOptions(&dwFlags); FAILED(result)) {
         throw exception{"FileDialog::GetOption error: " + std::to_string(result)};
      }

      if (HRESULT result = fdialog_->SetOptions(dwFlags | option); FAILED(result)) {
         throw exception{"FileDialog::SetOptions error: " + std::to_string(result)};
      }
   }

   void setFolder(std::wstring path) {
      std::ranges::transform(path, path.begin(), [](wchar_t value) {
         return value == L'/' ? L'\\' : value;
      });

      std::unique_ptr<IShellItem, void (*)(IShellItem*)> item{nullptr, [](IShellItem*) {}};
      {
         IShellItem* pCurFolder = nullptr;
         if (HRESULT result = SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(&pCurFolder)); FAILED(result)) {
            throw exception{"FileDialog::SHCreateItemFromParsingName error: " + std::to_string(result)};
         }

         item = {
            pCurFolder,
            [](IShellItem* pCurFolder) {
               pCurFolder->Release();
            }
         };
      };

      if (HRESULT result = fdialog_->SetFolder(item.get()); FAILED(result)) {
         throw exception{"FileDialog::SetFolder error: " + std::to_string(result)};
      }
   }

   bool show(HWND owner = nullptr) {
      if (HRESULT result = fdialog_->Show(owner); FAILED(result)) {
         if (result == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
            return false;
         }
         throw exception{"FileDialog::Show error: " + std::to_string(result)};
      }

      return true;
   }

   std::wstring getResult() {

      std::unique_ptr<IShellItem, void (*)(IShellItem*)> files{nullptr, [](IShellItem*) {}};
      {
         //  RETRIEVE FILE NAME FROM THE SELECTED ITEM
         IShellItem* files_ptr;
         if (HRESULT result = fdialog_->GetResult(&files_ptr); FAILED(result)) {
            throw exception{"FileDialog::GetResult error: " + std::to_string(result)};
         }

         files = {
            files_ptr,
            [](IShellItem* files) {
               files->Release();
            }
         };
      };

      LPWSTR result_ptr;
      if (HRESULT result = files->GetDisplayName(SIGDN_FILESYSPATH, &result_ptr); FAILED(result)) {
         throw exception{"FileDialog::GetDisplayName error: " + std::to_string(result)};
      }

      std::wstring result{result_ptr};
      CoTaskMemFree(result_ptr);
      return result;
   }

   auto operator->() {
      return fdialog_.get();
   }

   auto operator->() const {
      return fdialog_.get();
   }

private:
   Fdialog fdialog_;
};

template <Type type>
Promise<std::string>
Open(std::string_view path) {
   static std::mutex                                        thread_mutex{};
   static std::unordered_map<std::thread::id, std::jthread> threads{};

   co_return co_await make_promise([path](resolve_t<std::string> const& resolve, reject_t const& reject) -> Promise<std::string, true> {
      {
         std::lock_guard lock{thread_mutex};
         std::jthread    thread{
            [&reject, &resolve, value = std::filesystem::path{ReplaceEnv(std::string{path})}]() {
               struct unregister_thread {
                  ~unregister_thread() {
                     std::lock_guard lock{thread_mutex};

                     auto it = threads.find(std::this_thread::get_id());
                     assert(it != threads.end());
                     it->second.detach();
                     threads.erase(it);
                  }
               } _{};

               try {
                  FileDialog fdialog{};

                  if constexpr (type == Type::File) {
                     std::array<COMDLG_FILTERSPEC, 1> filTypes = {
                        COMDLG_FILTERSPEC{L"MSFS Executable", L"FlightSimulator2024.exe;FlightSimulator.exe"}
                     };
                     fdialog.setFileTypes(filTypes);
                  } else {
                     fdialog.addOptions(FOS_PICKFOLDERS);
                  }

                  if (std::filesystem::exists(value)) {

                     std::filesystem::path path{value};
                     if (!std::filesystem::is_directory(path)) {
                        path = path.parent_path();
                     }

                     fdialog.setFolder(path);
                  }

                  if (!fdialog.show()) {
                     return make_reject<exception>(reject, "FileDialog closed");
                  }

                  return resolve(webview::detail::narrow_string(fdialog.getResult()));
               } catch (...) {
                  reject(std::current_exception());
               }
            }
         };
         threads.emplace(thread.get_id(), std::move(thread));
      }

      co_return;
   });
}
}  // namespace

Promise<std::string>
OpenFile(std::string_view path) {
   return make_promise(Open<Type::File>, path);
}

Promise<std::string>
OpenFolder(std::string_view path) {
   return make_promise(Open<Type::Folder>, path);
}
}  // namespace dialog