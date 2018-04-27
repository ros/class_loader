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

#include <chrono>
#include <cstddef>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "class_loader/class_loader.hpp"
#include "class_loader/multi_library_class_loader.hpp"

#include "gtest/gtest.h"

#include "./base.hpp"

const std::string LIBRARY_1 = class_loader::systemLibraryFormat("class_loader_TestPlugins1");  // NOLINT
const std::string LIBRARY_2 = class_loader::systemLibraryFormat("class_loader_TestPlugins2");  // NOLINT

TEST(ClassLoaderSharedPtrTest, basicLoad) {
  try {
    class_loader::ClassLoader loader1(LIBRARY_1, false);
    loader1.createSharedInstance<Base>("Cat")->saySomething();  // See if lazy load works
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  }

  SUCCEED();
}

TEST(ClassLoaderSharedPtrTest, correctNonLazyLoadUnload) {
  try {
    ASSERT_FALSE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
    class_loader::ClassLoader loader1(LIBRARY_1, false);
    ASSERT_TRUE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
    ASSERT_TRUE(loader1.isLibraryLoaded());
    loader1.unloadLibrary();
    ASSERT_FALSE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
    ASSERT_FALSE(loader1.isLibraryLoaded());
    return;
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  } catch (...) {
    FAIL() << "Unhandled exception";
  }
}

TEST(ClassLoaderSharedPtrTest, correctLazyLoadUnload) {
  try {
    ASSERT_FALSE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
    class_loader::ClassLoader loader1(LIBRARY_1, true);
    ASSERT_FALSE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
    ASSERT_FALSE(loader1.isLibraryLoaded());

    {
      std::shared_ptr<Base> obj = loader1.createSharedInstance<Base>("Cat");
      ASSERT_TRUE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
      ASSERT_TRUE(loader1.isLibraryLoaded());
    }

    // The library will unload automatically when the only plugin object left is destroyed
    ASSERT_FALSE(class_loader::impl::isLibraryLoadedByAnybody(LIBRARY_1));
    return;
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  } catch (...) {
    FAIL() << "Unhandled exception";
  }
}

TEST(ClassLoaderSharedPtrTest, nonExistentPlugin) {
  class_loader::ClassLoader loader1(LIBRARY_1, false);

  try {
    std::shared_ptr<Base> obj = loader1.createSharedInstance<Base>("Bear");
    if (nullptr == obj) {
      FAIL() << "Null object being returned instead of exception thrown.";
    }

    obj->saySomething();
  } catch (const class_loader::CreateClassException &) {
    SUCCEED();
    return;
  } catch (...) {
    FAIL() << "Unknown exception caught.\n";
  }

  FAIL() << "Did not throw exception as expected.\n";
}

TEST(ClassLoaderSharedPtrTest, nonExistentLibrary) {
  try {
    class_loader::ClassLoader loader1("libDoesNotExist.so", false);
  } catch (const class_loader::LibraryLoadException &) {
    SUCCEED();
    return;
  } catch (...) {
    FAIL() << "Unknown exception caught.\n";
  }

  FAIL() << "Did not throw exception as expected.\n";
}

class InvalidBase
{
};

TEST(ClassLoaderSharedPtrTest, invalidBase) {
  try {
    class_loader::ClassLoader loader1(LIBRARY_1, false);
    if (loader1.isClassAvailable<InvalidBase>("Cat")) {
      FAIL() << "Cat should not be available for InvalidBase";
    } else if (loader1.isClassAvailable<Base>("Cat")) {
      SUCCEED();
      return;
    } else {
      FAIL() << "Class not available for correct base class.";
    }
  } catch (const class_loader::LibraryLoadException &) {
    FAIL() << "Unexpected exception";
  } catch (...) {
    FAIL() << "Unexpected and unknown exception caught.\n";
  }
}

void wait(int seconds)
{
  std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void run(class_loader::ClassLoader * loader)
{
  std::vector<std::string> classes = loader->getAvailableClasses<Base>();
  for (auto & class_ : classes) {
    loader->createSharedInstance<Base>(class_)->saySomething();
  }
}

TEST(ClassLoaderSharedPtrTest, threadSafety) {
  class_loader::ClassLoader loader1(LIBRARY_1);
  ASSERT_TRUE(loader1.isLibraryLoaded());

  // Note: Hard to test thread safety to make sure memory isn't corrupted.
  // The hope is this test is hard enough that once in a while it'll segfault
  // or something if there's some implementation error.
  try {
    std::vector<std::thread *> client_threads;

    for (size_t c = 0; c < 1000; c++) {
      client_threads.push_back(new std::thread(std::bind(&run, &loader1)));
    }

    for (auto & client_thread : client_threads) {
      client_thread->join();
    }

    for (auto & client_thread : client_threads) {
      delete (client_thread);
    }

    loader1.unloadLibrary();
    ASSERT_FALSE(loader1.isLibraryLoaded());
  } catch (const class_loader::ClassLoaderException &) {
    FAIL() << "Unexpected ClassLoaderException.";
  } catch (...) {
    FAIL() << "Unknown exception.";
  }
}

TEST(ClassLoaderSharedPtrTest, loadRefCountingNonLazy) {
  try {
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

    return;
  } catch (const class_loader::ClassLoaderException &) {
    FAIL() << "Unexpected exception.\n";
  } catch (...) {
    FAIL() << "Unknown exception caught.\n";
  }

  FAIL() << "Did not throw exception as expected.\n";
}

TEST(ClassLoaderSharedPtrTest, loadRefCountingLazy) {
  try {
    class_loader::ClassLoader loader1(LIBRARY_1, true);
    ASSERT_FALSE(loader1.isLibraryLoaded());

    {
      std::shared_ptr<Base> obj = loader1.createSharedInstance<Base>("Dog");
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

    return;
  } catch (const class_loader::ClassLoaderException &) {
    FAIL() << "Unexpected exception.\n";
  } catch (...) {
    FAIL() << "Unknown exception caught.\n";
  }

  FAIL() << "Did not throw exception as expected.\n";
}

void testMultiClassLoader(bool lazy)
{
  try {
    class_loader::MultiLibraryClassLoader loader(lazy);
    loader.loadLibrary(LIBRARY_1);
    loader.loadLibrary(LIBRARY_2);
    for (int i = 0; i < 2; ++i) {
      loader.createSharedInstance<Base>("Cat")->saySomething();
      loader.createSharedInstance<Base>("Dog")->saySomething();
      loader.createSharedInstance<Base>("Robot")->saySomething();
    }
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  }

  SUCCEED();
}

TEST(MultiClassLoaderTest, lazyLoad) {
  testMultiClassLoader(true);
}
TEST(MultiClassLoaderTest, lazyLoadSecondTime) {
  testMultiClassLoader(true);
}
TEST(MultiClassLoaderTest, nonLazyLoad) {
  testMultiClassLoader(false);
}
TEST(MultiClassLoaderTest, noWarningOnLazyLoad) {
  try {
    std::shared_ptr<Base> cat, dog, rob;
    {
      class_loader::MultiLibraryClassLoader loader(true);
      loader.loadLibrary(LIBRARY_1);
      loader.loadLibrary(LIBRARY_2);

      cat = loader.createSharedInstance<Base>("Cat");
      dog = loader.createSharedInstance<Base>("Dog");
      rob = loader.createSharedInstance<Base>("Robot");
    }
    cat->saySomething();
    dog->saySomething();
    rob->saySomething();
  } catch (class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  }

  SUCCEED();
}

// Run all the tests that were declared with TEST()
int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
