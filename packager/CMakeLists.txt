add_executable(packager WIN32)
target_sources(packager PRIVATE main.cpp)
target_compile_options(packager PUBLIC /std:c++latest)

find_program(NASM_EXECUTABLE nasm REQUIRED)

set(ASM_FILE ${CMAKE_BINARY_DIR}/Resources.asm)
set(ASM_OBJ ${CMAKE_BINARY_DIR}/Resources.obj)
set(ASM_CPP ${CMAKE_BINARY_DIR}/Resources.cpp)

file(GLOB_RECURSE ASSETS ${CMAKE_SOURCE_DIR}/app/*)
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/dist
    DEPENDS  ${ASSETS}
    COMMAND cd ${CMAKE_SOURCE_DIR}/app && npm run build
)

add_custom_command(
    OUTPUT ${ASM_FILE} ${ASM_CPP}
    COMMAND $<TARGET_FILE:packager> ${ASM_FILE};${CMAKE_SOURCE_DIR}/dist
    DEPENDS packager ${CMAKE_SOURCE_DIR}/dist
    COMMENT "Generating ${ASM_FILE} for ${CMAKE_CURRENT_SOURCE_DIR}/dist"
)


add_custom_command(
    DEPENDS ${ASM_FILE}
    OUTPUT ${ASM_OBJ} 
    COMMAND ${NASM_EXECUTABLE} -f win64 ${ASM_FILE} -o ${ASM_OBJ}
    COMMENT "Assembling ${ASM_FILE}"
)

add_library(package ${ASM_CPP} ${ASM_OBJ})
