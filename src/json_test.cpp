#include <json/json.h>

#include <Windows.h>
#include <WinUser.h>
#include <dwmapi.h>
#include <errhandlingapi.h>
#include <intsafe.h>
#include <minwindef.h>
#include <windef.h>
#include <winnt.h>
#include <winreg.h>

#ifdef _WIN32
int WINAPI
WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hPrevInst*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/) {
#else
int
main() {
#endif
   // try {
#ifndef NDEBUG
   if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) {
      FILE* old = nullptr;
      freopen_s(&old, "CONOUT$", "w", stdout);
      freopen_s(&old, "CONOUT$", "w", stderr);
   }
#endif  // DEBUG

   auto test = js::parse<std::tuple<double, std::string>>("[12.5, \"toto\"]");

   return 0;
}
