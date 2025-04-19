#include "Resources.h"

#include <Windows.h>
#include <synchapi.h>
#include <winbase.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
#pragma clang diagnostic ignored "-Wdeprecated-copy-with-user-provided-copy"
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#pragma clang diagnostic pop

#ifdef _WIN32
int WINAPI
WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hPrevInst*/, LPSTR lpCmdLine, int /*nCmdShow*/) {
#else
int
main() {
#endif
#ifndef NDEBUG
   if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) {
      FILE* old = nullptr;
      freopen_s(&old, "CONOUT$", "w", stdout);
      freopen_s(&old, "CONOUT$", "w", stderr);
   }
#endif  // DEBUG

   auto const temp = std::filesystem::temp_directory_path() / "vfrnav-installer.exe";
   {
      if (std::filesystem::exists(temp)) {
         MessageBox(
            nullptr,
            "VFRNav Installer is already runnning !",
            "Error",
            MB_OK | MB_ICONERROR
         );
         return EXIT_FAILURE;  //@todo
      }

      std::ofstream file(temp, std::ios::ate | std::ios::binary);

      using namespace boost::iostreams;

      filtering_istreambuf in;
      in.push(zlib_decompressor());
      in.push(array_source{reinterpret_cast<char const*>(installer_bin.data()), installer_bin.size()});

      copy(in, file);
   }

   struct Remove {
      std::filesystem::path const& path_;

      Remove(std::filesystem::path const& path)
         : path_{path} {}

      ~Remove() {
         auto const result = std::filesystem::remove(path_);

         if (!result) {
            MessageBox(
               nullptr,
               ("Couldn't remove \"" + path_.string() + "\" !").c_str(),
               "Error",
               MB_OK | MB_ICONERROR
            );
         }
      }
   } _{temp};

   // additional information
   STARTUPINFO         si;
   PROCESS_INFORMATION pi;

   // set the size of the structures
   ZeroMemory(&si, sizeof(si));
   si.cb = sizeof(si);
   ZeroMemory(&pi, sizeof(pi));

   // start the program up
   CreateProcess(temp.string().data(), lpCmdLine, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
   WaitForSingleObject(pi.hProcess, INFINITE);

   // Close process and thread handles.
   CloseHandle(pi.hProcess);
   CloseHandle(pi.hThread);

   return 0;
}