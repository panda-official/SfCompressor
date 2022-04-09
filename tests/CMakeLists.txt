add_executable(unit_tests sf_compressor_test.cc)
target_link_libraries(unit_tests PRIVATE ${SFC_TARGET_NAME})
target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)

# Discover tests
#catch_discover_tests(unit_tests)