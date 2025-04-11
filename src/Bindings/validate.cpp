
#include "Main.h"
#include "windows/Registry.h"

#include <filesystem>
#include <fstream>
#include <regex>

std::string
RemoveFromExeXml(std::string_view data) {
   std::regex reg{R"(( |\t|\r?\n)*<Launch\.Addon>((.|\r?\n)(?!<Name>))*.?<Name>MSFS VFR Nav' Server((.|\r?\n)(?!</Launch\.Addon>))*.?</Launch\.Addon>([ \t]*(\r?\n)?))"};
   return std::regex_replace(std::string{data}, reg, "");
}

std::string
AddToExeXml(std::string_view data, std::string_view path) {
   std::regex  reg{"</SimBase.Document>"};
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

   std::filesystem::copy(executablePath, installPath);

   auto const                  fsPath  = std::filesystem::path(communityPath).parent_path().parent_path();
   std::filesystem::path const exePath = std::filesystem::path(communityPath).parent_path().parent_path().string() + "/exe.xml";

   reg::Key Uninstall{};
   if (RegOpenKey(HKEY_CURRENT_USER, TEXT(R"(Software\Microsoft\Windows\CurrentVersion\Uninstall)"), Uninstall)
       != ERROR_SUCCESS) {
      webview_.eval(R"(window.pfatal("Could not open registry : HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Uninstall");)");
      co_return;
   }

   reg::Key UVFRNav{};
   RegDeleteKey(Uninstall, "MSFS.VFRNav.server");
   if (RegCreateKey(Uninstall, "MSFS.VFRNav.server", UVFRNav) != ERROR_SUCCESS) {
      webview_.eval(R"(window.pfatal("Could not create registry key : HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Uninstall\MSFS.VFRNav.server");)");
      co_return;
   }
   {
      std::string const value = installPath + "/vfrnav.exe";
      if (RegSetValueEx(UVFRNav, "DisplayIcon", 0, REG_SZ, reinterpret_cast<BYTE const*>(value.c_str()), value.size()) != ERROR_SUCCESS) {
         webview_.eval(R"(window.pfatal("Could not create registry key : HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Uninstall\MSFS.VFRNav.server");)");
         co_return;
      }
   }
   {
      std::string const value = "MSFS VFRNav' Server";
      if (RegSetValueEx(UVFRNav, "DisplayName", 0, REG_SZ, reinterpret_cast<BYTE const*>(value.c_str()), value.size()) != ERROR_SUCCESS) {
         webview_.eval(R"(window.pfatal("Could not create registry key : HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Uninstall\MSFS.VFRNav.server");)");
         co_return;
      }
   }
   {
      std::string const value = "1.0.0";
      if (RegSetValueEx(UVFRNav, "DisplayVersion", 0, REG_SZ, reinterpret_cast<BYTE const*>(value.c_str()), value.size()) != ERROR_SUCCESS) {
         webview_.eval(R"(window.pfatal("Could not create registry key : HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Uninstall\MSFS.VFRNav.server");)");
         co_return;
      }
   }
   {
      std::string const value = "alx-home";
      if (RegSetValueEx(UVFRNav, "Publisher", 0, REG_SZ, reinterpret_cast<BYTE const*>(value.c_str()), value.size()) != ERROR_SUCCESS) {
         webview_.eval(R"(window.pfatal("Could not create registry key : HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Uninstall\MSFS.VFRNav.server");)");
         co_return;
      }
   }
   {
      std::string const value = installPath + "/vfrnav.exe --uninstall";
      if (RegSetValueEx(UVFRNav, "UninstallString", 0, REG_SZ, reinterpret_cast<BYTE const*>(value.c_str()), value.size()) != ERROR_SUCCESS) {
         webview_.eval(R"(window.pfatal("Could not create registry key : HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Uninstall\MSFS.VFRNav.server");)");
         co_return;
      }
   }

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
