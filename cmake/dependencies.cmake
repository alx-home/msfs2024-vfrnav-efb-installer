include(FetchContent)

FetchContent_Declare(
    webview
    GIT_REPOSITORY https://github.com/webview/webview
    GIT_TAG 1.0.0)
FetchContent_MakeAvailable(webview)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/webview/webview
    GIT_TAG 1.0.0)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
    promise
    GIT_REPOSITORY https://github.com/webview/webview
    GIT_TAG 1.1.0)
FetchContent_MakeAvailable(promise)

FetchContent_Declare(
    windows
    GIT_REPOSITORY https://github.com/webview/webview
    GIT_TAG 1.0.0)
FetchContent_MakeAvailable(windows)