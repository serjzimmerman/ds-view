#include "dsview/dslib/scpi/commands/common.hpp"

#include <fmt/format.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <random>
#include <unordered_map>
#include <utility>

namespace
{

using command = ds::scpi::common::idn_cmd;
using parser = command::query_parser;

using ds::to_model;

auto
names_to_models() -> std::unordered_map<std::string_view, ds::ds_model>
{
    return {
        { "MSO1104Z-S", ds::ds_model::e_mso1104z_s },
        { "MSO1074Z-S", ds::ds_model::e_mso1074z_s },
        { "MSO1104Z", ds::ds_model::e_mso1104z },
        { "MSO1074Z", ds::ds_model::e_mso1074z },
        { "DS1104Z-S Plus", ds::ds_model::e_ds1104z_s_plus },
        { "DS1074Z-S Plus", ds::ds_model::e_ds1074z_s_plus },
        { "DS1104Z Plus", ds::ds_model::e_ds1104z_plus },
        { "DS1074Z Plus", ds::ds_model::e_ds1074z_plus },
        { "DS1054Z", ds::ds_model::e_ds1054z } };
}

TEST( dslib, idn_model ) // [NOLINT]
{
    for ( auto&& [ name, model ] : names_to_models() )
    {
        EXPECT_EQ( name, to_string( model ) );
        EXPECT_EQ( to_model( name ), model );
    }
}

TEST( dslib, idn_response ) // [NOLINT]
{
    const auto versions = std::to_array<std::string_view>( { "00.04.05.SP2", "00.04.04.SP3", "00.04.04.SP1" } );
    const auto serial_numbers = std::to_array<std::string_view>( { "DS1ZA170XXXXXX", "DS1ZA2XXXXXXXX" } );

    for ( const auto& [ model_name, model ] : names_to_models() )
    {
        for ( const auto& version : versions )
        {
            for ( const auto& serial : serial_numbers )
            {
                auto response = fmt::format( "RIGOL TECHNOLOGIES,{},{},{}\n", model_name, serial, version );
                auto parsed = parser::parse( response );

                EXPECT_EQ( parsed.model, model );
                EXPECT_EQ( parsed.serial_number, serial );
                EXPECT_EQ( parsed.software_version, version );
            }
        }
    }
}

} // namespace