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

#include <cstddef>

#ifndef _WIN32
#include <dlfcn.h>
#endif

#include <memory>
#include <string>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "class_loader/class_loader.hpp"
#include "class_loader/multi_library_class_loader.hpp"

#include "console_bridge/console.h"
#include "gtest/gtest.h"

#include "./base.hpp"

const std::string LIBRARY_1 = class_loader::systemLibraryFormat("class_loader_TestPlugins1");  // NOLINT
const std::string LIBRARY_2 = class_loader::systemLibraryFormat("class_loader_TestPlugins2");  // NOLINT
const std::string LIBRARY_3 = class_loader::systemLibraryFormat("class_loader_TestPlugins3");  // NOLINT

// These are loaded with dlopen() and RTLD_GLOBAL in loadUnloadLoadFromGraveyard and may cause
// unexpected side-effects if used elsewhere
const std::string GLOBAL_PLUGINS =  // NOLINT
  class_loader::systemLibraryFormat("class_loader_TestGlobalPlugins");

// Collects console_bridge warnings and errors logged while in scope.
struct WarningCapture : console_bridge::OutputHandler
{
  WarningCapture() {console_bridge::useOutputHandler(this);}
  ~WarningCapture() override {console_bridge::restorePreviousOutputHandler();}
  void log(const std::string & text, console_bridge::LogLevel level, const char *, int) override
  {
    if (level >= console_bridge::CONSOLE_BRIDGE_LOG_WARN) {warnings += text;}
  }
  std::string warnings;
};

// Tests for managed instances run once with createInstance() (std::shared_ptr) and once with
// createUniqueInstance() (ClassLoader::UniquePtr).
struct SharedPtr
{
  template<class Base>
  using Ptr = std::shared_ptr<Base>;

  template<class Base, class Loader, class ... Args>
  static auto create(Loader & loader, const std::string & class_name, Args &&... args)
  {
    return loader.template createInstance<Base>(class_name, std::forward<Args>(args)...);
  }
};

struct UniquePtr
{
  template<class Base>
  using Ptr = class_loader::ClassLoader::UniquePtr<Base>;

  template<class Base, class Loader, class ... Args>
  static auto create(Loader & loader, const std::string & class_name, Args &&... args)
  {
    return loader.template createUniqueInstance<Base>(class_name, std::forward<Args>(args)...);
  }
};

struct InstanceTypeNames
{
  template<class T>
  static std::string GetName([[maybe_unused]] int index)
  {
    if constexpr (std::is_same_v<T, SharedPtr>) {
      return "SharedPtr";
    }
    return "UniquePtr";
  }
};

using InstanceTypes = ::testing::Types<SharedPtr, UniquePtr>;

template<class T>
class ClassLoaderTest : public ::testing::Test {};
TYPED_TEST_SUITE(ClassLoaderTest, InstanceTypes, InstanceTypeNames);

template<class T>
class MultiClassLoaderTest : public ::testing::Test {};
TYPED_TEST_SUITE(MultiClassLoaderTest, InstanceTypes, InstanceTypeNames);

TYPED_TEST(ClassLoaderTest, basicLoad) {
  class_loader::ClassLoader loader1(LIBRARY_1, false);
  ASSERT_NO_THROW(class_loader::impl::printDebugInfoToScreen());
  TypeParam::template create<Base>(loader1, "Cat")->saySomething();
}

TYPED_TEST(ClassLoaderTest, basicLoadTwice) {
  {
    class_loader::ClassLoader loader1(LIBRARY_1, false);
    TypeParam::template create<Base>(loader1, "Cat")->saySomething();
  }
  class_loader::ClassLoader loader1(LIBRARY_1, false);
  TypeParam::template create<Base>(loader1, "Cat")->saySomething();
  ASSERT_NO_THROW(class_loader::impl::printDebugInfoToScreen());
}

TYPED_TEST(ClassLoaderTest, basicLoadTwiceSameTime) {
  class_loader::ClassLoader loader1(LIBRARY_1, false);
  TypeParam::template create<Base>(loader1, "Cat")->saySomething();
  class_loader::ClassLoader loader2(LIBRARY_1, false);
  TypeParam::template create<Base>(loader2, "Cat")->saySomething();
  ASSERT_NO_THROW(class_loader::impl::printDebugInfoToScreen());
}

