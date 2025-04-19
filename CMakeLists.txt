cmake_minimum_required(VERSION 3.20)

project(vfrnav LANGUAGES CXX)

# set(FETCHCONTENT_SOURCE_DIR_MICROSOFT_WEB_WEBVIEW2 ${CMAKE_SOURCE_DIR}/packages/Microsoft.Web.WebView2/)
set(FETCHCONTENT_SOURCE_DIR_WEBVIEW ${CMAKE_SOURCE_DIR}/packages/webview)
set(FETCHCONTENT_SOURCE_DIR_JSON ${CMAKE_SOURCE_DIR}/packages/json)
set(FETCHCONTENT_SOURCE_DIR_PROMISE ${CMAKE_SOURCE_DIR}/packages/promise)
set(FETCHCONTENT_SOURCE_DIR_WINDOWS ${CMAKE_SOURCE_DIR}/packages/windows)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies.cmake)

add_subdirectory(packager)
add_subdirectory(server)
add_subdirectory(installer)


add_custom_target(init_submodules
   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
   COMMAND git submodule update --init --recursive
)

add_custom_target(init_npm 
   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/app
   COMMENT "initializing npm modules"
   COMMAND npm install
)

add_custom_target(init 
   COMMENT "initializing git submodules"
   DEPENDS init_submodules init_npm
)