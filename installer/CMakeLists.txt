add_subdirectory(app)

option(WATCH_MODE "Enable watch mode in debug mode" ON)

set(COMPILE_OPTIONS)
if(WATCH_MODE)
    list(APPEND COMPILE_OPTIONS
        "$<$<CONFIG:Debug>:-DWATCH_MODE>"
    )
endif()

win32_executable(TARGET_NAME installer_int 
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
        Bindings/validate.cpp

    COMPILE_OPTIONS
        ${COMPILE_OPTIONS}
)

target_link_libraries(installer_int PRIVATE 
        alx-home::webview 
        alx-home::json 
        alx-home::promise
        installer_resources 
        vfrnav::registry
        Dwmapi
        Boost::iostreams
)

set(MAJOR_VERSION 1)
set(MINOR_VERSION 0)
set(REVISION 0)

set(VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${REVISION})

set_target_properties(installer_int PROPERTIES VERSION ${MAJOR_VERSION})
set_target_properties(installer_int PROPERTIES SOVERSION ${MAJOR_VERSION}.${MINOR_VERSION})
set_target_properties(installer_int PROPERTIES OUTPUT_NAME "vfrnav-installer-int")

# PACKAGER

package(TARGET_NAME installer_resources 
        Z_RESOURCES 
            server_bin ${CMAKE_BINARY_DIR}/server/vfrnav-server.exe
        APP_RESOURCES
            resources ${CMAKE_CURRENT_BINARY_DIR}/app
)

add_subdirectory(Wrapper/)