/*
 * ----------------------------------------------------------------------------
 *  "THE BEER-WARE LICENSE" (Revision 42):
 *  <tsimmerman.ss@phystech.edu> wrote this file. As long as you retain this notice you
 *  can do whatever you want with this stuff. If we meet some day, and you think
 *  this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

/**
 * @file display.hpp
 * @brief Display category commands for DS1000 series. Prefixed with ':'
 */

#pragma once

#include "dsview/dslib/scpi/command.hpp"

#include <algorithm>
#include <cstdint>

namespace ds::scpi::ds1000
{

using display_category = basic_category<global_category, fixstr::fixed_string{ "DISP" }>;

//! @brief Display command category
namespace display
{

/**
 * @brief Clear all the waveforms on the screen.
 * @note Syntax: :DISPlay:CLEar
 *
 * If the oscilloscope is in the RUN state, waveform will still be displayed. This command is equivalent to pressing
 * the CLEAR key at the front panel. Sending the :CLEar command can also clear all the waveforms on the screen.
 * @see ds::scpi::ds1000::run_cmd ds::scpi::ds1000::clear_cmd
 */
using clear_cmd = basic_command<display_category, fixstr::fixed_string{ "CLE" }, void>;

/**
 * @brief Read the data stream of the image currently displayed on the screen and set the color,
 * invert display, and format of the image acquired.
 * @note Syntax: :DISPlay:DATA? [<color>,<invert>,<format>]
 *
 * <color>: color of the image; ON denotes color and OFF denotes intensity graded color.
 * <invert>: the invert function; 1|ON denotes turning on the invert function and 0|OFF denotes turning off the invert
 * function.
 *
 * When [<color>,<invert>,<format>] is omitted, by default, the image color (:STORage:IMAGe:COLor) and the status of the
 * invert function (:STORage:IMAGe:INVERT) currently selected are used and the image format is set to BMP24.
 *
 * The command is sent from the PC to the instrument through the VISA interface. The
 * instrument responds to the command and directly returns the data stream of the
 * image currently displayed to the buffer area of the PC.
 */

using data_cmd = basic_command<display_category, fixstr::fixed_string{ "DATA" }, passthrough_parser<>, void>;

} // namespace display

} // namespace ds::scpi::ds1000