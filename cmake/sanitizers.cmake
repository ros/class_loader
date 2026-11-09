# Sets LIST_OF_SANITIZERS in the caller's scope to the comma-separated sanitizers selected by the
# ENABLE_SANITIZER_* options (empty if none, or if the compiler isn't GCC or Clang).
function(class_loader_get_sanitizers)
  set(SANITIZERS "")

  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    option(ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    if(ENABLE_SANITIZER_ADDRESS)
      list(APPEND SANITIZERS "address")
    endif()

    option(ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    if(ENABLE_SANITIZER_LEAK)
      list(APPEND SANITIZERS "leak")
    endif()

    option(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR "Enable undefined behavior sanitizer" OFF)
    if(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR)
      list(APPEND SANITIZERS "undefined")
    endif()

    option(ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    if(ENABLE_SANITIZER_THREAD)
      if("address" IN_LIST SANITIZERS OR "leak" IN_LIST SANITIZERS)
        message(WARNING "Thread sanitizer does not work with Address and Leak sanitizer enabled")
      else()
        list(APPEND SANITIZERS "thread")
      endif()
    endif()

    option(ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    if(ENABLE_SANITIZER_MEMORY AND CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
      message(WARNING "Memory sanitizer requires all the code (including libc++) \
        to be MSan-instrumented otherwise it reports false positives")
      if("address" IN_LIST SANITIZERS
          OR "thread" IN_LIST SANITIZERS
          OR "leak" IN_LIST SANITIZERS)
        message(WARNING "Memory sanitizer does not work with Address, Thread and Leak sanitizer enabled")
      else()
        list(APPEND SANITIZERS "memory")
      endif()
    endif()
  endif()

  list(JOIN SANITIZERS "," LIST_OF_SANITIZERS)
  set(LIST_OF_SANITIZERS "${LIST_OF_SANITIZERS}" PARENT_SCOPE)
endfunction()

# Sets the compile and link options for the selected sanitizers (and ENABLE_COVERAGE) on the
# current directory, so they apply to every target defined after this call, here and in
# subdirectories. Call it before defining any targets, so the library is instrumented as well as
# the tests and test plugins. Directory options aren't exported, so consumers of the installed
# library are unaffected.
function(class_loader_enable_sanitizers)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" OFF)
    if(ENABLE_COVERAGE)
      add_compile_options(--coverage -O0 -g -fno-omit-frame-pointer)
      add_link_options(--coverage)
    endif()
  endif()

  class_loader_get_sanitizers()
  if(NOT LIST_OF_SANITIZERS STREQUAL "")
    message(STATUS "class_loader: building with -fsanitize=${LIST_OF_SANITIZERS}")
    set(flags -fsanitize=${LIST_OF_SANITIZERS} -fno-omit-frame-pointer)
    if(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR)
      # Make undefined behavior fail the test instead of only printing a diagnostic.
      list(APPEND flags -fno-sanitize-recover=undefined)
    endif()
    add_compile_options(${flags})
    add_link_options(${flags})
  endif()
endfunction()

# Adds the options for the selected sanitizers (and ENABLE_COVERAGE) to the interface of target
# project_name. Kept for compatibility, since this file is installed; this package itself uses
# class_loader_enable_sanitizers().
function(enable_sanitizers project_name)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" OFF)
    if(ENABLE_COVERAGE)
      target_compile_options(${project_name} INTERFACE --coverage -O0 -g -fno-omit-frame-pointer)
      target_link_libraries(${project_name} INTERFACE --coverage)
    endif()
  endif()

  class_loader_get_sanitizers()
  if(NOT LIST_OF_SANITIZERS STREQUAL "")
    target_compile_options(${project_name} INTERFACE -fsanitize=${LIST_OF_SANITIZERS})
    target_link_options(${project_name} INTERFACE -fsanitize=${LIST_OF_SANITIZERS})
  endif()
endfunction()
