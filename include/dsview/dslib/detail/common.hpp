/*
 * ----------------------------------------------------------------------------
 *  "THE BEER-WARE LICENSE" (Revision 42):
 *  <tsimmerman.ss@phystech.edu> wrote this file. As long as you retain this notice you
 *  can do whatever you want with this stuff. If we meet some day, and you think
 *  this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

#pragma once

#include <boost/asio.hpp>

namespace ds
{

// clang-format off
struct as_string_t {};
struct as_vector_t {};
// clang-format on

static constexpr auto as_string = as_string_t{};
static constexpr auto as_vector = as_vector_t{};

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

namespace util
{

template <typename T>
void
ignore( T&& )
{
}

} // namespace util

} // namespace ds