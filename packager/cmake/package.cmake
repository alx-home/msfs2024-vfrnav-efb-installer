find_program(NASM_EXECUTABLE nasm REQUIRED)

function(package)
    set(options)
    set(oneValueArgs TARGET_NAME)
    set(multiValueArgs DEPENDS Z_RESOURCES RESOURCES APP_RESOURCES)
    cmake_parse_arguments(PARSE_ARGV 0 arg
        "${options}" "${oneValueArgs}" "${multiValueArgs}"
    )

    set(ASM_FILE ${CMAKE_CURRENT_BINARY_DIR}/Resources.asm)
    set(ASM_OBJ ${CMAKE_CURRENT_BINARY_DIR}/Resources.obj)
    set(ASM_CPP ${CMAKE_CURRENT_BINARY_DIR}/Resources.cpp)

    set(ARGS "")
    unset(RESOURCE_NAME)
    foreach(ELEM IN LISTS arg_APP_RESOURCES)
        if(DEFINED RESOURCE_NAME)
            set(ARGS ${ARGS} --app ${RESOURCE_NAME} ${ELEM}/dist)
            unset(RESOURCE_NAME)
            list(APPEND arg_DEPENDS ${ELEM}/dist/index.html)
        else()
            set(RESOURCE_NAME "${ELEM}")
        endif()
    endforeach()
    
    if (DEFINED RESOURCE_NAME)
        message(FATAL_ERROR "Invalid package args")
    endif()

    foreach(ELEM IN LISTS arg_RESOURCES)
        if(DEFINED RESOURCE_NAME)
            set(ARGS ${ARGS} --resource ${RESOURCE_NAME} ${ELEM})
            unset(RESOURCE_NAME)
            list(APPEND arg_DEPENDS ${ELEM})
        else()
            set(RESOURCE_NAME "${ELEM}")
        endif()
    endforeach()
    
    if (DEFINED RESOURCE_NAME)
        message(FATAL_ERROR "Invalid package args")
    endif()

    foreach(ELEM IN LISTS arg_Z_RESOURCES)
        if(DEFINED RESOURCE_NAME)
            set(ARGS ${ARGS} --resource ${RESOURCE_NAME} --option zip ${ELEM})
            unset(RESOURCE_NAME)
            list(APPEND arg_DEPENDS ${ELEM})
        else()
            set(RESOURCE_NAME "${ELEM}")
        endif()
    endforeach()
    
    if (DEFINED RESOURCE_NAME)
        message(FATAL_ERROR "Invalid package args")
    endif()

    add_custom_command(
        OUTPUT ${ASM_FILE} ${ASM_CPP}
        COMMAND $<TARGET_FILE:packager> ${CMAKE_CURRENT_BINARY_DIR} ${ARGS}
        DEPENDS 
            packager 
            ${arg_DEPENDS}
        COMMENT "Generating ${arg_DEPENDS} with ${ARGS}"
    )

    add_custom_command(
        DEPENDS ${ASM_FILE}
        OUTPUT ${ASM_OBJ} 
        COMMAND ${NASM_EXECUTABLE} -f win64 ${ASM_FILE} -o ${ASM_OBJ}
        COMMENT "Assembling ${ASM_FILE}"
    )

    add_library(${arg_TARGET_NAME} ${ASM_CPP} ${ASM_OBJ})
    set_target_properties(${arg_TARGET_NAME} PROPERTIES 
        LINKER_LANGUAGE CXX
        CXX_STANDARD 26
        CMAKE_CXX_STANDARD_REQUIRED ON
        CMAKE_CXX_EXTENSIONS ON
    )
endfunction(package)