TYPED_TEST(ClassLoaderTest, constructorLoad) {
  class_loader::ClassLoader loader1(LIBRARY_3, false);
  ASSERT_EQ(
    TypeParam::template create<BaseWithInterfaceCtor>(
      loader1, "Identity", "identity 1", std::make_unique<int>(1))->get_number(), 1);
  ASSERT_EQ(
    TypeParam::template create<BaseWithInterfaceCtor>(
      loader1, "Identity", "identity 2", std::make_unique<int>(10))->get_number(), 10);
  ASSERT_EQ(
    TypeParam::template create<BaseWithInterfaceCtor>(
      loader1, "Double", "double 1", std::make_unique<int>(1))->get_number(), 2);
  ASSERT_EQ(
    TypeParam::template create<BaseWithInterfaceCtor>(
      loader1, "Double", "double 2", std::make_unique<int>(10))->get_number(), 20);
  ASSERT_NO_THROW(class_loader::impl::printDebugInfoToScreen());
}

TYPED_TEST(ClassLoaderTest, correctLazyLoadUnload) {
  ASSERT_FALSE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
  class_loader::ClassLoader loader1(LIBRARY_1, true);
  ASSERT_FALSE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
  ASSERT_FALSE(loader1.isLibraryLoaded());

  {
    auto obj = TypeParam::template create<Base>(loader1, "Cat");
    ASSERT_TRUE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
    ASSERT_TRUE(loader1.isLibraryLoaded());
  }

  // The library will unload automatically when the only plugin object left is destroyed
  ASSERT_FALSE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
}

TYPED_TEST(ClassLoaderTest, nonExistentPlugin) {
  class_loader::ClassLoader loader1(LIBRARY_1, false);
  EXPECT_THROW(
    std::ignore = TypeParam::template create<Base>(loader1, "Bear"),
    class_loader::CreateClassException);
}

TYPED_TEST(ClassLoaderTest, threadSafety) {
  class_loader::ClassLoader loader1(LIBRARY_1);
  ASSERT_TRUE(loader1.isLibraryLoaded());

  // Note: Hard to test thread safety to make sure memory isn't corrupted.
  // The hope is this test is hard enough that once in a while it'll segfault
  // or something if there's some implementation error.
  std::vector<std::thread> client_threads;
  for (size_t c = 0; c < STRESS_TEST_NUM_THREADS; ++c) {
    client_threads.emplace_back(
      [&loader1]() {
        for (const auto & class_name : loader1.getAvailableClasses<Base>()) {
          TypeParam::template create<Base>(loader1, class_name)->saySomething();
        }
      });
  }
  for (auto & client_thread : client_threads) {
    client_thread.join();
  }

  loader1.unloadLibrary();
  ASSERT_FALSE(loader1.isLibraryLoaded());
}

TYPED_TEST(ClassLoaderTest, loadRefCountingLazy) {
  class_loader::ClassLoader loader1(LIBRARY_1, true);
  ASSERT_FALSE(loader1.isLibraryLoaded());

  {
    auto obj = TypeParam::template create<Base>(loader1, "Dog");
    ASSERT_TRUE(loader1.isLibraryLoaded());
  }

  ASSERT_FALSE(loader1.isLibraryLoaded());

  loader1.loadLibrary();
  ASSERT_TRUE(loader1.isLibraryLoaded());

  loader1.loadLibrary();
  ASSERT_TRUE(loader1.isLibraryLoaded());

  loader1.unloadLibrary();
  ASSERT_TRUE(loader1.isLibraryLoaded());

  loader1.unloadLibrary();
  ASSERT_FALSE(loader1.isLibraryLoaded());

  loader1.unloadLibrary();
  ASSERT_FALSE(loader1.isLibraryLoaded());

  loader1.loadLibrary();
  ASSERT_TRUE(loader1.isLibraryLoaded());
}

