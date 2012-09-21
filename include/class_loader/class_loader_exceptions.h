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

#include <exception>

namespace class_loader
{

/**
 * @class class_loaderException
 * @brief A base class for all class_loader exceptions that inherits from std::runtime_exception
 */
class PluginException: public std::runtime_error
{
  public:
    PluginException(const std::string error_desc) : std::runtime_error(error_desc) {}
};

/**
 * @class LibraryLoadException
 * @brief An exception class thrown when class_loader is unable to load the library associated with a given plugin
 */
class LibraryLoadException: public PluginException
{
  public:
    LibraryLoadException(const std::string error_desc) : PluginException(error_desc) {}
};

/**
 * @class LibraryUnloadException
 * @brief An exception class thrown when class_loader is unable to unload the library associated with a given plugin
 */
class LibraryUnloadException: public PluginException
{
  public:
    LibraryUnloadException(const std::string error_desc) : PluginException(error_desc) {}
};

/**
 * @class CreateClassException
 * @brief An exception class thrown when class_loader is unable to create the class associated with a given plugin
 */
class CreateClassException: public PluginException
{
  public:
    CreateClassException(const std::string error_desc) : PluginException(error_desc) {}
};

}
