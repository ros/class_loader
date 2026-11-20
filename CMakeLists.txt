cmake_minimum_required(VERSION 3.20)

project(class_loader)

# The C++/C standard comes from ament_ros_defaults (cxx_std_20 / c_std_17),
# linked PRIVATE below so it applies to this library's own translation units
# only and is not propagated to downstream consumers.
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ENABLE_SANITIZER_* options: instrument the library, test plugins and tests alike.
include(cmake/sanitizers.cmake)
class_loader_enable_sanitizers()

set(CLASS_LOADER_IGNORE_AMENT FALSE CACHE BOOL
  "Do not use ament when building this package.")
if(NOT CLASS_LOADER_IGNORE_AMENT)
  find_package(ament_cmake REQUIRED)
  find_package(ament_cmake_ros_core REQUIRED)
  set(explicit_library_type "SHARED")
else()
  set(explicit_library_type "SHARED")
endif()

if(CLASS_LOADER_IGNORE_AMENT)
  set(_use_rcpputils_default OFF)
else()
  set(_use_rcpputils_default ON)
endif()
option(CLASS_LOADER_USE_RCPPUTILS
  "Load libraries with rcpputils (otherwise with a minimal built-in replacement)"
  ${_use_rcpputils_default})

if(NOT CLASS_LOADER_IGNORE_AMENT)
  # Provides console_bridge 0.4.0 on platforms without it.
  find_package(console_bridge_vendor REQUIRED)
endif()
find_package(console_bridge REQUIRED)

if(CLASS_LOADER_USE_RCPPUTILS)
  find_package(rcpputils REQUIRED)
endif()

if(ament_cmake_FOUND)
  ament_export_dependencies(console_bridge)
  if(CLASS_LOADER_USE_RCPPUTILS)
    ament_export_dependencies(rcpputils)
  endif()
endif()

add_library(${PROJECT_NAME} ${explicit_library_type}
  src/class_loader.cpp
  src/class_loader_core.cpp
  src/meta_object.cpp
  src/multi_library_class_loader.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")
# The public headers use C++20 (e.g. concepts), so users need it as well.
# (Previously, this was only propagated through rcpputils.)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
# dlopen() is used directly (and by the built-in shared library wrapper).
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS})
if(CLASS_LOADER_USE_RCPPUTILS)
  target_link_libraries(${PROJECT_NAME} PUBLIC rcpputils::rcpputils)
else()
  target_sources(${PROJECT_NAME} PRIVATE src/shared_library.cpp)
  # Also needed by users, as the public headers select the shared library wrapper with it.
  target_compile_definitions(${PROJECT_NAME} PUBLIC CLASS_LOADER_NO_RCPPUTILS)
endif()

if(ament_cmake_FOUND)
  target_link_libraries(${PROJECT_NAME} PUBLIC
    console_bridge::console_bridge)
  # ament_ros_defaults is an INTERFACE target providing cxx_std_20 / c_std_17.
  # Linked PRIVATE so the standard applies to our own sources without leaking
  # into the exported interface (a SHARED library does not propagate PRIVATE
  # link requirements to consumers).
  target_link_libraries(${PROJECT_NAME} PRIVATE ament_cmake_ros_core::ament_ros_defaults)
  ament_export_targets(${PROJECT_NAME})
else()
  target_include_directories(${PROJECT_NAME}
    PUBLIC ${console_bridge_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} PUBLIC ${console_bridge_LIBRARIES})
  # Standalone (non-ament) build: request C++20 for our own translation units.
  target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
endif()
if(WIN32)
  # Causes the visibility macros to use dllexport rather than dllimport
  # which is appropriate when building the dll but not consuming it.
  target_compile_definitions(${PROJECT_NAME} PRIVATE "CLASS_LOADER_BUILDING_DLL")
endif()

if(BUILD_TESTING)
  find_package(ament_lint_auto QUIET)
  if(ament_lint_auto_FOUND)
    # TODO(mikaelarguedas) add copyright test once the linter accepts /** block comments
    list(APPEND AMENT_LINT_AUTO_EXCLUDE
      ament_cmake_copyright
    )
    ament_lint_auto_find_test_dependencies()

    # Files in test/fviz_case_study do have compatible copyrights
    find_package(ament_cmake_copyright)
    ament_copyright(
      TESTNAME copyright_check_tests_only
      "test/fviz_case_study"
    )
  endif()
  add_subdirectory(test)
endif()

if(ament_cmake_FOUND)
  ament_package(
    CONFIG_EXTRAS "class_loader-extras.cmake"
  )
else()
  set(TARGET_NAME ${PROJECT_NAME})
  file(READ "${CMAKE_CURRENT_SOURCE_DIR}/package.xml" _package_xml)
  string(REGEX MATCH "<version>([^<]*)</version>" _ "${_package_xml}")
  set(PROJECT_VERSION "${CMAKE_MATCH_1}")
  set(PROJECT_DESCRIPTION "Library for loading plugins at runtime")
  set(PKGCONFIG_REQUIRES console_bridge)
  if(NOT CLASS_LOADER_USE_RCPPUTILS)
    set(PKGCONFIG_CFLAGS "-DCLASS_LOADER_NO_RCPPUTILS")
  endif()

  # Prepare and install necessary files to support finding of the library
  # using pkg-config
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
    @ONLY)

  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION lib/pkgconfig)

  # Prepare and install a CMake package configuration, providing the target
  # class_loader::class_loader as in ament builds
  install(EXPORT ${PROJECT_NAME}
    NAMESPACE ${PROJECT_NAME}::
    FILE ${PROJECT_NAME}Targets.cmake
    DESTINATION share/${PROJECT_NAME}/cmake)
  include(CMakePackageConfigHelpers)
  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION share/${PROJECT_NAME}/cmake)
  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion)  # as in ament builds
  install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION share/${PROJECT_NAME}/cmake)
endif()

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME})

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
