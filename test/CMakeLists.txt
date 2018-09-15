cmake_minimum_required(VERSION 3.5)

find_package(Boost REQUIRED COMPONENTS thread)
find_package(catkin REQUIRED COMPONENTS class_loader)

include_directories(${catkin_INCLUDE_DIRS})

add_library(${PROJECT_NAME}_TestPlugins1 EXCLUDE_FROM_ALL plugins1.cpp)
target_link_libraries(${PROJECT_NAME}_TestPlugins1 ${PROJECT_NAME})
if(WIN32)
  # On Windows, default library runtime output set to CATKIN_GLOBAL_BIN_DESTINATION,
  # change it back to CATKIN_PACKAGE_BIN_DESTINATION so the test can run correctly
  set_target_properties(${PROJECT_NAME}_TestPlugins1 PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CATKIN_DEVEL_PREFIX}/${CATKIN_PACKAGE_BIN_DESTINATION})
endif()
class_loader_hide_library_symbols(${PROJECT_NAME}_TestPlugins1)
add_library(${PROJECT_NAME}_TestPlugins2 EXCLUDE_FROM_ALL plugins2.cpp)
target_link_libraries(${PROJECT_NAME}_TestPlugins2 ${PROJECT_NAME})
if(WIN32)
  # On Windows, default library runtime output set to CATKIN_GLOBAL_BIN_DESTINATION,
  # change it back to CATKIN_PACKAGE_BIN_DESTINATION so the test can run correctly
  set_target_properties(${PROJECT_NAME}_TestPlugins2 PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CATKIN_DEVEL_PREFIX}/${CATKIN_PACKAGE_BIN_DESTINATION})
endif()
class_loader_hide_library_symbols(${PROJECT_NAME}_TestPlugins2)

catkin_add_gtest(${PROJECT_NAME}_utest utest.cpp)
if(TARGET ${PROJECT_NAME}_utest)
  target_link_libraries(${PROJECT_NAME}_utest ${Boost_LIBRARIES} ${class_loader_LIBRARIES})
  add_dependencies(${PROJECT_NAME}_utest ${PROJECT_NAME}_TestPlugins1 ${PROJECT_NAME}_TestPlugins2)
endif()

catkin_add_gtest(${PROJECT_NAME}_shared_ptr_test shared_ptr_test.cpp)
if(TARGET ${PROJECT_NAME}_shared_ptr_test)
  target_link_libraries(${PROJECT_NAME}_shared_ptr_test ${Boost_LIBRARIES} ${class_loader_LIBRARIES})
  add_dependencies(${PROJECT_NAME}_shared_ptr_test ${PROJECT_NAME}_TestPlugins1 ${PROJECT_NAME}_TestPlugins2)
endif()

catkin_add_gtest(${PROJECT_NAME}_unique_ptr_test unique_ptr_test.cpp)
if(TARGET ${PROJECT_NAME}_unique_ptr_test)
  target_link_libraries(${PROJECT_NAME}_unique_ptr_test ${Boost_LIBRARIES} ${class_loader_LIBRARIES})
  add_dependencies(${PROJECT_NAME}_unique_ptr_test ${PROJECT_NAME}_TestPlugins1 ${PROJECT_NAME}_TestPlugins2)
endif()
