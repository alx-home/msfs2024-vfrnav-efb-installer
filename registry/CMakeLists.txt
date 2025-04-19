add_library(vfrnav_registry INTERFACE)
target_include_directories(vfrnav_registry INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries(vfrnav_registry INTERFACE alx-home::windows)

add_library(vfrnav::registry ALIAS vfrnav_registry)

# Hack for compile_commands
add_library(vfrnav_registry_ EXCLUDE_FROM_ALL empty.cpp)
target_link_libraries(vfrnav_registry_ PRIVATE vfrnav::registry)