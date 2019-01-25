cmake_minimum_required(VERSION 3.0.0)
project(CLI-proto VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

IF(MSVC)
  add_compile_options(/permissive-)
ENDIF()
    
# include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
# conan_basic_setup()

find_package(range-v3 CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)

add_executable(trimandlower-test test/TestTrim.cpp)
target_link_libraries(trimandlower-test Catch2::Catch2 range-v3)

include(CTest)
include(Catch)
catch_discover_tests(trimandlower-test)


