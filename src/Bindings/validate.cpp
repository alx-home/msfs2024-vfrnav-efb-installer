
#include "Main.h"
#include "Registry/Install.h"
#include "Registry/Registry.h"

#include <filesystem>
#include <fstream>
#include <regex>
#include <string_view>

std::string
RemoveFromExeXml(std::string_view data) {
   // error_stack....
   // std::regex reg{R"(( |\t|\r?\n)*<Launch\.Addon>((.|\r?\n)(?!<Name>))*.?<Name>MSFS VFR Nav' Server((.|\r?\n)(?!</Launch\.Addon>))*.?</Launch\.Addon>([ \t]*(\r?\n)?))"};

   auto it = data.begin();

   auto const skip_space = [&]() constexpr {
      while (it != data.end() && (*it == ' ' || *it == '\t' || *it == '\n' || *it == '\r')) {
         ++it;
      }
   };

   auto const find = [&](std::string_view name) constexpr {
      skip_space();

      auto it2 = name.begin();

      for (; it != data.end(); ++it, ++it2) {
         if (it2 == name.end()) {
            skip_space();

            if (it != data.end() && *it == '>') {
               ++it;
               return true;
            }

            return false;
         }

         if (*it != *it2) {
            return false;
         }
      }

      return false;
   };

   auto const find_end = [&](std::string_view name) constexpr {
      for (; it != data.end(); ++it) {

         auto end = it;

         if (*it == '<') {
            ++it;
            if (it == data.end()) {
               return name.end();
            }

            skip_space();

            if (*it == '/') {
               ++it;
               skip_space();

               if (find(name)) {
                  return end;
               }
            }
         }
      }

      return name.end();
   };

   while (it != data.end()) {
      auto const beg = it;

      if (*it == ' ' || *it == '\t' || *it == '\r' || *it == '\n') {
         skip_space();
         if (it == data.end()) {
            return data.data();
         }
      }

      if (*it == '<') {
         ++it;
         if (find("Launch.Addon")) {
            auto const launch_beg = it;

            if (auto const launch_end = find_end("Launch.Addon"); launch_end != data.end()) {
               auto end = it;
               it       = launch_beg;

               while (it != end) {
                  if (*it == '<') {
                     ++it;

                     if (find("Name")) {
                        auto name_beg = it;

                        if (auto const name_end = find_end("Name"); name_end != data.end()) {
                           if (std::distance(it, launch_end) > 0) {
                              if (std::string_view{name_beg, name_end} == "MSFS VFR Nav' Server") {
                                 // FOUND :)
                                 it = end;
                                 skip_space();
                                 end = it;

                                 if (end != data.end()) {
                                    return std::string{data.begin(), beg} + std::string{end, data.end()};
                                 } else {
                                    return std::string{data.begin(), beg};
                                 }
                              }
                           }
                        }
                     }
                  } else {
                     ++it;
                  }
               }
            }
         }
      } else {
         ++it;
      }
   }

   return data.data();
}

std::string
AddToExeXml(std::string_view data, std::string_view path) {
   std::regex  reg{"(\r?\n)?</SimBase.Document>"};
   std::string replaceString{std::string_view{
      R"_(
    <Launch.Addon>
        <Disabled>False</Disabled>
        <ManualLoad>False</ManualLoad>
        <Name>MSFS VFR Nav' Server</Name>
        <Path>)_"
      + std::string{path}
      + R"_(/vfrnav.exe</Path>
        <CommandLine></CommandLine>
        <NewConsole>False</NewConsole>
    </Launch.Addon>
</SimBase.Document>)_"
   }};

   std::regex formated{"^[ \t].*"};

   if (!std::regex_search(std::string{data}, formated)) {
      std::regex remove_trailing{R"((\r?\n *))"};
      replaceString = std::regex_replace(replaceString, remove_trailing, "");
   }

   return std::regex_replace(std::string{data}, reg, replaceString);
}

Promise<>
Main::validate(std::string startupOption, std::string communityPath, std::string installPath) {
   if (std::filesystem::path const path{installPath}; !path.has_parent_path() || !std::filesystem::exists(path.parent_path())) {
      webview_.eval("window.pfatal(\"Parent path not found : " + installPath + "\");");
      co_return;
   }

   if (!std::filesystem::exists(communityPath)) {
      webview_.eval("window.pfatal(\"Path not found : " + communityPath + "\");");
      co_return;
   }

   if (startupOption != "Startup" && startupOption != "Login" && startupOption != "Never") {
      webview_.eval("window.pfatal(\"Unknown startup option : " + startupOption + "\");");
      co_return;
   }

   std::string executablePath;
   executablePath.reserve(MAX_PATH);
   GetModuleFileName(NULL, executablePath.data(), MAX_PATH);
   executablePath = executablePath.data();

   if (!std::filesystem::create_directory(installPath)) {
      if (!std::filesystem::exists(installPath) || !std::filesystem::is_directory(installPath)) {
         webview_.eval("window.pfatal(\"Couldn't create directory : " + installPath + "\");");
         co_return;
      }
   }

   std::filesystem::copy(executablePath, installPath, std::filesystem::copy_options::overwrite_existing);

   auto const                  fsPath  = std::filesystem::path(communityPath).parent_path().parent_path();
   std::filesystem::path const exePath = std::filesystem::path(communityPath).parent_path().parent_path().string() + "/exe.xml";

   auto& registry = registry::get<Store::HKEY_CURRENT_USER_>();
   registry.clear();

   auto& uninstall = registry.current_version_->uninstall_;

   uninstall->icon_      = installPath + "\\vfrnav.exe";
   uninstall->name_      = "MSFS VFRNav' Server";
   uninstall->version_   = "1.0.0";
   uninstall->publisher_ = "alx-home";
   uninstall->uninstall_ = installPath + "\\vfrnav.exe --uninstall";

   if (startupOption == "Startup") {
      if (!std::filesystem::exists(exePath)) {
         webview_.eval("window.pfatal(\"Path not found : " + exePath.string() + "\");");
         co_return;
      }

      std::string content;

      {
         std::ifstream file{exePath};

         file.seekg(0, std::ios::end);
         content.resize(file.tellg());
         file.seekg(0, std::ios::beg);

         file.read(content.data(), content.size());
         content = content.data();

         content = RemoveFromExeXml(content);
         content = AddToExeXml(content, installPath);
      }

      std::ofstream file{exePath, std::ios::ate};
      file.write(content.data(), content.size());
   }

   co_return;
}
