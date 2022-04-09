cmake_minimum_required(VERSION 3.16)

project(sf_compressor VERSION 1.0.1)

option(SFC_BUILD_TESTS "Enable unit tests" OFF)
option(SFC_BUILD_EXAMPLES "Enable examples" OFF)
option(SFC_BUILD_BENCHMARKS "Enable benchmarks" OFF)

set(SFC_TARGET_NAME ${PROJECT_NAME})

# Create sfc target
add_library(${SFC_TARGET_NAME} sources/sf_compressor.cc)

target_compile_features(${SFC_TARGET_NAME} PUBLIC cxx_std_17)

# Include paths
target_include_directories(${SFC_TARGET_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Catch2 installation
include(FetchContent)
if (SFC_BUILD_TESTS OR SFC_BUILD_BENCHMARKS)
    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.0.0-preview3)

    FetchContent_MakeAvailable(Catch2)

    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

    enable_testing()
    include(CTest)
    include(Catch)
endif ()


if (SFC_BUILD_TESTS)
    message(STATUS "Unit tests enabled")
    add_subdirectory(tests)
endif ()

if (SFC_BUILD_BENCHMARKS)
    message(STATUS "Benchmarks enabled")
    add_subdirectory(benchmarks)
endif ()

if (SFC_BUILD_EXAMPLES)
    message(STATUS "Examples enabled")
    add_subdirectory(examples)
endif ()


# Install rules
include(GNUInstallDirs)

# Create package targets file
install(TARGETS ${SFC_TARGET_NAME} EXPORT ${SFC_TARGET_NAME}-target)
install(EXPORT ${SFC_TARGET_NAME}-target
        FILE ${SFC_TARGET_NAME}-targets.cmake
        NAMESPACE ${SFC_TARGET_NAME}::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

include(CMakePackageConfigHelpers)

# Create package config file
configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/${SFC_TARGET_NAME}-config.cmake.in"
        "${SFC_TARGET_NAME}-config.cmake"
        INSTALL_DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
)
# Create package version file
write_basic_package_version_file(${SFC_TARGET_NAME}-config-version.cmake COMPATIBILITY ExactVersion)

# Install cmake files
install(FILES "${PROJECT_BINARY_DIR}/${SFC_TARGET_NAME}-config.cmake"
        "${PROJECT_BINARY_DIR}/${SFC_TARGET_NAME}-config-version.cmake"
        DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

# Install headers
install(DIRECTORY ${PROJECT_SOURCE_DIR}/sf_compressor TYPE INCLUDE
        PATTERN "*.md" EXCLUDE)