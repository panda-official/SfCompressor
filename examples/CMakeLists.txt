add_executable(example example.cc)
target_link_libraries(example ${SFC_TARGET_NAME})


add_compile_definitions(example EXAMPLE_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}")