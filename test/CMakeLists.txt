Include(FetchContent)

FetchContent_Declare(
        Catch2
        URL https://github.com/catchorg/Catch2/archive/v2.13.4.zip)

FetchContent_MakeAvailable(Catch2)

set(SOURCE_FILES
        sf/sf_compressor_test.cc
        main.cc)

add_executable(sf_compressor_test ${SOURCE_FILES})
target_link_libraries(sf_compressor_test PRIVATE Catch2::Catch2 sf_compressor)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(CTest)
include(Catch)
catch_discover_tests(sf_compressor_test)