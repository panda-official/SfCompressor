set(SOURCE_FILES main.cc
        sf/sf_benchmarks.cc)

add_executable(sf_compressor_benchmarks ${SOURCE_FILES})
target_link_libraries(sf_compressor_benchmarks PRIVATE Catch2::Catch2 sf_compressor)
