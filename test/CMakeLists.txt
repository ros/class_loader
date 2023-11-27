find_package(ament_cmake_gtest REQUIRED)

include("../cmake/class_loader_hide_library_symbols.cmake")

add_library(${PROJECT_NAME}_TestPlugins1 EXCLUDE_FROM_ALL SHARED plugins1.cpp)
target_link_libraries(${PROJECT_NAME}_TestPlugins1 ${PROJECT_NAME})
class_loader_hide_library_symbols(${PROJECT_NAME}_TestPlugins1)

add_library(${PROJECT_NAME}_TestPlugins2 EXCLUDE_FROM_ALL SHARED plugins2.cpp)
target_link_libraries(${PROJECT_NAME}_TestPlugins2 ${PROJECT_NAME})
class_loader_hide_library_symbols(${PROJECT_NAME}_TestPlugins2)

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
  target_link_libraries(${PROJECT_NAME}_utest ${PROJECT_NAME})
  if(NOT WIN32)
    target_link_libraries(${PROJECT_NAME}_utest pthread)
  endif()
  add_dependencies(${PROJECT_NAME}_utest
    ${PROJECT_NAME}_TestPlugins1
    ${PROJECT_NAME}_TestPlugins2
    ${PROJECT_NAME}_TestGlobalPlugins)
endif()

ament_add_gtest(${PROJECT_NAME}_unique_ptr_test unique_ptr_test.cpp
  APPEND_LIBRARY_DIRS "${append_library_dirs}"
)
if(TARGET ${PROJECT_NAME}_unique_ptr_test)
  target_link_libraries(${PROJECT_NAME}_unique_ptr_test ${PROJECT_NAME})
  if(NOT WIN32)
    target_link_libraries(${PROJECT_NAME}_unique_ptr_test pthread)
  endif()
  add_dependencies(${PROJECT_NAME}_unique_ptr_test
    ${PROJECT_NAME}_TestPlugins1
    ${PROJECT_NAME}_TestPlugins2)
endif()

add_subdirectory(fviz_case_study)
