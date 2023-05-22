/*
 * ----------------------------------------------------------------------------
 *  "THE BEER-WARE LICENSE" (Revision 42):
 *  <tsimmerman.ss@phystech.edu> wrote this file. As long as you retain this notice you
 *  can do whatever you want with this stuff. If we meet some day, and you think
 *  this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

/**
 * @file global.hpp
 * @brief Global category commands for DS1000 series. Prefixed with ':'
 */

#pragma once

#include "dsview/dslib/scpi/command.hpp"

#include <algorithm>
#include <cstdint>

namespace ds::scpi::ds1000
{

/**
 * @brief Enable the waveform auto setting function. The oscilloscope will automatically adjust the
 * vertical scale, horizontal timebase, and trigger mode according to the input signal to
 * realize optimum waveform display. This command is equivalent to pressing the AUTO key
 * at the front panel.
 * @note Syntax: :AUToscale
 *
 * Theoretically, waveform auto setting function requires that the frequency of sine is no
 * lower than 41Hz; the duty cycle should be greater than 1% and the amplitude must
 * be at least 20mVpp for square (the probe ratio is 1X).
 *
 * When the pass/fail function is enabled (see the :MASK:ENABle command), if you sent
 * this command, the oscilloscope will disable the pass/fail function firstly and then
 * execute the waveform auto setting function.
 *
 * When the waveform record function is enabled or during the playback of the
 * recorded waveform, this command is invalid.
 *
 */
using auto_cmd = basic_command<global_category, fixstr::fixed_string{ "AUT" }, void>;

/**
 * @brief Clear all the waveforms on the screen. If the oscilloscope is in the RUN state, waveform
 * will still be displayed. This command is equivalent to pressing the CLEAR key at the front
 * panel.
 * @note Syntax: :CLEar
 */
using clear_cmd = basic_command<global_category, fixstr::fixed_string{ "CLE" }, void>;

/**
 * @brief The :RUN command starts the oscilloscope and the :STOP command stops the
 * oscilloscope. These commands are equivalent to pressing the RUN/STOP key at the
 * front panel.
 *
 * When the waveform record function is enabled or during the playback of the recorded
 * waveform, these commands are invalid.
 *
 * @note Syntax: :RUN
 */
using run_cmd = basic_command<global_category, fixstr::fixed_string{ "RUN" }, void>;

/**
 * @brief The :RUN command starts the oscilloscope and the :STOP command stops the
 * oscilloscope. These commands are equivalent to pressing the RUN/STOP key at the
 * front panel.
 *
 * When the waveform record function is enabled or during the playback of the recorded
 * waveform, these commands are invalid.
 *
 * @note Syntax: :STOP
 */
using stop_cmd = basic_command<global_category, fixstr::fixed_string{ "STOP" }, void>;

/**
 * @brief Set the oscilloscope to the single trigger mode. This command is equivalent to any of the
 * following two operations: pressing the SINGLE key at the front panel and sending
 * the :TRIGger:SWEep SINGle command.
 *
 * In the single trigger mode, the oscilloscope triggers once when the trigger conditions
 * are met and then stops.
 *
 * When the waveform record function is enabled or during the playback of the
 * recorded waveform, this command is invalid.
 *
 * @note Syntax: :SINGle
 * @see tforce_cmd run_cmd stop_cmd
 */
using single_cmd = basic_command<global_category, fixstr::fixed_string{ "SINGL" }, void>;

/**
 * @brief Generate a trigger signal forcefully. This command is only applicable to the normal and
 * single trigger modes (see the :TRIGger:SWEep command) and is equivalent to pressing
 * the FORCE key in the trigger control area at the front panel.
 *
 * @note Syntax: :TFORce
 * @see tforce_cmd run_cmd stop_cmd
 */
using tforce_cmd = basic_command<global_category, fixstr::fixed_string{ "TFOR" }, void>;

} // namespace ds::scpi::ds1000