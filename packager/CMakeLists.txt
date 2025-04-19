add_executable(packager WIN32)
target_sources(packager PRIVATE main.cpp)

set_target_properties(packager PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(packager PROPERTIES CXX_STANDARD 26)
set_target_properties(packager PROPERTIES CMAKE_CXX_STANDARD_REQUIRED ON)
set_target_properties(packager PROPERTIES CMAKE_CXX_EXTENSIONS ON)

set(COMPILE_OPTIONS
    -std=c++2c
    "$<$<CONFIG:DEBUG>:-DDEBUG>"
    -Wall -Wextra -Wpedantic -Wcast-align -Waddress-of-packed-member
    -ftemplate-backtrace-limit=0
    "$<$<CONFIG:Release>:-O3>"
    "$<$<CONFIG:Debug>:-O0>"
)


if(MSVC)
    list(TRANSFORM COMPILE_OPTIONS PREPEND "-clang:")
    target_compile_options(packager PUBLIC /W4 ${COMPILE_OPTIONS})
else()
    target_compile_options(packager PUBLIC 
        -export-dynamic
        -ggdb3 -pg -g
        ${COMPILE_OPTIONS}
        -D_GNU_SOURCE
        -Wno-psabi
    )
endif()

target_link_libraries(packager PRIVATE Boost::iostreams)

include(cmake/package.cmake)