template<class Instances>
void testMultiClassLoader(bool lazy)
{
  class_loader::MultiLibraryClassLoader loader(lazy);
  loader.loadLibrary(LIBRARY_1);
  loader.loadLibrary(LIBRARY_2);
  loader.loadLibrary(LIBRARY_3);
  for (int i = 0; i < 2; ++i) {
    Instances::template create<Base>(loader, "Cat")->saySomething();
    Instances::template create<Base>(loader, "Dog")->saySomething();
    Instances::template create<Base>(loader, "Robot")->saySomething();
    ASSERT_EQ(
      Instances::template create<BaseWithInterfaceCtor>(
        loader, "Identity", "identity 1", std::make_unique<int>(1))->get_number(), 1);
    ASSERT_EQ(
      Instances::template create<BaseWithInterfaceCtor>(
        loader, "Identity", "identity 2", std::make_unique<int>(10))->get_number(), 10);
    ASSERT_EQ(
      Instances::template create<BaseWithInterfaceCtor>(
        loader, "Double", "double 1", std::make_unique<int>(1))->get_number(), 2);
    ASSERT_EQ(
      Instances::template create<BaseWithInterfaceCtor>(
        loader, "Double", "double 2", std::make_unique<int>(10))->get_number(), 20);
  }
}

TYPED_TEST(MultiClassLoaderTest, lazyLoad) {
  testMultiClassLoader<TypeParam>(true);
}

TYPED_TEST(MultiClassLoaderTest, lazyLoadSecondTime) {
  testMultiClassLoader<TypeParam>(true);
}

TYPED_TEST(MultiClassLoaderTest, nonLazyLoad) {
  testMultiClassLoader<TypeParam>(false);
}

TYPED_TEST(MultiClassLoaderTest, noWarningOnLazyLoad) {
  WarningCapture capture;
  typename TypeParam::template Ptr<Base> cat, dog, rob;
  {
    class_loader::MultiLibraryClassLoader loader(true);
    loader.loadLibrary(LIBRARY_1);
    loader.loadLibrary(LIBRARY_2);

    cat = TypeParam::template create<Base>(loader, "Cat");
    dog = TypeParam::template create<Base>(loader, "Dog");
    rob = TypeParam::template create<Base>(loader, "Robot");
  }
  cat->saySomething();
  dog->saySomething();
  rob->saySomething();
  EXPECT_EQ("", capture.warnings);
}

TYPED_TEST(MultiClassLoaderTest, nonExistentPlugin) {
  class_loader::MultiLibraryClassLoader loader(true);
  loader.loadLibrary(LIBRARY_1);
  EXPECT_THROW(
    std::ignore = TypeParam::template create<Base>(loader, "Cat2"),
    class_loader::ClassLoaderException);
}

// Tests that don't depend on how instances are managed.

TEST(ClassLoaderBasicTest, basicLoadFailures) {
  class_loader::ClassLoader loader1(LIBRARY_1, false);
  class_loader::ClassLoader loader2("", false);
  loader2.loadLibrary();
  EXPECT_THROW(
    class_loader::impl::loadLibrary("LIBRARY_1", &loader1),
    class_loader::LibraryLoadException);
}

TEST(ClassLoaderBasicTest, librariesUsedByClassLoader) {
  class_loader::ClassLoader loader1(LIBRARY_1, false);
  std::vector<std::string> v = class_loader::impl::getAllLibrariesUsedByClassLoader(&loader1);
  ASSERT_EQ(v.size(), 1u);
}

TEST(ClassLoaderBasicTest, correctNonLazyLoadUnload) {
  ASSERT_FALSE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
  class_loader::ClassLoader loader1(LIBRARY_1, false);
  ASSERT_TRUE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
  ASSERT_TRUE(loader1.isLibraryLoaded());
  loader1.unloadLibrary();
  ASSERT_FALSE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
  ASSERT_FALSE(loader1.isLibraryLoaded());
}

TEST(ClassLoaderBasicTest, nonExistentLibrary) {
  EXPECT_THROW(
    class_loader::ClassLoader loader1("libDoesNotExist.so", false),
    class_loader::LibraryLoadException);
}

class InvalidBase
{
};

TEST(ClassLoaderBasicTest, invalidBase) {
  class_loader::ClassLoader loader1(LIBRARY_1, false);
  EXPECT_FALSE(loader1.isClassAvailable<InvalidBase>("Cat"));
  EXPECT_TRUE(loader1.isClassAvailable<Base>("Cat"));
}

