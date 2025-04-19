
#include "Resources.h"
#include "Main.h"
#include "Registry/Install.h"
#include "Registry/Registry.h"
#include "json/json.h"
#include "windows/Registry/impl/Registry.h"

#include <filesystem>
#include <fstream>
#include <regex>
#include <string_view>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
#pragma clang diagnostic ignored "-Wdeprecated-copy-with-user-provided-copy"
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#pragma clang diagnostic pop

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

Promise<bool>
Main::validate(std::string startupOption, std::string communityPath, std::string installPath) {
   if (std::filesystem::path const path{installPath}; !path.has_parent_path() || !std::filesystem::exists(path.parent_path())) {
      co_return Fatal("Parent path not found : <br/>\"" + installPath + "\""), false;
   }

   if (!std::filesystem::exists(communityPath)) {
      co_return Fatal("Path not found : <br/>\"" + communityPath + "\""), false;
   }

   if (startupOption != "Startup" && startupOption != "Login" && startupOption != "Never") {  //@todo json : std::variant + fixed string value
      co_return Fatal("Unknown startup option : \"" + startupOption + "\""), false;
   }

   if (!std::filesystem::create_directory(installPath)) {
      if (!std::filesystem::exists(installPath) || !std::filesystem::is_directory(installPath)) {
         co_return Fatal("Couldn't create directory : <br/>\"" + installPath + "\""), false;
      }
   }

   {
      std::ofstream file(installPath + "\\vfrnav-server.exe", std::ios::ate | std::ios::binary);

      using namespace boost::iostreams;

      filtering_istreambuf in;
      in.push(zlib_decompressor());
      in.push(array_source{reinterpret_cast<char const*>(server_bin.data()), server_bin.size()});

      copy(in, file);
   }

   auto const                  fsPath  = std::filesystem::path(communityPath).parent_path().parent_path().parent_path();
   std::filesystem::path const exePath = fsPath.string() + "\\exe.xml";

   auto& registry = registry::get<Store::HKEY_CURRENT_USER_>();
   auto& settings = registry.alx_home_->settings_;

   bool cleanExe = false;
   if (settings->launch_mode_) {
      auto const oldValue = *settings->launch_mode_;

      if (oldValue == "Startup") {
         cleanExe = true;
      }
   }

   registry.clear();

   auto& uninstall = registry.current_version_->uninstall_;

   uninstall->icon_      = installPath + "\\vfrnav.exe";
   uninstall->name_      = "MSFS VFRNav' Server";
   uninstall->version_   = "1.0.0";
   uninstall->publisher_ = "alx-home";
   uninstall->uninstall_ = installPath + "\\vfrnav.exe --uninstall";

   settings->launch_mode_ = startupOption;
   settings->community_   = communityPath;
   settings->destination_ = installPath;

   if (startupOption == "Startup" || cleanExe) {
      if (auto const exists = std::filesystem::exists(exePath); !exists && (startupOption == "Startup")) {
         Warning("Path not found : <br/>\"" + exePath.string() + "\"<br/><br/>Couldn't clean it !");
      } else if (exists) {
         std::string content;

         {
            std::ifstream file{exePath};

            file.seekg(0, std::ios::end);
            content.resize(file.tellg());
            file.seekg(0, std::ios::beg);

            file.read(content.data(), content.size());
            content = content.data();

            content = RemoveFromExeXml(content);

            if (startupOption == "Startup") {
               content = AddToExeXml(content, installPath);
            }
         }

         std::ofstream file{exePath, std::ios::ate};
         file.write(content.data(), content.size());
      } else {
         Warning("Path not found : <br/>\"" + exePath.string() + "\"");
      }
   }

   if (startupOption == "Login") {
      auto& run   = registry.current_version_->run_;
      run->value_ = installPath + "\\vfrnav.exe --minimize";
   }

   co_return true;
}
