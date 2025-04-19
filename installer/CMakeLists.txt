add_subdirectory(app)

option(WATCH_MODE "Enable watch mode in debug mode" ON)

add_executable(installer WIN32 main.cpp main.rc 
        window/windows/FileDialog.cpp
        Bindings/abort.cpp
        Bindings/defaultInstallPath.cpp
        Bindings/exists.cpp
        Bindings/findCommunity.cpp
        Bindings/log.cpp
        Bindings/openFile.cpp
        Bindings/openFolder.cpp
        Bindings/parentExists.cpp
        Bindings/validate.cpp
        )
# add_executable(installer WIN32 promise_test.cpp Promise/impl/Promise.cpp)
# add_executable(installer WIN32 json_test.cpp ./json/json.cpp)

set_target_properties(installer PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(installer PROPERTIES CXX_STANDARD 26)
set_target_properties(installer PROPERTIES CMAKE_CXX_STANDARD_REQUIRED ON)
set_target_properties(installer PROPERTIES CMAKE_CXX_EXTENSIONS ON)

target_include_directories(installer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(installer PRIVATE 
        alx-home::webview 
        alx-home::windows 
        alx-home::json 
        alx-home::promise)

add_dependencies(installer server)

target_link_libraries(installer PRIVATE installer_resources Dwmapi)

set(COMPILE_OPTIONS
    -std=c++2c
    "$<$<CONFIG:DEBUG>:-DDEBUG>"
    -Wall -Wextra -Wpedantic -Wcast-align -Waddress-of-packed-member
    -ftemplate-backtrace-limit=0
    "$<$<CONFIG:Release>:-O3>"
    "$<$<CONFIG:Debug>:-O0>"
)

# set(SANITIZE "address")

if(DEFINED SANITIZE)
    list(APPEND COMPILE_OPTIONS
        -fsanitize=${SANITIZE}
    )
endif(DEFINED SANITIZE)

# if(DEFINED ADDRESS_SANITIZER)
#     list(APPEND COMPILE_OPTIONS 
#         "-DADDRESS_SANITIZER"
#         -fsanitize-recover=address
#     )
# endif(DEFINED ADDRESS_SANITIZER)

if(WATCH_MODE)
    list(APPEND COMPILE_OPTIONS
        "$<$<CONFIG:Debug>:-DWATCH_MODE>"
    )
endif()

if(MSVC)
    list(TRANSFORM COMPILE_OPTIONS PREPEND "-clang:")
    target_compile_options(installer PUBLIC /W4 ${COMPILE_OPTIONS})
else()
    target_compile_options(installer PUBLIC 
        -export-dynamic
        -ggdb3 -pg -g
        ${COMPILE_OPTIONS}
        -D_GNU_SOURCE
        -Wno-psabi
    )
endif()

target_link_libraries(installer PRIVATE Boost::iostreams)


set(MAJOR_VERSION 1)
set(MINOR_VERSION 0)
set(REVISION 0)

set(VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${REVISION})

set_target_properties(installer PROPERTIES VERSION ${MAJOR_VERSION})
set_target_properties(installer PROPERTIES SOVERSION ${MAJOR_VERSION}.${MINOR_VERSION})
set_target_properties(installer PROPERTIES OUTPUT_NAME "vfrnav-installer")


# PACKAGER

package(TARGET_NAME installer_resources 
        Z_RESOURCES 
            server_bin ${CMAKE_BINARY_DIR}/server/vfrnav-server.exe
        APP_RESOURCES
            resources ${CMAKE_CURRENT_BINARY_DIR}/app
)

