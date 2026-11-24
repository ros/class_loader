/*
 * Copyright (c) 2026, DFKI GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holders nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

// Built-in replacement for rcpputils::SharedLibrary, only compiled if class_loader is built
// without rcpputils. It mirrors the behavior of rcutils_load_shared_library(): the library is
// opened with RTLD_LAZY (LoadLibrary on Windows) and its path is resolved where supported.

#include "class_loader/shared_library.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#if defined(_GNU_SOURCE) && defined(__GLIBC__)
#include <link.h>
#define CLASS_LOADER_HAS_DLINFO
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <cstdint>
#endif
#endif

namespace class_loader
{
namespace impl
{

namespace
{

/// Closes the given library handle; returns an error message on failure, an empty string otherwise.
std::string closeLibrary(void * handle)
{
#ifdef _WIN32
  if (!FreeLibrary(static_cast<HMODULE>(handle))) {
    return "FreeLibrary error: " + std::to_string(GetLastError());
  }
#else
  if (dlclose(handle) != 0) {
    const char * error = dlerror();
    return std::string("dlclose error: ") + (error ? error : "unknown error");
  }
#endif
  return "";
}

}  // namespace

SharedLibrary::SharedLibrary(const std::string & library_path)
{
#ifdef _WIN32
  HMODULE module = LoadLibraryA(library_path.c_str());
  if (!module) {
    throw std::runtime_error("LoadLibrary error: " + std::to_string(GetLastError()));
  }
  std::string path(MAX_PATH, '\0');
  while (true) {
    DWORD size = GetModuleFileNameA(module, path.data(), static_cast<DWORD>(path.size()));
    if (size == 0) {
      const std::string error = "GetModuleFileName error: " + std::to_string(GetLastError());
      FreeLibrary(module);
      throw std::runtime_error(error);
    }
    if (size < path.size()) {
      path.resize(size);
      break;
    }
    // The path was truncated, retry with a larger buffer.
    path.resize(path.size() * 2);
  }
  handle_ = module;
  library_path_ = path;
#else
  handle_ = dlopen(library_path.c_str(), RTLD_LAZY);
  if (!handle_) {
    const char * error = dlerror();
    throw std::runtime_error(std::string("dlopen error: ") + (error ? error : "unknown error"));
  }
  library_path_ = library_path;
#ifdef CLASS_LOADER_HAS_DLINFO
  struct link_map * map = nullptr;
  if (dlinfo(handle_, RTLD_DI_LINKMAP, &map) == 0 && map && map->l_name && map->l_name[0]) {
    library_path_ = map->l_name;
  }
#elif defined(__APPLE__)
  // As rcutils: find the loaded image with the same handle (likely near the end of the list).
  // This resolves e.g. a bare file name found via DYLD_LIBRARY_PATH, which pinLibrary() needs.
  for (uint32_t i = _dyld_image_count(); i > 0; --i) {
    const char * name = _dyld_get_image_name(i - 1);
    void * handle = name ? dlopen(name, RTLD_LAZY | RTLD_NOLOAD) : nullptr;
    if (handle) {
      dlclose(handle);  // still loaded through handle_
      if (handle == handle_) {
        library_path_ = name;
        break;
      }
    }
  }
#endif
#endif
}

SharedLibrary::~SharedLibrary()
{
  if (handle_) {
    const std::string error = closeLibrary(handle_);
    if (!error.empty()) {
      std::cerr << error << std::endl;
    }
  }
}

void SharedLibrary::unload_library()
{
  if (!handle_) {
    throw std::runtime_error("Library is not loaded");
  }
  const std::string error = closeLibrary(handle_);
  handle_ = nullptr;
  library_path_.clear();
  if (!error.empty()) {
    throw std::runtime_error(error);
  }
}

std::string SharedLibrary::get_library_path() const
{
  if (!handle_) {
    throw std::runtime_error("Library path is not defined");
  }
  return library_path_;
}

std::string get_platform_library_name(std::string library_name, bool debug)
{
  if (debug) {
    library_name += "d";
  }
#if defined(_WIN32) && !defined(__MINGW64__)
  return library_name + ".dll";
#elif defined(_WIN32)
  return "lib" + library_name + ".dll";
#elif defined(__APPLE__)
  return "lib" + library_name + ".dylib";
#else
  return "lib" + library_name + ".so";
#endif
}

}  // namespace impl
}  // namespace class_loader
