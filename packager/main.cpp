#include <Windows.h>

#include <WinUser.h>
#include <winnt.h>

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>


#ifdef _WIN32
int WINAPI
WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hPrevInst*/, LPSTR lpCmdLine, int /*nCmdShow*/) {
#else
int
main() {
#endif

    std::string cmd{lpCmdLine};

    auto const split = cmd.find_first_of(' ');

    std::size_t index = 0;

    std::string buildpath{cmd.substr(0, split)};
    std::string headerpath{buildpath.substr(0, buildpath.size() - 3) + "cpp"};
    std::string apppath{cmd.substr(split + 1)};

    std::cout << "AppPath: " << apppath << std::endl;
    std::cout << "BuildPath: " << buildpath << std::endl;
    std::cout << "HeaderPath: " << headerpath << std::endl
              << std::endl;

    std::ofstream asmOut{buildpath};
    std::ofstream headerOut{headerpath};

    asmOut << "section .data\n"
           << std::endl;

    std::vector<std::pair<std::string, std::string>> entries{};

    headerOut << R"_(#include <Windows.h>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

extern std::unordered_map<std::string, std::vector<char>> const resources;

)_" << std::endl;

    std::size_t resource_index  = 0;
    std::size_t offset = apppath.size();

    std::function<void(std::string const&)> const recurse = [&](std::string const& path) {
        std::cout << "Entering " << path << std::endl;
        if (!std::filesystem::is_directory(path)) {
            throw std::runtime_error("path " + path + " is not a directory");
        }

        for (auto const& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_directory()) {
                recurse(entry.path().string());
            } else {
                std::string filePath_ = entry.path().string();
                std::string filePath;
                std::string varName = "incbin_" + std::to_string(resource_index);
                ++resource_index;
                std::string relpath;

                for (std::size_t i = 0; i < filePath_.size(); ++i) {
                    if (i > offset) {
                        if (filePath_[i] == '\\') {
                            relpath += "/";
                        } else {
                            relpath += filePath_[i];
                        }
                    }

                    if (filePath_[i] == '\\' || filePath_[i] == '/' || filePath_[i] == '.' || filePath_[i] == '-') {
                        if (filePath_[i] == '\\') {
                            filePath += "/";
                        } else {
                            filePath += filePath_[i];
                        }
                    } else {
                        filePath += filePath_[i];
                    }
                }

                std::cout << "Generating " << entry << ": " << varName << std::endl;
                asmOut << std::format(R"_(global {0}_start, {0}_end
    {0}_start:
    incbin "{1}"
    {0}_end:
    )_",
                                      varName,
                                      filePath)
                       << std::endl;

                headerOut << std::format(R"_(
extern "C" char const    {0}_start[];
extern "C" char const    {0}_end[];
static std::size_t const {0}_size = {0}_end - {0}_start;)_",
                                         varName)
                          << std::endl;

                entries.emplace_back(relpath, varName);
                ++index;
            }
        }
    };

    recurse(apppath);

    headerOut << R"_(
std::unordered_map<std::string, std::vector<char>> const resources {)_"
              << std::endl;

    for (auto const& [path, label] : entries) {
        headerOut << "   { \""
                  << path << "\", []() { std::vector<char> result; result.resize(" << label << "_size); std::copy(" << label << "_start, " << label << "_end, result.begin()); return result; }() },"
                  << std::endl;
    }

    headerOut << R"_(};)_" << std::endl;
    return 0;
}
