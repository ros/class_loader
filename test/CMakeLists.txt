find_package(ament_cmake_gtest REQUIRED)

include("../cmake/class_loader_hide_library_symbols.cmake")

add_library(${PROJECT_NAME}_TestPlugins1 EXCLUDE_FROM_ALL SHARED plugins1.cpp)
target_include_directories(${PROJECT_NAME}_TestPlugins1
  PUBLIC "../include" ${console_bridge_INCLUDE_DIRS} ${Poco_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}_TestPlugins1 ${PROJECT_NAME})
class_loader_hide_library_symbols(${PROJECT_NAME}_TestPlugins1)

add_library(${PROJECT_NAME}_TestPlugins2 EXCLUDE_FROM_ALL SHARED plugins2.cpp)
target_include_directories(${PROJECT_NAME}_TestPlugins2
  PUBLIC "../include" ${console_bridge_INCLUDE_DIRS} ${Poco_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}_TestPlugins2 ${PROJECT_NAME})
class_loader_hide_library_symbols(${PROJECT_NAME}_TestPlugins2)

ament_add_gtest(${PROJECT_NAME}_utest utest.cpp)
if(TARGET ${PROJECT_NAME}_utest)
  target_include_directories(${PROJECT_NAME}_utest
    PUBLIC "../include" ${console_bridge_INCLUDE_DIRS} ${Poco_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME}_utest ${PROJECT_NAME} ${Poco_LIBRARIES})
  if(NOT WIN32)
    target_link_libraries(${PROJECT_NAME}_utest pthread)
  endif()
  add_dependencies(${PROJECT_NAME}_utest ${PROJECT_NAME}_TestPlugins1 ${PROJECT_NAME}_TestPlugins2)
endif()

add_subdirectory(fviz_case_study)
