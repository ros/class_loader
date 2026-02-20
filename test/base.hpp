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

#ifndef BASE_HPP_
#define BASE_HPP_

#include <memory>
#include <string>

#include "class_loader/interface_traits.hpp"

// This was originally at 1000, but arm32 platforms we have tested on are not able to
// successfully spin up 1000 threads in this test process. Using 500 as a reliably passing number.
static constexpr size_t STRESS_TEST_NUM_THREADS = 500;

class Base
{
public:
  virtual ~Base() {}
  virtual void saySomething() = 0;
};

class BaseWithInterfaceCtor
{
public:
  // constructor parameters for the base class do not need to match the derived classes
  explicit BaseWithInterfaceCtor(std::string) {}
  virtual ~BaseWithInterfaceCtor() = default;

  virtual int get_number() = 0;
};

template<>
struct class_loader::InterfaceTraits<BaseWithInterfaceCtor>
{
  // - string to check that arguments are not mixed up with class names and
  //   that they do not create ambiguous calls
  // - unique_ptr to check that the construction works correctly with move only types
  using constructor_parameters = class_loader::ConstructorParameters<std::string,
      std::unique_ptr<int>>;
};

static_assert(
  class_loader::is_interface_constructible_v<BaseWithInterfaceCtor, std::string,
  std::unique_ptr<int>>,
  "BaseWithInterfaceCtor should be interface constructible with the specifed types."
);

static_assert(
  class_loader::is_interface_constructible_v<BaseWithInterfaceCtor, const std::string &,
  std::unique_ptr<int>&&>,
  "BaseWithInterfaceCtor should be interface constructible with the specifed types."
);


#endif  // BASE_HPP_
