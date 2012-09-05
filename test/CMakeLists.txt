include_directories(../include)
add_library(Plugins1 SHARED plugins1.cpp)
add_library(Plugins2 SHARED plugins2.cpp)
link_directories(~/Source/plugins/build)
add_executable(test driver.cpp)
target_link_libraries(test Plugins dl PocoFoundation)


