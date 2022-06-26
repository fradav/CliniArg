cmake_minimum_required(VERSION 3.0.0)
include(pmm.cmake)
project(CLI-proto VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

IF(MSVC)
  add_compile_options(/permissive-)
ENDIF()

pmm(
  VCPKG
    REVISION cef0b3ec767df6e83806899fe9525f6cf8d7bc91
    REQUIRES range-v3 catch2
)

find_package(range-v3 CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
include(Catch)

include(CTest)
enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB test_files test/*.cpp)
foreach(filename ${test_files})
  get_filename_component(target ${filename} NAME_WE)
  add_executable(${target} ${filename})
  target_link_libraries(${target} PRIVATE Catch2::Catch2WithMain range-v3)
  catch_discover_tests(${target} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test)
endforeach(filename)
