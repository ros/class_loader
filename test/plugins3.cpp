/*
 * Software License Agreement (BSD License)
 *
 * Copyright (c) 2026, Multi-robot Systems (MRS) group at Czech Technical University in Prague
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

#include "class_loader/register_macro.hpp"

#include "./base.hpp"

class Identity : public BaseWithInterfaceCtor
{
public:
  explicit Identity(std::string name, std::unique_ptr<int> number)
  : BaseWithInterfaceCtor(name), number_(*number) {}

  int get_number() override
  {
    return number_;
  }

private:
  int number_;
};

class Double : public BaseWithInterfaceCtor
{
public:
  explicit Double(std::string name, std::unique_ptr<int> number)
  : BaseWithInterfaceCtor(name), number_(*number) {}

  int get_number() override
  {
    return 2 * number_;
  }

private:
  int number_;
};

CLASS_LOADER_REGISTER_CLASS(Identity, BaseWithInterfaceCtor)
CLASS_LOADER_REGISTER_CLASS(Double, BaseWithInterfaceCtor)
