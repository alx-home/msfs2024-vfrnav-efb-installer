function(win32_executable)
   set(options)
   set(oneValueArgs TARGET_NAME COMPILE_OPTIONS)
   set(multiValueArgs FILES)
   cmake_parse_arguments(PARSE_ARGV 0 arg
      "${options}" "${oneValueArgs}" "${multiValueArgs}"
   )

   add_executable(${arg_TARGET_NAME} WIN32 
      ${arg_FILES}
   )

   set_target_properties(${arg_TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)
   set_target_properties(${arg_TARGET_NAME} PROPERTIES CXX_STANDARD 26)
   set_target_properties(${arg_TARGET_NAME} PROPERTIES CMAKE_CXX_STANDARD_REQUIRED ON)
   set_target_properties(${arg_TARGET_NAME} PROPERTIES CMAKE_CXX_EXTENSIONS ON)

   target_include_directories(${arg_TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

   set(COMPILE_OPTIONS
      ${arg_COMPILE_OPTIONS}
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

   if(MSVC)
      list(TRANSFORM COMPILE_OPTIONS PREPEND "-clang:")
      target_compile_options(${arg_TARGET_NAME} PUBLIC /W4 ${COMPILE_OPTIONS})
   else()
      target_compile_options(${arg_TARGET_NAME} PUBLIC 
         -export-dynamic
         -ggdb3 -pg -g
         ${COMPILE_OPTIONS}
         -D_GNU_SOURCE
         -Wno-psabi
      )
   endif()
endfunction(win32_executable)