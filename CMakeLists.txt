cmake_minimum_required(VERSION 2.4.6)

include_directories(include)
add_library(Plugins SHARED src/class_loader.cpp src/meta_object.cpp src/plugins.cpp src/plugins_core.cpp)
target_link_libraries(Plugins dl PocoFoundation)
