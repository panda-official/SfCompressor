cmake_minimum_required(VERSION 3.16)

project(sf_compressor)

set(SFC_TARGET_NAME ${PROJECT_NAME})

# Create sfc target
add_library(${SFC_TARGET_NAME} sources/sf_compressor.cpp)

target_compile_features(${SFC_TARGET_NAME} PRIVATE cxx_std_17)

# Include paths
target_include_directories(${SFC_TARGET_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)