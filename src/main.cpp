#include "Main.h"
#include "Resources.h"

#include <json/json.h>
#include <promise/promise.h>
#include <webview/webview.h>

#include <ShObjIdl_core.h>
#include <WinUser.h>
#include <dwmapi.h>
#include <errhandlingapi.h>
#include <intsafe.h>
#include <minwindef.h>
#include <windef.h>
#include <winnt.h>
#include <winreg.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <utility>

#ifdef _WIN32
int WINAPI
WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hPrevInst*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/) {
#else
int
main() {
#endif

#ifdef PROMISE_MEMCHECK
   auto const _{promise::memcheck()};
#endif

   try {
#ifndef NDEBUG
      if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) {
         FILE* old = nullptr;
         freopen_s(&old, "CONOUT$", "w", stdout);
         freopen_s(&old, "CONOUT$", "w", stderr);
      }
#endif  // DEBUG

      Main main{};
   } catch (const webview::exception& e) {
      std::cerr << e.what() << '\n';
      return 1;
   }

   return 0;
}

Main::Main()
   : webview_{
#ifndef NDEBUG
        true,
#else
        false,
#endif
        nullptr,
        []() constexpr {
           auto const options{webview::detail::win32_edge_engine::make_options()};
           webview::detail::win32_edge_engine::set_schemes_option({"app"}, options);
           return options;
        }(),
        0,
        user_data_dir_
     } {

   {
      auto const handle          = webview_.window();
      COLORREF   title_bar_color = 0x00ffb000;
      DwmSetWindowAttribute(
         handle, DWMWINDOWATTRIBUTE::DWMWA_CAPTION_COLOR, &title_bar_color, sizeof(title_bar_color)
      );
   }

   install_ressource_handler();

   webview_.set_size(960, 640, webview::Hint::NONE);
   webview_.set_size(480, 320, webview::Hint::MIN);

   install_bindings();

   webview_.navigate("app://app/index.html");
   webview_.run();
}

void
Main::install_ressource_handler() {
   webview_.register_url_handler(
      "*", [](webview::http::request_t const& request) {
         if (std::string const origin = "app://app/"; request.uri.starts_with(origin)) {
            auto const file = request.uri.substr(origin.size());

            auto const resource = resources.find(file);
            if (resource != resources.end()) {
               std::vector<char> data{};
               data.resize(resource->second.size());
               std::ranges::copy(resource->second, data.begin());

               auto const ext         = file.substr(file.find_last_of('.') + 1);
               auto const contentType = ext == "js" ? "text/javascript" : "text/html";

               webview::http::response_t response{
                  .body         = data,
                  .reasonPhrase = "Ok",
                  .statusCode   = 200,
                  .headers      = {{"Content-Type", contentType}, {"Access-Control-Allow-Origin", "*"}}
               };

               return response;
            }
         }

         return webview::http::response_t{.body = {}, .reasonPhrase = "Not Found", .statusCode = 404, .headers = {}};
      }
   );

   webview_.install_ressource_handler();
}

template <class Return, class... Args>
void
Main::bind(std::string_view name, Return (Main::*member_ptr)(Args...)) {
   [&]<std::size_t... index>(std::index_sequence<index...>) constexpr {
      webview_.bind(name, std::function<Return(Args...)>{std::bind(member_ptr, this, std::_Ph<index + 1>{}...)});
   }(std::make_index_sequence<sizeof...(Args)>());
}

void
Main::install_bindings() {
   bind("abort", &Main::abort);
   bind("exists", &Main::exists);
   bind("parentExists", &Main::parentExists);
   bind("log", &Main::log);
   bind("openFile", &Main::openFile);
   bind("openFolder", &Main::openFolder);
   bind("findCommunity", &Main::findCommunity);
   bind("defaultInstallPath", &Main::defaultInstallPath);
   bind("validate", &Main::validate);
}