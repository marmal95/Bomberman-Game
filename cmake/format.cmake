file(GLOB_RECURSE ALL_CXX_SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.[ch]pp)

find_program(CLANG_FORMAT "clang-format")
if(NOT CLANG_FORMAT)
  message(WARNING "🔴 clang-format is not found!")
else()
  message(STATUS "🟢 clang-format found, 'format' target created.")
  add_custom_target(
    format
    COMMAND clang-format
    -i
    -style=file
    ${ALL_CXX_SOURCE_FILES}
    )
endif()
