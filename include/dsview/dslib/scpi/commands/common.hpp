/*
 * ----------------------------------------------------------------------------
 *  "THE BEER-WARE LICENSE" (Revision 42):
 *  <tsimmerman.ss@phystech.edu> wrote this file. As long as you retain this notice you
 *  can do whatever you want with this stuff. If we meet some day, and you think
 *  this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

/**
 * @file common.hpp
 * @brief Common SPCI commands defined by IEEE488.2
 *
 * This is in a no way a standard comforming implementation. List of such commands for Rigol DS series Oscilloscopes
 * consists of the following commands:
 *
 * @li @ref ds::scpi::common::cls_cmd.
 * @li @ref ds::scpi::common::ese_cmd.
 * @li @ref ds::scpi::common::esr_cmd.
 * @li @ref ds::scpi::common::idn_cmd.
 * @li @ref ds::scpi::common::opc_cmd.
 * @li @ref ds::scpi::common::rst_cmd.
 * @li @ref ds::scpi::common::sre_cmd.
 * @li @ref ds::scpi::common::stb_cmd.
 * @li @ref ds::scpi::common::tst_cmd.
 * @li @ref ds::scpi::common::wai_cmd.
 *
 */

#pragma once

#include <boost/fusion/adapted.hpp>
#include <boost/spirit/home/x3.hpp>

#include "dsview/dslib/model.hpp"
#include "dsview/dslib/scpi/command.hpp"

#include <algorithm>
#include <cstdint>

namespace ds::scpi::common
{

//! @brief Struct that is returned by @ref ds::scpi::common::idn_cmd command.
struct identify_result
{
    ds_model model;               //< Scope model
    std::string serial_number;    //< Serial number
    std::string software_version; //< Firmware version
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
              +( x3::char_ ) ];

        auto result = identify_result{};
        x3::parse( begin( str ), end( str ), parser, result );
        return result;
    }
};

struct opc_query_parser
{
    [[nodiscard]] static auto parse( std::string_view str ) -> bool
    {
        static const auto parser = x3::expect[ x3::int_ ];
        auto result = int{};
        x3::parse( begin( str ), end( str ), parser, result );

        if ( result != 0 && result != 1 )
        {
            throw std::runtime_error{ "Invalid response from OPC command" };
        }

        return result;
    }
};

template <typename parser_t> struct simple_parser
{
    [[nodiscard]] static auto parse( std::string_view str ) -> typename parser_t::attribute_type
    {
        static const auto parser = x3::expect[ parser_t{} ];
        auto result = typename parser_t::attribute_type{};
        x3::parse( begin( str ), end( str ), parser, result );
        return result;
    }
};

using uint_parser = simple_parser<x3::uint_type>;

}; // namespace parser

/**
 * @brief Clear all the event registers and clear the error queue.
 * @note Syntax: *CLS
 */
using cls_cmd = basic_command<root_category, fixstr::fixed_string{ "*CLS" }, void>;

/**
 * @brief Set or query the enable register for the standard event status register set.
 * @note Syntax: *ESE <Integer> *ESE?
 *
 * The bit 1 and bit 6 of the standard event status register are not used and are always
 * treated as 0; therefore, the range of <value> are the decimal numbers corresponding to
 * the binary numbers X0XXXX0X (X is 1 or 0).
 *
 * @note Return format: The query returns an integer which equals the sum of the weights of all the bits that have
 * already been set in the register.
 */
using ese_cmd = basic_command<root_category, fixstr::fixed_string{ "*ESE" }, parser::uint_parser, std::tuple<uint8_t>>;

/**
 * @brief Query and clear the event register for the standard event status register.
 * @note Syntax: *ESR?
 *
 * The bit 1 and bit 6 of the standard event status register are not used and are always
 * treated as 0. The range of the return value are the decimal numbers corresponding to
 * the binary numbers X0XXXX0X (X is 1 or 0).
 *
 * @note Return format: The query returns an integer which equals the sum of the weights of all the bits in the
 * register.
 */
using esr_cmd = basic_command<root_category, fixstr::fixed_string{ "*ESR" }, parser::uint_parser, void>;

/**
 * @brief Query the ID string of the instrument.
 * @note Syntax: *IDN?
 *
 * @note Return format: The query returns 'RIGOL TECHNOLOGIES,<model>,<serial number>,<software version>', where:
 * @li <model>: the model number of the instrument.
 * @li <serial number>: the serial number of the instrument.
 * @li <software version>: the software version of the instrument.
 */
using idn_cmd = basic_command<root_category, fixstr::fixed_string{ "*IDN" }, parser::idn_query_parser, void>;

/**
 * @brief The *OPC command is used to set the Operation Complete bit (bit 0) in the standard
 * event status register to 1 after the current operation is finished. The *OPC? command is
 * used to query whether the current operation is finished.
 * @remark It seems like query operation *OPC? is blocking, and response is not returned until all operations have
 * finished.
 *
 * @note Syntax: *OPC *OPC?
 * @note Return format: The query returns 1 if the current operation is finished; otherwise, returns 0.
 */
using opc_cmd = basic_command<root_category, fixstr::fixed_string{ "*OPC" }, parser::opc_query_parser>;

/**
 * @brief Restore the instrument to the default state.
 * @note Syntax: *RST
 */
using rst_cmd = basic_command<root_category, fixstr::fixed_string{ "*RST" }, void>;

/**
 * @brief Set or query the enable register for the status byte register set.
 * @note Syntax: *SRE <Integer> *SRE?
 *
 * The bit 0 and bit 1 of the status byte register are not used and are always treated as 0;
 * therefore, the range of <value> are the decimal numbers corresponding to the binary
 * numbers XXXXXX00 (X is 1 or 0).
 *
 * @note Return format: The query returns an integer which equals the sum of the weights of all the bits that
 * have already been set in the register.
 */
using sre_cmd = basic_command<root_category, fixstr::fixed_string{ "*SRE" }, parser::uint_parser, std::tuple<uint8_t>>;

/**
 * @brief Query the event register for the status byte register. The value of the status byte register
 * is set to 0 after this command is executed.
 *
 * @note Syntax: *STB?
 *
 * The bit 0 and bit 1 of the status byte register are not used and are always treated as 0.
 * The query returns the decimal numbers corresponding to the binary numbers X0XXXX0X
 * (X is 1 or 0).
 *
 * @note Return format: The query returns an integer which equals the sum of the weights of all the bits in the
 * register.
 */
using stb_cmd = basic_command<root_category, fixstr::fixed_string{ "*STB" }, parser::uint_parser>;

/**
 * @brief Perform a self-test and then return the seilf-test results.
 * @note Syntax: *TST?
 * @note Return format: The query returns a decimal integer.
 */
using tst_cmd = basic_command<root_category, fixstr::fixed_string{ "*STB" }, parser::uint_parser>;

/**
 * @brief Wait for the operation to finish.
 * @note Syntax: *WAI
 *
 * The subsequent command can only be carried out after the current command has been
 * executed.
 */
using wai_cmd = basic_command<root_category, fixstr::fixed_string{ "*WAI" }, void>;

} // namespace ds::scpi::common