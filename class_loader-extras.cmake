list(INSERT CMAKE_MODULE_PATH 0 "${class_loader_DIR}/Modules")
find_package(Poco REQUIRED COMPONENTS Foundation)

include("${class_loader_DIR}/class_loader_hide_library_symbols.cmake")