TEST(ClassLoaderBasicTest, loadRefCountingNonLazy) {
  class_loader::ClassLoader loader1(LIBRARY_1, false);
  ASSERT_TRUE(loader1.isLibraryLoaded());

  loader1.loadLibrary();
  loader1.loadLibrary();
  ASSERT_TRUE(loader1.isLibraryLoaded());

  loader1.unloadLibrary();
  ASSERT_TRUE(loader1.isLibraryLoaded());

  loader1.unloadLibrary();
  ASSERT_TRUE(loader1.isLibraryLoaded());

  loader1.unloadLibrary();
  ASSERT_FALSE(loader1.isLibraryLoaded());

  loader1.unloadLibrary();
  ASSERT_FALSE(loader1.isLibraryLoaded());

  loader1.loadLibrary();
  ASSERT_TRUE(loader1.isLibraryLoaded());
}

TEST(ClassLoaderBasicTest, isLibraryLoadedScopedToLoader) {
  class_loader::ClassLoader loader1(LIBRARY_1, false);
  ASSERT_TRUE(loader1.isLibraryLoaded());

  class_loader::ClassLoader loader2(LIBRARY_1, true);
  ASSERT_TRUE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
  ASSERT_FALSE(loader2.isLibraryLoaded());

  loader2.loadLibrary();
  ASSERT_TRUE(loader2.isLibraryLoaded());

  loader2.unloadLibrary();
  ASSERT_FALSE(loader2.isLibraryLoaded());
  ASSERT_TRUE(loader1.isLibraryLoaded());
  ASSERT_TRUE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));

  loader1.unloadLibrary();
  ASSERT_FALSE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
}

#ifndef _WIN32
// Not run on Windows because this tests dlopen-specific behavior

// This is a different class name so that static variables in ClassLoader are isolated
TEST(ClassLoaderGraveyardTest, loadUnloadLoadFromGraveyard) {
  // This first load/unload adds the plugin to the graveyard
  try {
    class_loader::ClassLoader loader(GLOBAL_PLUGINS, false);
    loader.createInstance<Base>("Kangaroo")->saySomething();
    loader.unloadLibrary();
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  }

  // Not all platforms use RTLD_GLOBAL as a default, and rcutils doesn't explicitly choose either.
  // In order to invoke graveyard behavior, this needs to be loaded first for global relocation.

  void * handle = dlopen(GLOBAL_PLUGINS.c_str(), RTLD_NOW | RTLD_GLOBAL);
  ASSERT_NE(handle, nullptr);

  // This load will cause system to use globally relocatable library.
  // For testing purposes, this will cause ClassLoader to revive the library from the graveyard.
  try {
    class_loader::ClassLoader loader(GLOBAL_PLUGINS, false);
    loader.createInstance<Base>("Panda")->saySomething();
    loader.unloadLibrary();

    loader.loadLibrary();
    loader.createInstance<Base>("Hyena")->saySomething();
    loader.unloadLibrary();
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  }

  dlclose(handle);
  // With all libraries closed, this should act like a normal load/unload.
  try {
    class_loader::ClassLoader loader(GLOBAL_PLUGINS, false);
    loader.createInstance<Base>("Alpaca")->saySomething();
    loader.unloadLibrary();
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  }
}

#endif  // ifndef _WIN32

// Leaves LIBRARY_2 loaded, so keep this suite last.
TEST(ClassLoaderLifetimeTest, instancesOutliveLoader) {
  auto loader = std::make_unique<class_loader::ClassLoader>(LIBRARY_2, false);
  std::shared_ptr<Base> shared = loader->createInstance<Base>("Robot");
  class_loader::ClassLoader::UniquePtr<Base> unique = loader->createUniqueInstance<Base>("Robot");
  loader.reset();
  EXPECT_EQ("Beep boop", shared->saySomething());
  EXPECT_EQ("Beep boop", unique->saySomething());
  // Must not use the destroyed loader (heap-use-after-free under AddressSanitizer).
  shared.reset();
  unique.reset();
}

// Run all the tests that were declared with TEST()
int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
