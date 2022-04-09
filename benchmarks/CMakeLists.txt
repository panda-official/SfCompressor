add_executable(benchmarks sf_benchmarks.cc)
target_link_libraries(benchmarks PRIVATE Catch2::Catch2WithMain ${SFC_TARGET_NAME})
