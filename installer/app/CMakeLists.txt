file(GLOB_RECURSE ASSETS ${CMAKE_CURRENT_SOURCE_DIR}/*)

add_custom_command(
   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dist/index.html
   DEPENDS ${ASSETS}
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
   COMMAND npm run build
)

add_custom_target(installer_app 
   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/dist/index.html
)