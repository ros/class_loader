include("${class_loader_DIR}/class_loader_hide_library_symbols.cmake")

find_package(poco_vendor REQUIRED)
find_package(Poco REQUIRED COMPONENTS Foundation)
list(APPEND class_loader_LIBRARIES ${Poco_LIBRARIES})
