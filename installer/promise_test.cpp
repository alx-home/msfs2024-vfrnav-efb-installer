#include "promise/promise.h"

#include <Windows.h>
#include <WinUser.h>
#include <dwmapi.h>
#include <errhandlingapi.h>
#include <intsafe.h>
#include <minwindef.h>
#include <windef.h>
#include <winnt.h>
#include <winreg.h>

#include <chrono>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <thread>

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

   // try {
#ifndef NDEBUG
   if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) {
      FILE* old = nullptr;
      freopen_s(&old, "CONOUT$", "w", stdout);
      freopen_s(&old, "CONOUT$", "w", stderr);
   }
#endif  // DEBUG
   {
      auto main_prom{make_promise([]() -> Promise<void> {
         try {
            resolve_t<int> const* resolver{};
            reject_t const*       rejecter{};

            auto prom{
               make_promise([&resolver, &rejecter](resolve_t<int> const& resolve, reject_t const& reject) -> Promise<int, true> {
                  resolver = &resolve;
                  rejecter = &reject;
                  //   make_reject<std::runtime_error>(*rejecter, "tutu");
                  co_return;
               })
            };

            auto prom2{make_promise([&]() -> Promise<int> {
               auto const result = ((co_await prom) + 1);
               // throw std::runtime_error("test");
               co_return result;
            })};

            auto promInt{
               prom2
                  .then([](int value) -> Promise<double> {
                     throw std::runtime_error("test");
                     co_return value + 3;
                  })
                  .then([](double value) -> Promise<double> {
                     std::cout << "not evaluted" << std::endl;
                     co_return value;
                  })
                  .catch_([](std::exception_ptr) -> Promise<int> {
                     // throw std::runtime_error("test");
                     co_return 300;
                  })
                  .then([](std::variant<int, double> value) -> Promise<double> {
                     // throw std::runtime_error("test");
                     co_return std::holds_alternative<int>(value) ? std::get<int>(value) + 3 : std::get<double>(value) + 8788;
                  })
                  .then([](resolve_t<int> const& resolve, reject_t const&, double value) -> Promise<int, true> {
                     co_return resolve(static_cast<int>(value) + 3);
                  })
                  .then([](int value) -> Promise<int> {
                     co_return value + 3;
                  })
            };

            auto prom3{
               make_promise([&](resolve_t<int> const& resolve, reject_t const&) -> Promise<int, true> {
                  try {
                     resolve((co_await prom2) + 5);
                  } catch (std::runtime_error const& e) {
                     std::cout << "PP3 " << e.what() << std::endl;
                     throw;
                  }
                  co_return;
               })
            };

            auto prom4{
               make_promise([]() -> Promise<int> {
                  co_return 999;
               })
                  .then([](int value) -> Promise<void> {
                     std::cout << value << std::endl;
                     co_return;
                  })
                  .then([]() -> Promise<void> {
                     co_return;
                  })
                  .then([](resolve_t<int> const& resolve, reject_t const&) -> Promise<int, true> {
                     co_return resolve(111);
                  })
                  .then([](resolve_t<void> const& resolve, reject_t const&, int value) -> Promise<void, true> {
                     std::cout
                        << value << std::endl;
                     co_return resolve();
                  })
                  .then([]() -> Promise<int> {
                     co_return 888;
                  })
            };

            auto promall = make_promise([&]() -> Promise<void> {
               auto const [res1, res2, res3, res4, int_] = co_await promise::All(prom, prom2, prom3, prom4, promInt);

               std::cout << res1 << " " << res2 << " " << res3 << " " << res4
                         << " " << int_
                         << std::endl;

               co_return;
            });

            std::this_thread::sleep_for(std::chrono::seconds(1));
            // make_reject<std::runtime_error>(*rejecter, "titi");
            (*resolver)(5);
            co_await promall;
         } catch (std::exception const& e) {
            std::cout << "exc? " << e.what() << std::endl;
         }

         co_return;
      })};
   }

   return 0;
}
