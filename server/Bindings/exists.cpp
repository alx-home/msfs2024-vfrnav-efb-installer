

#include "Main.h"

#include <filesystem>

Promise<bool>
Main::exists(std::string path) {
   co_return std::filesystem::exists(path);
}