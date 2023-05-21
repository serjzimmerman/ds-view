/*
 * ----------------------------------------------------------------------------
 *  "THE BEER-WARE LICENSE" (Revision 42):
 *  <tsimmerman.ss@phystech.edu> wrote this file. As long as you retain this notice you
 *  can do whatever you want with this stuff. If we meet some day, and you think
 *  this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

/**
 * @brief Interface for working with Rigol DS series scopes. Currently implements a concrete class for communication via
 * TCP.
 *
 * @file device.hpp
 */

#pragma once

#include "detail/common.hpp"
#include "scpi/commands/common.hpp"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <fmt/format.h>

#include <chrono>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <optional>
#include <span>
#include <sstream>
#include <vector>

/**
 * @namespace ds
 * @brief Contains everything related to dslib.
 */
namespace ds
{

//! @brief Empty flag type used for blocking overloads
struct block_query_t
{
};

//! @brief Object of type block_query_t for passing into functions for blocking overloads.
static constexpr auto block_query = block_query_t{};

/**
 * @brief Interface for any DS series Scope. Abstracts away SCPI operations which can be transported via USBTMC or
 * TCP/IP.
 */
class idevice
{
  public:
    //! A view into a binary/text buffer
    using buffer_view = std::span<const char>;

    //! Owning buffer
    using buffer_type = std::vector<char>;

    //! Type used to represent timeout duration.
    using timeout_type = std::chrono::milliseconds;

    //! A sentinel used to represent an infinite timeout duration.
    static constexpr auto no_timeout = timeout_type{ 0 };

    //! Convenience default value for timeout.
    static constexpr auto default_timeout = std::chrono::seconds{ 1 };

  public:
    /**
     * @brief Synchronous read until a sentinel.
     *
     * @param time Timeout duration. If it occurs then an exception is thrown.
     * @param delim Delimiter to read the data until. Note that this delimiter is not a part of the returned value.
     *
     * @return This overload with a dummy parameter as_vector_t returns a buffer_type.
     */
    [[nodiscard]] virtual auto read_until( as_vector_t, timeout_type time, std::string_view delim ) -> buffer_type = 0;

    /**
     * @brief Synchronous read until a sentinel.
     *
     * @param time Timeout duration. If it occurs then an exception is thrown.
     * @param delim Delimiter to read the data until. Note that this delimiter is not a part of the returned value.
     *
     * @return This overload with a dummy parameter as_string_t returns a string type for convenience.
     * @remark It pains me to make two overloads with basically the same meaning. However, I don't know of a way to
     * convert a vector into a string without additional copies. Maybe something should be done about this.
     */
    [[nodiscard]] virtual auto read_until( as_string_t, timeout_type time, std::string_view delim ) -> std::string = 0;

    /**
     * @brief Synchronous read of exact number of bytes.
     *
     * @param time Timeout duration. If it occurs then an exception is thrown.
     * @param n Number of bytes to read.
     *
     * @return This overload with a dummy parameter as_string_t returns a string type for convenience.
     */
    [[nodiscard]] virtual auto read_n( as_string_t, std::size_t n, timeout_type ) -> std::string = 0;

    /**
     * @brief Synchronous read of exact number of bytes.
     *
     * @param time Timeout duration. If it occurs then an exception is thrown.
     * @param n Number of bytes to read.
     *
     * @return This overload with a dummy parameter as_vector_t returns a buffer_type.
     */
    [[nodiscard]] virtual auto read_n( as_vector_t, std::size_t n, timeout_type ) -> buffer_type = 0;

    /**
     * @brief Synchronous write.
     *
     * @param data View into a buffer that is to be written to the device
     */
    virtual void write( buffer_view data ) = 0; //< This overload will win for a string literal

  private:
    template <typename command_t>
        requires ( command_t::has_query )
    auto query_impl( timeout_type time = default_timeout )
    {
        const auto message = fmt::format( "{}\n", std::string_view{ command_t::get_query_string() } );
        write( message );
        auto result = read_until( as_string, time, "\n" );
        return command_t::query_parser::parse( result );
    }

    template <typename... commands_t>
        requires ( ( sizeof...( commands_t ) > 1 ) && ( commands_t::has_query && ... ) )
    auto query_impl( timeout_type time = default_timeout )
        -> std::tuple<std::invoke_result_t<decltype( &commands_t::query_parser::parse ), std::string_view>...>
    {
        auto ss = std::stringstream{};
        ( ss << ... << fmt::format( "{}\n", std::string_view{ commands_t::get_query_string() } ) );
        const auto message = ss.str();
        write( message );

        const auto result = read_until( as_string, time, "\n" );
        auto split = std::vector<std::string_view>{};
        boost::split( split, result, boost::is_any_of( ";" ) );

        auto tuple_maker = [ &split]<auto... I>( std::index_sequence<I...> )
        {
            return std::tuple{ commands_t::query_parser::parse( split.at( I ) )... };
        };

        return tuple_maker( std::make_index_sequence<sizeof...( commands_t )>{} );
    }

