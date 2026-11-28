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

// Tests class_loader::impl::SharedLibrary, i.e., the built-in implementation or
// rcpputils::SharedLibrary (depending on CLASS_LOADER_USE_RCPPUTILS).

#include <stdexcept>
#include <string>
#include <tuple>

#include "class_loader/class_loader.hpp"
#include "class_loader/shared_library.hpp"

#include "gtest/gtest.h"

using class_loader::impl::SharedLibrary;

// A library without plugins, so it can be unloaded safely.
const std::string LIBRARY =  // NOLINT
  class_loader::systemLibraryFormat("class_loader_TestSharedLibrary");

TEST(SharedLibraryTest, loadAndUnload) {
  SharedLibrary library(LIBRARY);
  const std::string path = library.get_library_path();
  EXPECT_NE(std::string::npos, path.find("class_loader_TestSharedLibrary")) << path;

  library.unload_library();
  EXPECT_THROW(library.unload_library(), std::runtime_error);
  EXPECT_THROW(std::ignore = library.get_library_path(), std::runtime_error);
}

TEST(SharedLibraryTest, unloadOnDestruction) {
  {
    SharedLibrary library(LIBRARY);
  }
  // Loading again works after the destructor has unloaded the library.
  SharedLibrary library(LIBRARY);
  EXPECT_FALSE(library.get_library_path().empty());
}

TEST(SharedLibraryTest, nonexistentLibrary) {
  EXPECT_THROW(
    SharedLibrary{class_loader::systemLibraryFormat("class_loader_nonexistent")},
    std::runtime_error);
}

TEST(SharedLibraryTest, platformLibraryName) {
#ifdef _WIN32
  const std::string suffix = ".dll";
#elif defined(__APPLE__)
  const std::string suffix = ".dylib";
#else
  const std::string suffix = ".so";
#endif
  EXPECT_TRUE(class_loader::impl::get_platform_library_name("foo").ends_with("foo" + suffix));
  EXPECT_TRUE(
    class_loader::impl::get_platform_library_name("foo", true).ends_with("food" + suffix));
  EXPECT_EQ(
    class_loader::impl::get_platform_library_name("foo"), class_loader::systemLibraryFormat("foo"));
}
