# Checks an installed class_loader built without ament (CLASS_LOADER_IGNORE_AMENT=ON) by building
# one of the tests against it, once using its CMake package configuration and once using its
# pkg-config file (if pkg-config is available and not on Windows).
cmake_minimum_required(VERSION 3.20)

project(class_loader_standalone_test CXX)

enable_testing()

find_package(class_loader REQUIRED)
find_package(GTest REQUIRED)
if(NOT WIN32)
  find_package(PkgConfig)
endif()

# The plugin libraries need the names expected by the test.
foreach(_i 1 2)
  add_library(class_loader_TestPlugins${_i} SHARED ../plugins${_i}.cpp)
  target_link_libraries(class_loader_TestPlugins${_i} class_loader::class_loader)
  class_loader_hide_library_symbols(class_loader_TestPlugins${_i})
endforeach()

set(_variants cmake)
add_executable(external_load_test_cmake ../external_load_test.cpp)
target_link_libraries(external_load_test_cmake class_loader::class_loader GTest::gtest_main)

if(PKG_CONFIG_FOUND)
  pkg_check_modules(class_loader_pc REQUIRED IMPORTED_TARGET class_loader)
  list(APPEND _variants pkgconfig)
  add_executable(external_load_test_pkgconfig ../external_load_test.cpp)
  target_link_libraries(external_load_test_pkgconfig
    PkgConfig::class_loader_pc GTest::gtest_main)
  # pkg-config cannot express the C++ standard required by the headers.
  target_compile_features(external_load_test_pkgconfig PRIVATE cxx_std_20)
endif()

# The tests load the plugin libraries by name, and need to find class_loader at runtime.
if(WIN32)
  set(_var PATH)
  set(_sep "\\;")
elseif(APPLE)
  set(_var DYLD_LIBRARY_PATH)
  set(_sep ":")
else()
  set(_var LD_LIBRARY_PATH)
  set(_sep ":")
endif()
set(_dirs
  "$<TARGET_FILE_DIR:class_loader_TestPlugins1>"
  "$<TARGET_FILE_DIR:class_loader::class_loader>"
  "$ENV{${_var}}")
string(REPLACE ";" "${_sep}" _dirs "${_dirs}")

foreach(_variant ${_variants})
  add_test(NAME external_load_test_${_variant} COMMAND external_load_test_${_variant})
  set_tests_properties(external_load_test_${_variant} PROPERTIES ENVIRONMENT "${_var}=${_dirs}")
endforeach()