  public:
    /**
     * @brief A blocking query. First sends a *OPC? query to the device which blocks until all operations have finished.
     * Beware the timeout, because some operations may take a long time to complete.
     *
     * @tparam commands_t A variadic pack of command queries to concat. The query strings get concatenated into a single
     * query with '\n' delimeter.
     *
     * @param time Timeout for the read operations
     * @return Returns a std::tuple of results returned by commands_t::query_parser::parse for each response.
     *
     */
    template <typename... commands_t>
        requires ( commands_t::has_query && ... )
    auto query( block_query_t, timeout_type time = no_timeout )
        -> std::optional<decltype( query_impl<commands_t...>() )>
    {
        if ( auto res = query_impl<scpi::common::opc_cmd>( time ); !res )
        {
            return std::nullopt;
        }

        return std::optional{ query_impl<commands_t...>( time ) };
    }

    /**
     * @brief A non-blocking (for the SCPI device) query operation.
     *
     * @tparam commands_t A variadic pack of command queries to concat. The query strings get concatenated into a single
     * query with '\n' delimeter.
     *
     * @param time Timeout for the read operations
     * @return Returns a std::tuple of results returned by commands_t::query_parser::parse for each response.
     *
     */
    template <typename... commands_t>
        requires ( commands_t::has_query && ... )
    auto query( timeout_type time = no_timeout ) -> decltype( query_impl<commands_t...>() )
    {
        return query_impl<commands_t...>( time );
    }

    template <typename command_t, typename... args_t>
        requires ( command_t::has_operation )
    void submit( args_t&&... args )
    {
        const auto message =
            fmt::format( "{}\n", std::string_view{ command_t::get_command_string( std::forward<args_t>( args )... ) } );
        write( message );
    }

  public:
    virtual ~idevice() = default;
};

/**
 * @brief Implementation of SCPI Rigol DS Scope client over TCP/IP sockets.
 */
class lan_device : public idevice
{
  public:
    [[nodiscard]] auto read_until( as_vector_t, timeout_type, std::string_view delim ) -> buffer_type override;
    [[nodiscard]] auto read_until( as_string_t, timeout_type, std::string_view delim ) -> std::string override;

    [[nodiscard]] auto read_n( as_string_t, std::size_t n, timeout_type = default_timeout ) -> std::string override;
    [[nodiscard]] auto read_n( as_vector_t, std::size_t n, timeout_type = default_timeout ) -> buffer_type override;

    void write( buffer_view data ) override;

  public:
    static constexpr auto device_port = asio::ip::port_type{ 5555 };
    lan_device( std::string_view host, std::string_view port = std::to_string( device_port ) );
    [[nodiscard]] auto endpoint() const -> tcp::endpoint { return m_endpoint; }

  private:
    auto resolve( std::string_view host, std::string_view port ) -> tcp::endpoint;

    template <typename result_t>
    auto read_impl( timeout_type timeout, size_t suffix_size, auto async_func ) -> result_t;

  private:
    boost::asio::io_service m_service;
    asio::streambuf m_streambuf;
    tcp::endpoint m_endpoint;
    tcp::socket m_sock;
};

template <typename result_t>
auto
lan_device::read_impl( timeout_type timeout, size_t suffix_size, auto async_func ) -> result_t
{
    auto timer_handler = [ &sock = m_sock ]( boost::system::error_code code ) {
        if ( code == asio::error::operation_aborted )
        {
            return;
        }

        sock.cancel();

        if ( code )
        {
            throw boost::system::system_error{ code };
        }

        throw std::runtime_error{ "Timeout on read operation: lan_device::read_impl" };
    };

    auto timer = asio::steady_timer{ m_service, std::chrono::steady_clock::now() + timeout };
    if ( timeout != timeout_type{ 0 } )
    {
        timer.async_wait( timer_handler );
    }

    result_t res;
    auto read_handler =
        [ this, &res, &timer, suffix_size ]( boost::system::error_code code, std::size_t num_transferred ) {
            if ( code == asio::error::operation_aborted )
            {
                return;
            }

            timer.cancel();

            if ( code )
            {
                throw boost::system::system_error{ code };
            }

            res.reserve( num_transferred );
            std::copy_n(
                std::istreambuf_iterator<char>{ &m_streambuf },
                num_transferred - suffix_size,
                std::back_inserter( res ) );

            std::istream{ &m_streambuf }.ignore(
                std::numeric_limits<std::streamsize>::max() ); // [NOTE]: Might be error prone

            assert( m_streambuf.size() == 0 && "Buffer should have been emptied" );
        };

    async_func( m_sock, m_streambuf, read_handler );
    m_service.restart();
    m_service.run();

    return res;
}

} // namespace ds