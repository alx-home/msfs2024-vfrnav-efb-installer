
#include "Main.h"
#include "Registry/Install.h"
#include "Registry/Registry.h"

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
