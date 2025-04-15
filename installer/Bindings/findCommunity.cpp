#include "Main.h"

#include <filesystem>

std::string
FindCommunity() {
   if (std::string const steam = ReplaceEnv("%AppData%\\Microsoft Flight Simulator 2024\\Packages\\Community");
       std::filesystem::exists(steam)) {
      return steam + "\\alexhome-msfs2024-vfrnav";
   }

   if (std::string const msStore = ReplaceEnv("%LocalAppData%\\Packages\\Microsoft.Limitless_8wekyb3d8bbwe\\LocalCache\\Packages\\Community");
       std::filesystem::exists(msStore)) {
      return msStore + "\\alexhome-msfs2024-vfrnav";
   }

   return "";
}

Promise<std::string>
Main::findCommunity() {
   co_return FindCommunity();
}
