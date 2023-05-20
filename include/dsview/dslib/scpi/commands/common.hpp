/*
 * ----------------------------------------------------------------------------
 *  "THE BEER-WARE LICENSE" (Revision 42):
 *  <tsimmerman.ss@phystech.edu> wrote this file. As long as you retain this notice you
 *  can do whatever you want with this stuff. If we meet some day, and you think
 *  this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

#pragma once

#include <boost/fusion/adapted.hpp>
#include <boost/spirit/home/x3.hpp>

#include "dsview/dslib/model.hpp"
#include "dsview/dslib/scpi/command.hpp"

#include <algorithm>

namespace ds::scpi::common
{

struct identify_result
{
    ds_model model;
    std::string serial_number;
    std::string software_version;
};

} // namespace ds::scpi::common

BOOST_FUSION_ADAPT_STRUCT( ds::scpi::common::identify_result, model, serial_number, software_version ); // [NOLINT]

namespace ds::scpi::common
{

namespace x3 = boost::spirit::x3;

namespace parser
{

struct idn_query_parser
{
    struct model_table : x3::symbols<ds_model>
    {
        model_table()
        {
            std::for_each( begin( detail::model_name_arr ), end( detail::model_name_arr ), [ this ]( auto pair ) {
                add( pair.first, pair.second );
            } );
        }
    };

    [[nodiscard]] static auto parse( std::string_view str ) -> identify_result
    {
        static const model_table model_parser;
        static const auto parser = x3::expect
            [ x3::lit( "RIGOL TECHNOLOGIES" ) >> ',' >> model_parser >> ',' >> +( x3::char_ - ',' ) >> ',' >>
              +( x3::char_ - '\n' ) >> '\n' ];

        auto result = identify_result{};
        x3::parse( begin( str ), end( str ), parser, result );
        return result;
    }
};

struct opc_query_parser
{
    [[nodiscard]] static auto parse( std::string_view str ) -> bool
    {
        static const auto parser = x3::expect[ x3::int_ >> '\n' ];
        auto result = int{};
        x3::parse( begin( str ), end( str ), parser, result );

        if ( result != 0 && result != 1 )
        {
            throw std::runtime_error{ "Invalid response from OPC command" };
        }

        return result;
    }
};

}; // namespace parser

using idn_cmd = basic_command<root_category, fixstr::fixed_string{ "*IDN" }, parser::idn_query_parser, void>;
using rst_cmd = basic_command<root_category, fixstr::fixed_string{ "*RST" }, void, std::tuple<>>;
using opc_cmd = basic_command<root_category, fixstr::fixed_string{ "*OPC" }, parser::opc_query_parser, std::tuple<>>;
using cls_cmd = basic_command<root_category, fixstr::fixed_string{ "*CLS" }, void, std::tuple<>>;

} // namespace ds::scpi::common