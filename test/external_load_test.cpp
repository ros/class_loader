/*
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2026, Open Source Robotics Foundation, Inc.
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
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <algorithm>
#include <string>
#include <tuple>

#include "class_loader/class_loader.hpp"
#include "rcpputils/shared_library.hpp"

#include "gtest/gtest.h"

#include "./base.hpp"

// Plugin libraries opened by means other than class_loader, e.g. linked into the executable or
// opened with dlopen(). After that, class_loader never unloads any library in the process, so
// these tests run in their own executable.

const std::string LIBRARY_1 = class_loader::systemLibraryFormat("class_loader_TestPlugins1");  // NOLINT
const std::string LIBRARY_2 = class_loader::systemLibraryFormat("class_loader_TestPlugins2");  // NOLINT

// Opens LIBRARY_1 outside class_loader, once for the whole process. The handle is leaked on
// purpose: closing the library at exit would unmap the code that class_loader's global factory
// maps, destroyed later, still need to delete the library's MetaObjects.
void openLibrary1Externally()
{
  static const auto * library = new rcpputils::SharedLibrary(LIBRARY_1);
  (void)library;
  ASSERT_TRUE(class_loader::impl::hasANonPurePluginLibraryBeenOpened());
}

// A ClassLoader with an empty path stands for libraries linked into the executable, and can use
// the classes of any library opened outside class_loader.
TEST(ClassLoaderExternalLoadTest, linkedClassLoader) {
  ASSERT_NO_FATAL_FAILURE(openLibrary1Externally());
  class_loader::ClassLoader loader("");
  const auto classes = loader.getAvailableClasses<Base>();
  EXPECT_NE(classes.end(), std::ranges::find(classes, "Dog"));
  EXPECT_NE(classes.end(), std::ranges::find(classes, "Cat"));
  EXPECT_TRUE(loader.isClassAvailable<Base>("Dog"));
  EXPECT_EQ("Bark", loader.createInstance<Base>("Dog")->saySomething());
  EXPECT_EQ("Meow", loader.createUniqueInstance<Base>("Cat")->saySomething());
  EXPECT_THROW(
    std::ignore = loader.createInstance<Base>("Robot"), class_loader::CreateClassException);
}

// A ClassLoader for the same library can still use its classes.
TEST(ClassLoaderExternalLoadTest, loaderForExternallyOpenedLibrary) {
  ASSERT_NO_FATAL_FAILURE(openLibrary1Externally());
  for (bool on_demand : {false, true}) {
    SCOPED_TRACE(on_demand ? "on-demand" : "non-on-demand");
    class_loader::ClassLoader loader(LIBRARY_1, on_demand);
    EXPECT_TRUE(loader.isClassAvailable<Base>("Dog"));
    EXPECT_EQ("Bark", loader.createInstance<Base>("Dog")->saySomething());
    EXPECT_EQ("Meow", loader.createUniqueInstance<Base>("Cat")->saySomething());
  }
}

// Other libraries still load and work, but are no longer unloaded.
TEST(ClassLoaderExternalLoadTest, otherLibrariesAreNotUnloaded) {
  ASSERT_NO_FATAL_FAILURE(openLibrary1Externally());
  // Robot's factory keeps this loader as its owner, since the library is never unloaded. The
  // loader is leaked so that no other ClassLoader can reuse its address and take over Robot.
  static auto * loader = new class_loader::ClassLoader(LIBRARY_2, false);
  EXPECT_EQ("Beep boop", loader->createInstance<Base>("Robot")->saySomething());
  loader->unloadLibrary();
  EXPECT_TRUE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_2));
}

int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
