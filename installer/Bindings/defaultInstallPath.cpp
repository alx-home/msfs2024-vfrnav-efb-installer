
#include "Main.h"

Promise<std::string>
Main::defaultInstallPath() {
   co_return GetAppData() + "\\MSFS VFRNav Server";
}
