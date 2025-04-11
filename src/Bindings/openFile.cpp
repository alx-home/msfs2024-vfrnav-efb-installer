#include "Main.h"

#include <window/FileDialog.h>

Promise<std::string>
Main::openFile(std::string defaultPath) {
   co_return co_await dialog::OpenFile(defaultPath);
}