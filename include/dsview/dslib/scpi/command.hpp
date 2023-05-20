/*
 * ----------------------------------------------------------------------------
 *  "THE BEER-WARE LICENSE" (Revision 42):
 *  <tsimmerman.ss@phystech.edu> wrote this file. As long as you retain this notice you
 *  can do whatever you want with this stuff. If we meet some day, and you think
 *  this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

#pragma once

#include "dsview/dslib/detail/common.hpp"

#include <fixed_string.hpp>
#include <fmt/format.h>

#include <concepts>
#include <tuple>
#include <utility>

namespace ds::scpi
{

struct root_category
{
    static constexpr auto path = fixstr::fixed_string{ "" };
    [[nodiscard]] static constexpr auto concat( auto name ) { return name; }
};

template <typename T> // clang-format off
concept scpi_category = requires () {
    T::path;
    { T::concat( fixstr::fixed_string{ "" } ) } -> std::convertible_to<std::string_view>;
}; // clang-format on

template <scpi_category parent_category, auto child_category> struct basic_category
{
    static constexpr auto path = parent_category::template concat( child_category );
    [[nodiscard]] static constexpr auto concat( auto name ) { return path + ":" + name; }
};

using global_category = basic_category<root_category, fixstr::fixed_string{ "" }>;

static_assert( scpi_category<root_category> );
static_assert( scpi_category<global_category> );

template <scpi_category category_t, auto command_name_p, typename query_parser_t, typename command_args_tuple>
class basic_command
{
  public:
    using category = category_t;
    using query_parser = query_parser_t;

    static constexpr auto category_path = category::path;
    static constexpr auto command_name = fixstr::fixed_string{ command_name_p };
    static constexpr auto command_base = category::template concat( command_name );

    static constexpr bool has_query = !std::is_void_v<query_parser_t>;
    static constexpr bool has_operation = !std::is_void_v<command_args_tuple>;

  public:
    [[nodiscard]] static constexpr auto get_query_string()
        requires ( has_query )
    {
        return command_base + "?";
    }

  private:
    template <auto I0, auto... I>
    [[nodiscard]] static constexpr auto get_command_format_string( std::index_sequence<I0, I...> )
    {
        return command_base + " " + ( "{}" + ... + fixstr::fixed_string{ ( util::ignore( I ), ",{}" ) } );
    }

    [[nodiscard]] static constexpr auto get_command_format_string( std::index_sequence<> ) { return command_base; }

  public:
    template <typename... Ts>
        requires ( has_operation && std::same_as<std::tuple<Ts...>, command_args_tuple> )
    [[nodiscard]] static auto get_command_string( Ts&&... args )
    {
        static constexpr auto format_string =
            get_command_format_string( std::make_index_sequence<sizeof...( args )>{} );
        return fmt::vformat( std::string_view{ format_string }, fmt::make_format_args( args... ) );
    }
};

} // namespace ds::scpi