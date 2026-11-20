# Checks an installed class_loader built without ament (CLASS_LOADER_IGNORE_AMENT=ON) by building
# one of the tests against it, once using its CMake package configuration and once using its
# pkg-config file.
cmake_minimum_required(VERSION 3.20)

project(class_loader_standalone_test CXX)

enable_testing()

find_package(class_loader REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(class_loader_pc REQUIRED IMPORTED_TARGET class_loader)
find_package(GTest REQUIRED)

# The plugin libraries need the names expected by the test.
foreach(_i 1 2)
  add_library(class_loader_TestPlugins${_i} SHARED ../plugins${_i}.cpp)
  target_link_libraries(class_loader_TestPlugins${_i} class_loader::class_loader)
  class_loader_hide_library_symbols(class_loader_TestPlugins${_i})
endforeach()

add_executable(external_load_test_cmake ../external_load_test.cpp)
target_link_libraries(external_load_test_cmake class_loader::class_loader GTest::gtest_main)

add_executable(external_load_test_pkgconfig ../external_load_test.cpp)
target_link_libraries(external_load_test_pkgconfig
  PkgConfig::class_loader_pc GTest::gtest_main)
# pkg-config cannot express the C++ standard required by the headers.
target_compile_features(external_load_test_pkgconfig PRIVATE cxx_std_20)

foreach(_variant cmake pkgconfig)
  add_test(NAME external_load_test_${_variant} COMMAND external_load_test_${_variant})
  set_tests_properties(external_load_test_${_variant} PROPERTIES
    ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:$ENV{LD_LIBRARY_PATH}")
endforeach()
