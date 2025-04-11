#pragma once

#include <windows/Env.h>

#include <webview/webview.h>
#include <wrl/client.h>
#include <string>

class Main {
public:
   Main();

private:
   void install_ressource_handler();
   void install_bindings();

   Promise<>            abort();
   Promise<bool>        exists(std::string path);
   Promise<bool>        parentExists(std::string path);
   Promise<>            log(std::string value);
   Promise<std::string> openFile(std::string defaultPath);
   Promise<std::string> openFolder(std::string defaultPath);
   Promise<std::string> findCommunity();
   Promise<std::string> defaultInstallPath();
   Promise<>            validate(std::string startupOption, std::string communityPath, std::string installPath);

private:
   std::string const appdata_       = GetAppData();
   std::string const user_data_dir_ = appdata_ + "\\MSFS VFRNav Server";

   webview::webview webview_;

   template <class Return, class... Args>
   void bind(std::string_view name, Return (Main::*member_ptr)(Args...));
};