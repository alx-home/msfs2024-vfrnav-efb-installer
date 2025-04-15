#include "Main.h"

#include <filesystem>

Promise<bool>
Main::parentExists(std::string path) {
   std::filesystem::path const _path = path;
   co_return _path.has_parent_path() && std::filesystem::exists(_path.parent_path());
}
