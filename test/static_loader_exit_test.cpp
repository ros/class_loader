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

#include <memory>
#include <string>

#include "class_loader/class_loader.hpp"
#include "class_loader/multi_library_class_loader.hpp"

#include "base.hpp"
#include "gtest/gtest.h"

// Regression test for ros/class_loader#33: loaders with static storage duration (as in a
// static pluginlib::ClassLoader) are constructed before main(), so the globals class_loader
// first touches in loadLibrary() are created after them and destroyed before them. Unloading a
// library when the loaders are destroyed at exit must not use any destroyed global.
// Keep this a separate executable so nothing else touches class_loader before these loaders do.

namespace
{
const std::string & library()
{
  static const std::string name = class_loader::systemLibraryFormat("class_loader_TestPlugins1");
  return name;
}

class_loader::MultiLibraryClassLoader g_multi_loader(false);
// Lazy: nothing is loaded until the first createSharedInstance().
class_loader::ClassLoader g_loader(library(), true);
}  // namespace

TEST(StaticLoaderExitTest, staticLoadersUnloadAtStaticDestruction) {
  g_multi_loader.loadLibrary(library());
  EXPECT_TRUE(g_multi_loader.isLibraryAvailable(library()));
  {
    auto multi_instance = g_multi_loader.createInstance<Base>("Dog");
    ASSERT_NE(nullptr, multi_instance);
    EXPECT_EQ("Bark", multi_instance->saySomething());
  }

  auto instance = g_loader.createInstance<Base>("Cat");
  ASSERT_NE(nullptr, instance);
  EXPECT_EQ("Meow", instance->saySomething());
}
