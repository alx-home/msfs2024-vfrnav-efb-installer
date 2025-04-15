cmake_minimum_required(VERSION 3.20)

project(vfrnav LANGUAGES CXX)

# enable_language(C ASM_MASM)

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