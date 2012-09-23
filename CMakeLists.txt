cmake_minimum_required(VERSION 2.8)
project(class_loader)

find_package(catkin REQUIRED)
catkin_package()

find_package(Boost REQUIRED COMPONENTS thread)
find_package(console_bridge REQUIRED)

catkin_package_export(
  INCLUDE_DIRS include
  LIBRARIES poco_lite boost_fs_wrapper tinyxml
  DEPENDS console_bridge
)

include_directories(include ${catkin_INCLUDE_DIRS} ${console_bridge_INCLUDE_DIRS})

set(${PROJECT_NAME}_SRCS
  src/class_loader.cpp
  src/class_loader_core.cpp
  src/meta_object.cpp
  src/multi_library_class_loader.cpp
)
add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SRCS})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${console_bridge_LIBRARIES} dl PocoFoundation tinyxml)

install(FILES cmake/hide_plugin_symbols.cmake
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/cmake)
install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION})
install(DIRECTORY include/class_loader/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})

add_library(${PROJECT_NAME}_TestPlugins1 EXCLUDE_FROM_ALL test/plugins1.cpp)
target_link_libraries(${PROJECT_NAME}_TestPlugins1 ${PROJECT_NAME})
add_library(${PROJECT_NAME}_TestPlugins2 EXCLUDE_FROM_ALL test/plugins2.cpp)
target_link_libraries(${PROJECT_NAME}_TestPlugins2 ${PROJECT_NAME})

catkin_add_gtest(${PROJECT_NAME}_utest test/utest.cpp)
target_link_libraries(${PROJECT_NAME}_utest ${PROJECT_NAME} ${Boost_LIBRARIES} ${console_bridge_LIBRARIES} dl PocoFoundation tinyxml)
add_dependencies(${PROJECT_NAME}_utest ${PROJECT_NAME}_TestPlugins1 ${PROJECT_NAME}_TestPlugins2)
