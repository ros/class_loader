cmake_minimum_required(VERSION 2.8.3)
project(class_loader)

set(Poco_DIR cmake)

find_package(Poco REQUIRED COMPONENTS Foundation)
find_package(Boost REQUIRED COMPONENTS thread system)
find_package(catkin REQUIRED)
find_package(console_bridge REQUIRED)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
  DEPENDS Boost Poco console_bridge
  CFG_EXTRAS class_loader-extras.cmake
)

include_directories(include ${console_bridge_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${Poco_INCLUDE_DIRS})

set(${PROJECT_NAME}_SRCS
  src/class_loader.cpp
  src/class_loader_core.cpp
  src/meta_object.cpp
  src/multi_library_class_loader.cpp
)
add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SRCS})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${console_bridge_LIBRARIES} ${Poco_LIBRARIES})

install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})
install(DIRECTORY include/class_loader/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})

if(CATKIN_ENABLE_TESTING)
  add_subdirectory(test)
endif()
