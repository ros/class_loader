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

#ifndef CLASS_LOADER__INTERFACE_TRAITS_HPP_
#define CLASS_LOADER__INTERFACE_TRAITS_HPP_

#include <type_traits>

namespace class_loader
{

namespace impl
{

template<class ...>
constexpr bool false_v = false;

}  // namespace impl

template<class ... Params>
struct ConstructorParameters {};

/**
 * @brief Customization point that allows setting additional properties of
 * an interface class.
 *
 * @tparam T Base class for which the traits are defined
 *
 * Users can crete specializations of this class for the base type for which
 * the properties should be set. Properties are optional and therefore might not
 * exist in a given specialization. To access these properties, use their
 * respective accessor traits which provide the default value when it is not
 * specified.
 *
 * Supported properties:
 * - `constructor_parameters` (member type)
 *   - accessor: interface_constructor_parameters
 *   - default: ConstructorParameters<>
 *   - defines the parameters with which will the class loader instantiate the
 *     derived classes
 *   - example:
 *     \code
 *     using constructor_parameters = ConstructorParameters<double, int>;
 *     \endcode
 *     defines that derived classes of class T (replace with the base class type
 *     used in this specialization) will be instantiated with `double` and `int`
 *     as their constructor arguments
 *
 * Example:
 * \code
 * class BaseWithInterfaceCtor
 * {
 * public:
 *   // constructor parameters for the base class do not need to match the derived classes
 *   explicit BaseWithInterfaceCtor(std::string) {}
 *   virtual ~BaseWithInterfaceCtor() = default;
 *
 *   virtual int get_number() = 0;
 * };
 *
 * // Specialize the class_loader::InterfaceTraits struct to define properties of your interface
 * template<>
 * struct class_loader::InterfaceTraits<BaseWithInterfaceCtor>
 * {
 *   // derived classes will be instantiated with two parameters (string and unique_ptr<int>)
 *   using constructor_parameters = ConstructorParameters<std::string, std::unique_ptr<int>>;
 * };
 * \endcode
 */
template<class T>
struct InterfaceTraits
{
};

namespace impl
{

template<class T, class = void>
struct interface_constructor_parameters_impl
{
  using type = ConstructorParameters<>;
};

template<class T>
struct interface_constructor_parameters_impl<T,
  std::void_t<typename InterfaceTraits<T>::constructor_parameters>>
{
  using type = typename InterfaceTraits<T>::constructor_parameters;
};

}  // namespace impl

/**
 * @brief Type trait for extracting the `constructor_parameters` of
 * InterfaceTraits.
 *
 * @tparam T same as in InterfaceTraits
 *
 * Helper type:\n
 * @ref interface_constructor_parameters_t<T> = interface_constructor_parameters<T>::type;
 */
template<class T>
struct interface_constructor_parameters
{
  /**
   * @brief InterfaceTraits<T>::constructor_parameters if present, otherwise the
   * default value (see InterfaceTraits).
   */
  using type = typename impl::interface_constructor_parameters_impl<T>::type;
};

/**
 * @brief Helper type alias @ref interface_constructor_parameters<T>::type
 * @see interface_constructor_parameters
 */
template<class T>
using interface_constructor_parameters_t =
  typename interface_constructor_parameters<T>::type;

namespace impl
{

template<class ... Ts>
struct is_interface_constructible_impl
{
  static_assert(false_v<Ts...>, "Base template selected.");
};

template<class ... Params, class ... Args>
struct is_interface_constructible_impl<ConstructorParameters<Params...>, Args...>
  : std::is_invocable<void(Params...), Args...>
{
};

}  // namespace impl

/**
 * @brief Type trait for checking whether plugins derived from T can be
 * constructed with specified arguments.
 *
 * @tparam Base same as in InterfaceTraits
 * @tparam Args list of arguments to check
 *
 * Contains static constexpr member variable `value` that is true if plugins
 * derived from `T` can be constructed using `Args`, false otherwise.
 *
 * Helper variable template:\n
 * @ref is_interface_constructible_v<T, Args...> =
 * @ref is_interface_constructible "is_interface_constructible<T, Args...>::value"
 */
template<class Base, class ... Args>
struct is_interface_constructible
  : impl::is_interface_constructible_impl<interface_constructor_parameters_t<Base>, Args...>
{
};

/**
 * @brief Helper variable template for @ref is_interface_constructible "is_interface_constructible<T, Args...>::value"
 * @see is_interface_constructible
 */
template<class Base, class ... Args>
constexpr bool is_interface_constructible_v =
  is_interface_constructible<Base, Args...>::value;

}  // namespace class_loader

#endif  // CLASS_LOADER__INTERFACE_TRAITS_HPP_
