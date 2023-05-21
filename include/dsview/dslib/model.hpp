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

//! @namespace
namespace ds
{

//! @brief Types of Scope models
enum class ds_model
{
    e_mso1104z_s,     //!< MSO1104Z-S
    e_mso1074z_s,     //!< MSO1074Z-S
    e_mso1104z,       //!< MSO1104Z
    e_mso1074z,       //!< MSO1074Z
    e_ds1104z_s_plus, //!< DS1104Z-S Plus
    e_ds1074z_s_plus, //!< DS1074Z-S Plus
    e_ds1104z_plus,   //!< DS1104Z Plus
    e_ds1074z_plus,   //!< DS1074Z Plus
    e_ds1054z         //!< DS1054Z
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

constexpr inline auto model_name_arr = detail::create_sorted_model_arr();

} // namespace detail

//! @brief Parses model string representation.
//! @throws std::out_of_range If the name does not correspond to any model.
[[nodiscard]] constexpr auto
to_model( std::string_view model_name ) -> ds_model
{
    auto found = std::lower_bound(
        begin( detail::model_name_arr ),
        end( detail::model_name_arr ),
        model_name,
        []( auto&& lhs, std::string_view to_find ) { return lhs.first < to_find; } );

    if ( found == end( detail::model_name_arr ) || found->first != model_name )
    {
        throw std::out_of_range{ "Model name is unknown" };
    }

    return found->second;
};

//! @brief Analog bandwidth
enum class model_bandwidth
{
    e_50mhz,  //!< 50 MHz
    e_70mhz,  //!< 70 MHz
    e_100mhz, //!< 100 MHz
};

//! @brief Analog channel count
enum class model_analog_channels
{
    e_count_2, //!< 2 Channels
    e_count_4, //!< 4 Channels (for DS1000 series)
};

//! @brief Digital channel count
enum class model_digital_channels
{
    e_count_none, //!< No digital input
    e_count_16,   //!< 16 Digital channels (not supported by DS1054Z)
};

struct model_capabilities
{
    model_bandwidth analog_bandwidth;
    model_analog_channels analog_channels;
    model_digital_channels digital_channels;
};

namespace detail
{

consteval auto
create_sorted_capabilities_arr()
{
    auto unsorted = std::to_array<std::pair<ds_model, model_capabilities>>(
        { { ds_model::e_mso1104z_s,
            { model_bandwidth::e_100mhz, model_analog_channels::e_count_4, model_digital_channels::e_count_16 } },
          { ds_model::e_mso1074z_s,
            { model_bandwidth::e_70mhz, model_analog_channels::e_count_4, model_digital_channels::e_count_16 } },
          { ds_model::e_mso1104z,
            { model_bandwidth::e_100mhz, model_analog_channels::e_count_4, model_digital_channels::e_count_16 } },
          { ds_model::e_mso1074z,
            { model_bandwidth::e_70mhz, model_analog_channels::e_count_4, model_digital_channels::e_count_16 } },
          { ds_model::e_ds1104z_s_plus,
            { model_bandwidth::e_100mhz, model_analog_channels::e_count_4, model_digital_channels::e_count_16 } },
          { ds_model::e_ds1074z_s_plus,
            { model_bandwidth::e_70mhz, model_analog_channels::e_count_4, model_digital_channels::e_count_16 } },
          { ds_model::e_ds1104z_plus,
            { model_bandwidth::e_100mhz, model_analog_channels::e_count_4, model_digital_channels::e_count_16 } },
          { ds_model::e_ds1074z_plus,
            { model_bandwidth::e_70mhz, model_analog_channels::e_count_4, model_digital_channels::e_count_16 } },
          { ds_model::e_ds1054z,
            { model_bandwidth::e_50mhz, model_analog_channels::e_count_4, model_digital_channels::e_count_none } } } );

    std::sort( begin( unsorted ), end( unsorted ), [ less = std::less<>{} ]( auto&& lhs, auto&& rhs ) {
        return less( lhs.first, rhs.first );
    } );

    return unsorted;
}

} // namespace detail

[[nodiscard]] constexpr auto
get_model_capabilities( ds_model model ) -> model_capabilities
{
    constexpr auto capabilities_arr = detail::create_sorted_capabilities_arr();

    auto found =
        std::lower_bound( begin( capabilities_arr ), end( capabilities_arr ), model, []( auto&& lhs, ds_model mod ) {
            return lhs.first < mod;
        } );

    if ( found == end( capabilities_arr ) || found->first != model )
    {
        throw std::out_of_range{ "Model is not found" };
    }

    return found->second;
}

} // namespace ds