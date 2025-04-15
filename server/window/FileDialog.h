#include <promise/promise.h>
#include <string>
#include <string_view>

namespace dialog {

Promise<std::string>
OpenFile(std::string_view path);

Promise<std::string>
OpenFolder(std::string_view path);

}  // namespace dialog