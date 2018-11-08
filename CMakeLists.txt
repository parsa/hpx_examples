cmake_minimum_required(VERSION 3.3.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(hpxapp1 CXX)
find_package(HPX REQUIRED)

find_package(blaze NO_CMAKE_PACKAGE_REGISTRY)
if(NOT blaze_FOUND)
    message(FATAL_ERROR "Blaze could not be found. Please specify blaze_DIR to assist locating it.")
endif()

add_hpx_executable(
    app1
    SOURCES prog.cpp
    COMPONENT_DEPENDENCIES iostreams
    DEPENDENCIES blaze::blaze
)

target_compile_definitions(app1_exe PRIVATE BLAZE_USE_HPX_THREADS)
if (MSVC)
    # windows.h conflicts with STL's min and max
    target_compile_definitions(app1_exe PRIVATE NOMINMAX)

    # warning C4146: unary minus operator applied to unsigned type, result still unsigned
    target_compile_options(app1_exe PRIVATE /wd4146)
    # warning C4244: conversion from 'uint64_t' to 'int', possible loss of data
    target_compile_options(app1_exe PRIVATE /wd4244)
    # warning C4267: conversion from 'size_t' to 'int', possible loss of data
    target_compile_options(app1_exe PRIVATE /wd4267)
    # warning C4552: '*': operator has no effect; expected operator with side-effect
    target_compile_options(app1_exe PRIVATE /wd4552)
    # warning C4805: '==': unsafe mix of type 'const bool' and type 'const uint8_t' in operation
    target_compile_options(app1_exe PRIVATE /wd4805)
endif()
