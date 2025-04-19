add_subdirectory(app)

option(WATCH_MODE "Enable watch mode in debug mode" ON)

set(COMPILE_OPTIONS)
if(WATCH_MODE)
    list(APPEND COMPILE_OPTIONS
        "$<$<CONFIG:Debug>:-DWATCH_MODE>"
    )
endif()

win32_executable(TARGET_NAME server 
    FILES 
        main.cpp main.rc 
        window/windows/FileDialog.cpp
        Bindings/abort.cpp
        Bindings/defaultInstallPath.cpp
        Bindings/exists.cpp
        Bindings/findCommunity.cpp
        Bindings/log.cpp
        Bindings/openFile.cpp
        Bindings/openFolder.cpp
        Bindings/parentExists.cpp

    COMPILE_OPTIONS
        ${COMPILE_OPTIONS}
)

target_link_libraries(server 
    PRIVATE 
        alx-home::webview 
        alx-home::json 
        alx-home::promise
        vfrnav::registry
        server_resources 
        Dwmapi
)

set(MAJOR_VERSION 1)
set(MINOR_VERSION 0)
set(REVISION 0)

set(VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${REVISION})

set_target_properties(server PROPERTIES VERSION ${MAJOR_VERSION})
set_target_properties(server PROPERTIES SOVERSION ${MAJOR_VERSION}.${MINOR_VERSION})
set_target_properties(server PROPERTIES OUTPUT_NAME "vfrnav-server")


# PACKAGER

package(TARGET_NAME server_resources 
        APP_RESOURCES
            resources ${CMAKE_CURRENT_BINARY_DIR}/app
)