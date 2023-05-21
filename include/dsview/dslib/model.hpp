/*
 * ----------------------------------------------------------------------------
 *  "THE BEER-WARE LICENSE" (Revision 42):
 *  <tsimmerman.ss@phystech.edu> wrote this file. As long as you retain this notice you
 *  can do whatever you want with this stuff. If we meet some day, and you think
 *  this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <stdexcept>
#include <string_view>

/**
 * @brief Definitions of Rigol DS Scope model names and utilities for parsing model names.
 * @file model.hpp
 */

//! @namespace ds
namespace ds
{

//! Types of Scope models
enum class ds_model
{
    e_mso1104z_s,     //< MSO1104Z-S
    e_mso1074z_s,     //< MSO1074Z-S
    e_mso1104z,       //< MSO1104Z
    e_mso1074z,       //< MSO1074Z
    e_ds1104z_s_plus, //< DS1104Z-S Plus
    e_ds1074z_s_plus, //< DS1074Z-S Plus
    e_ds1104z_plus,   //< DS1104Z Plus
    e_ds1074z_plus,   //< DS1074Z Plus
    e_ds1054z         //< DS1054Z
};

//! Convert model to a readable name. These names are taken from the technical documentaion.
[[nodiscard]] constexpr auto
to_string( ds_model model ) -> std::string_view
{
    switch ( model )
    {
    case ds_model::e_mso1104z_s:
        return "MSO1104Z-S";
    case ds_model::e_mso1074z_s:
        return "MSO1074Z-S";
    case ds_model::e_mso1104z:
        return "MSO1104Z";
    case ds_model::e_mso1074z:
        return "MSO1074Z";
    case ds_model::e_ds1104z_s_plus:
        return "DS1104Z-S Plus";
    case ds_model::e_ds1074z_s_plus:
        return "DS1074Z-S Plus";
    case ds_model::e_ds1104z_plus:
        return "DS1104Z Plus";
    case ds_model::e_ds1074z_plus:
        return "DS1074Z Plus";
    case ds_model::e_ds1054z:
        return "DS1054Z";
    }
};

namespace detail
{

consteval auto
create_sorted_model_arr()
{
    auto unsorted = std::to_array<std::pair<std::string_view, ds_model>>(
        { { "MSO1104Z-S", ds_model::e_mso1104z_s },
          { "MSO1074Z-S", ds_model::e_mso1074z_s },
          { "MSO1104Z", ds_model::e_mso1104z },
          { "MSO1074Z", ds_model::e_mso1074z },
          { "DS1104Z-S Plus", ds_model::e_ds1104z_s_plus },
          { "DS1074Z-S Plus", ds_model::e_ds1074z_s_plus },
          { "DS1104Z Plus", ds_model::e_ds1104z_plus },
          { "DS1074Z Plus", ds_model::e_ds1074z_plus },
          { "DS1054Z", ds_model::e_ds1054z } } );

    std::sort( begin( unsorted ), end( unsorted ), [ less = std::less<>{} ]( auto&& lhs, auto&& rhs ) {
        return less( lhs.first, rhs.first );
    } );

    return unsorted;
}

static constexpr auto model_name_arr = create_sorted_model_arr();

} // namespace detail

//! @brief Parses model string representation.
//! @throws std::out_of_range If the name does not correspond to any model.
[[nodiscard]] constexpr auto
to_model( std::string_view model_name ) -> ds_model
{
    const auto start = begin( detail::model_name_arr );
    const auto finish = end( detail::model_name_arr );

    auto found = std::lower_bound( start, finish, model_name, []( auto&& lhs, std::string_view to_find ) {
        return lhs.first < to_find;
    } );

    if ( found == finish || found->first != model_name )
    {
        throw std::out_of_range{ "Model name is unknown" };
    }

    return found->second;
};

} // namespace ds