cmake_minimum_required(VERSION 3.20)

project(vfrnav LANGUAGES CXX)

# enable_language(C ASM_MASM)

add_subdirectory(packager)
add_subdirectory(src)


add_custom_target(init_submodules ALL
   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
   COMMAND git submodule update --init --recursive
)

add_custom_target(init_npm ALL
   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/app
   COMMENT "initializing npm modules"
   COMMAND npm install
)

add_custom_target(init ALL
   COMMENT "initializing git submodules"
   DEPENDS init_submodules init_npm
)