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

#ifndef CLASS_LOADER__SHARED_LIBRARY_HPP_
#define CLASS_LOADER__SHARED_LIBRARY_HPP_

/**
 * @note This header is part of the internal implementation. It selects the shared library
 * wrapper used by class_loader: rcpputils::SharedLibrary by default, or a minimal built-in
 * replacement if class_loader was built without rcpputils (CMake option
 * CLASS_LOADER_USE_RCPPUTILS=OFF), in which case CLASS_LOADER_NO_RCPPUTILS is defined for the
 * library and all its users.
 */

#include <string>

#ifdef CLASS_LOADER_NO_RCPPUTILS

#include "class_loader/visibility_control.hpp"

namespace class_loader
{
namespace impl
{

/// Minimal replacement for the subset of rcpputils::SharedLibrary used by class_loader.
class CLASS_LOADER_PUBLIC SharedLibrary
{
public:
  /// Loads the library at the given path (or found by the system's library search).
  /**
   * \throws std::runtime_error if the library cannot be loaded
   */
  explicit SharedLibrary(const std::string & library_path);

  /// Unloads the library, if it has not been unloaded with unload_library() before.
  ~SharedLibrary();

  SharedLibrary(const SharedLibrary &) = delete;
  SharedLibrary & operator=(const SharedLibrary &) = delete;

  /// Unloads the library.
  /**
   * \throws std::runtime_error if the library is not loaded or cannot be unloaded
   */
  void unload_library();

  /// Returns the path of the loaded library (resolved by the system loader where supported).
  /**
   * \throws std::runtime_error if the library is not loaded
   */
  std::string get_library_path() const;

private:
  void * handle_ = nullptr;
  std::string library_path_;
};

/// Same as rcpputils::get_platform_library_name(): adds the platform's prefix and suffix.
/**
 * E.g. "foo" becomes "libfoo.so" on Linux, "libfoo.dylib" on macOS and "foo.dll" on Windows.
 * With debug=true, a "d" is appended to the library name ("libfood.so").
 */
CLASS_LOADER_PUBLIC
std::string get_platform_library_name(std::string library_name, bool debug = false);

}  // namespace impl
}  // namespace class_loader

#else  // CLASS_LOADER_NO_RCPPUTILS

#include "rcpputils/shared_library.hpp"

namespace class_loader
{
namespace impl
{

using SharedLibrary = rcpputils::SharedLibrary;
using rcpputils::get_platform_library_name;

}  // namespace impl
}  // namespace class_loader

#endif  // CLASS_LOADER_NO_RCPPUTILS

#endif  // CLASS_LOADER__SHARED_LIBRARY_HPP_
