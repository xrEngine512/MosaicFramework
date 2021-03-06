cmake_minimum_required(VERSION 3.4)
project(interop)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

set(INTEROP_ROOT ${PROJECT_SOURCE_DIR})

include(interop_setup_environment)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
      "No build type specified, options are: Debug Release
RelWithDebInfo MinSizeRel. Using Debug by default"
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

option(JS_V8_SUPPORT "Build with JavaScript support (V8 engine dependency)" ON)
option(JS_NODE_SUPPORT "Build with NodeJS support" OFF)

enable_testing()

add_subdirectory(common)
add_subdirectory(os_abstraction)
add_subdirectory(framework)
add_subdirectory(tests)
