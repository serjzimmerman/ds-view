#include "dsview/dslib/scpi/commands/common.hpp"

#include <fmt/format.h>
#include <gtest/gtest.h>

#include <range/v3/view/cartesian_product.hpp>

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
names_to_models() -> const std::unordered_map<std::string_view, ds::ds_model>&
{
    static const auto map = std::unordered_map<std::string_view, ds::ds_model>{
        { "MSO1104Z-S", ds::ds_model::e_mso1104z_s },
        { "MSO1074Z-S", ds::ds_model::e_mso1074z_s },
        { "MSO1104Z", ds::ds_model::e_mso1104z },
        { "MSO1074Z", ds::ds_model::e_mso1074z },
        { "DS1104Z-S Plus", ds::ds_model::e_ds1104z_s_plus },
        { "DS1074Z-S Plus", ds::ds_model::e_ds1074z_s_plus },
        { "DS1104Z Plus", ds::ds_model::e_ds1104z_plus },
        { "DS1074Z Plus", ds::ds_model::e_ds1074z_plus },
        { "DS1054Z", ds::ds_model::e_ds1054z } };

    return map;
}

TEST( dslib, idn_model ) // [NOLINT]
{
    for ( auto&& [ name, model ] : names_to_models() )
    {
        EXPECT_EQ( name, to_string( model ) );
        EXPECT_EQ( to_model( name ), model );
    }
}

constexpr auto versions = std::array{
    "00.04.05.SP2",
    "00.04.04.SP3",
    "00.04.04.SP1" //
};

constexpr auto serial_numbers = std::array{
    "DS1ZA170XXXXXX",
    "DS1ZA2XXXXXXXX" //
};

TEST( dslib, idn_response ) // [NOLINT]
{
    for ( auto&& [ model_pair, version, serial ] :
          ranges::views::cartesian_product( names_to_models(), versions, serial_numbers ) )
    {
        auto&& [ model_name, model ] = model_pair;

        auto response = fmt::format( "RIGOL TECHNOLOGIES,{},{},{}", model_name, serial, version );
        auto parsed = parser::parse( response );

        EXPECT_EQ( parsed.model, model );
        EXPECT_EQ( parsed.serial_number, serial );
        EXPECT_EQ( parsed.software_version, version );
    }
}

} // namespace