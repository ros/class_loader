if(ament_cmake_FOUND)
  find_package(ament_cmake_gtest REQUIRED)
else()
  find_package(GTest REQUIRED)

  # Minimal replacement for ament_add_gtest() without ament, using GoogleTest from the system.
  # Only APPEND_LIBRARY_DIRS is supported. These dirs are prepended (not appended as by ament),
  # so that the tests do not pick up an installed class_loader.
  function(ament_add_gtest target)
    cmake_parse_arguments(ARG "" "" "APPEND_LIBRARY_DIRS" ${ARGN})
    add_executable(${target} ${ARG_UNPARSED_ARGUMENTS})
    target_link_libraries(${target} GTest::gtest_main)
    add_test(NAME ${target} COMMAND ${target})
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
    string(REPLACE ";" "${_sep}" _dirs "${ARG_APPEND_LIBRARY_DIRS};$ENV{${_var}}")
    set_tests_properties(${target} PROPERTIES ENVIRONMENT "${_var}=${_dirs}")
  endfunction()
endif()

include("../cmake/class_loader_hide_library_symbols.cmake")

add_library(${PROJECT_NAME}_TestPlugins1 EXCLUDE_FROM_ALL SHARED plugins1.cpp)
target_link_libraries(${PROJECT_NAME}_TestPlugins1 ${PROJECT_NAME})
class_loader_hide_library_symbols(${PROJECT_NAME}_TestPlugins1)

add_library(${PROJECT_NAME}_TestPlugins2 EXCLUDE_FROM_ALL SHARED plugins2.cpp)
target_link_libraries(${PROJECT_NAME}_TestPlugins2 ${PROJECT_NAME})
class_loader_hide_library_symbols(${PROJECT_NAME}_TestPlugins2)

add_library(${PROJECT_NAME}_TestPlugins3 EXCLUDE_FROM_ALL SHARED plugins3.cpp)
target_link_libraries(${PROJECT_NAME}_TestPlugins3 ${PROJECT_NAME})
class_loader_hide_library_symbols(${PROJECT_NAME}_TestPlugins3)

# These plugins are loaded using dlopen() with RTLD_GLOBAL in utest and may cause side effects
# in other tests if they are used elsewhere.
add_library(${PROJECT_NAME}_TestGlobalPlugins EXCLUDE_FROM_ALL SHARED global_plugins.cpp)
target_link_libraries(${PROJECT_NAME}_TestGlobalPlugins ${PROJECT_NAME})
class_loader_hide_library_symbols(${PROJECT_NAME}_TestGlobalPlugins)

if(WIN32)
  set(append_library_dirs "$<TARGET_FILE_DIR:${PROJECT_NAME}>;$<TARGET_FILE_DIR:${PROJECT_NAME}_TestPlugins1>")
else()
  set(append_library_dirs "${CMAKE_CURRENT_BINARY_DIR}")
endif()

ament_add_gtest(${PROJECT_NAME}_utest utest.cpp
  APPEND_LIBRARY_DIRS "${append_library_dirs}"
)

if(TARGET ${PROJECT_NAME}_utest)
  if(ament_cmake_FOUND)
    target_include_directories(${PROJECT_NAME}_utest
      PUBLIC "../include")
    target_link_libraries(${PROJECT_NAME}_utest
      console_bridge::console_bridge)
  else()
    target_include_directories(${PROJECT_NAME}_utest
      PUBLIC "../include" ${console_bridge_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME}_utest)
  endif()
  target_link_libraries(${PROJECT_NAME}_utest ${PROJECT_NAME})

  if(NOT WIN32)
    target_link_libraries(${PROJECT_NAME}_utest pthread)
  endif()
  add_dependencies(${PROJECT_NAME}_utest
    ${PROJECT_NAME}_TestPlugins1
    ${PROJECT_NAME}_TestPlugins2
    ${PROJECT_NAME}_TestPlugins3
    ${PROJECT_NAME}_TestGlobalPlugins)
endif()

ament_add_gtest(${PROJECT_NAME}_unmanaged_test unmanaged_test.cpp
  APPEND_LIBRARY_DIRS "${append_library_dirs}"
)
if(TARGET ${PROJECT_NAME}_unmanaged_test)
  target_include_directories(${PROJECT_NAME}_unmanaged_test PUBLIC "../include")
  target_link_libraries(${PROJECT_NAME}_unmanaged_test ${PROJECT_NAME})
  add_dependencies(${PROJECT_NAME}_unmanaged_test
    ${PROJECT_NAME}_TestPlugins1
    ${PROJECT_NAME}_TestPlugins2
    ${PROJECT_NAME}_TestPlugins3)
endif()

ament_add_gtest(${PROJECT_NAME}_external_load_test external_load_test.cpp
  APPEND_LIBRARY_DIRS "${append_library_dirs}"
)
if(TARGET ${PROJECT_NAME}_external_load_test)
  target_include_directories(${PROJECT_NAME}_external_load_test PUBLIC "../include")
  target_link_libraries(${PROJECT_NAME}_external_load_test ${PROJECT_NAME})
  add_dependencies(${PROJECT_NAME}_external_load_test
    ${PROJECT_NAME}_TestPlugins1
    ${PROJECT_NAME}_TestPlugins2)
endif()

add_subdirectory(fviz_case_study)
