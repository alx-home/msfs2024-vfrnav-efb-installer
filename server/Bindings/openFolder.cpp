#include "Main.h"

#include <window/FileDialog.h>

Promise<std::string>
Main::openFolder(std::string defaultPath) {
   co_return co_await dialog::OpenFolder(defaultPath);
}