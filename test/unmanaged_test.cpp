/*
 * Copyright (c) 2012, Willow Garage, Inc.
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
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
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

#include <memory>
#include <string>
#include <tuple>

#include "class_loader/class_loader.hpp"
#include "class_loader/multi_library_class_loader.hpp"

#include "gtest/gtest.h"

#include "./base.hpp"

// createUnmanagedInstance() disables on-demand unloading for the whole process, so these tests
// run in their own executable.

const std::string LIBRARY_1 = class_loader::systemLibraryFormat("class_loader_TestPlugins1");  // NOLINT
const std::string LIBRARY_3 = class_loader::systemLibraryFormat("class_loader_TestPlugins3");  // NOLINT

TEST(ClassLoaderUnmanagedTest, basicLoadUnmanaged) {
  class_loader::ClassLoader loader1(LIBRARY_1, false);
  Base * unmanaged_instance = loader1.createUnmanagedInstance<Base>("Dog");
  ASSERT_NE(unmanaged_instance, nullptr);
  unmanaged_instance->saySomething();
  delete unmanaged_instance;
}

TEST(ClassLoaderUnmanagedTest, constructorLoadUnmanaged) {
  class_loader::ClassLoader loader1(LIBRARY_3, false);
  BaseWithInterfaceCtor * unmanaged_instance =
    loader1.createUnmanagedInstance<BaseWithInterfaceCtor>(
    "Identity", "identity", std::make_unique<int>(1));
  ASSERT_NE(unmanaged_instance, nullptr);
  ASSERT_EQ(unmanaged_instance->get_number(), 1);
  delete unmanaged_instance;
}

TEST(ClassLoaderUnmanagedTest, multiLibraryClassLoaderUnmanaged) {
  class_loader::MultiLibraryClassLoader loader(false);
  loader.loadLibrary(LIBRARY_1);
  loader.loadLibrary(LIBRARY_3);

  std::unique_ptr<Base> dog(loader.createUnmanagedInstance<Base>("Dog"));
  EXPECT_EQ("Bark", dog->saySomething());
  std::unique_ptr<Base> cat(loader.createUnmanagedInstance<Base>("Cat", LIBRARY_1));
  EXPECT_EQ("Meow", cat->saySomething());
  std::unique_ptr<BaseWithInterfaceCtor> doubled(
    loader.createUnmanagedInstance<BaseWithInterfaceCtor>(
      "Double", LIBRARY_3, "double", std::make_unique<int>(4)));
  EXPECT_EQ(8, doubled->get_number());

  EXPECT_THROW(
    std::ignore = loader.createUnmanagedInstance<Base>("Dog2"),
    class_loader::CreateClassException);
  EXPECT_THROW(
    std::ignore = loader.createUnmanagedInstance<Base>("Dog", "not_a_loaded_library"),
    class_loader::NoClassLoaderExistsException);
}

int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
