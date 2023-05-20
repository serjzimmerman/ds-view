/*
 * ----------------------------------------------------------------------------
 *  "THE BEER-WARE LICENSE" (Revision 42):
 *  <tsimmerman.ss@phystech.edu> wrote this file. As long as you retain this notice you
 *  can do whatever you want with this stuff. If we meet some day, and you think
 *  this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

#pragma once

#include "detail/common.hpp"

#include "scpi/commands/common.hpp"

#include <fmt/format.h>

#include <chrono>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <optional>
#include <span>
#include <vector>

namespace ds
{

struct block_query_t
{
};

static constexpr auto block_query = block_query_t{};

class idevice
{
  public:
    using buffer_view = std::span<const char>;
    using buffer_type = std::vector<char>;
    using timeout_type = std::chrono::milliseconds;

    static constexpr auto no_timeout = timeout_type{ 0 };
    static constexpr auto default_timeout = std::chrono::seconds{ 1 };

  public:
    [[nodiscard]] virtual auto read_until( as_vector_t, timeout_type, std::string_view delim ) -> buffer_type = 0;
    [[nodiscard]] virtual auto read_until( as_string_t, timeout_type, std::string_view delim ) -> std::string = 0;

    [[nodiscard]] virtual auto read_n( as_string_t, std::size_t n, timeout_type ) -> std::string = 0;
    [[nodiscard]] virtual auto read_n( as_vector_t, std::size_t n, timeout_type ) -> buffer_type = 0;

    virtual void write( buffer_view data ) = 0; //< This overload will win for a string literal

  public:
    template <typename command_t>
        requires ( command_t::has_query )
    auto query( timeout_type time = default_timeout )
    {
        const auto message = fmt::format( "{}\n", std::string_view{ command_t::get_query_string() } );
        write( message );
        auto result = read_until( as_string, time, "\n" ); // TODO[]: Make this customizable for commands that query
                                                           // binary data. Plus add support for concatenated commands.
        return command_t::query_parser::parse( result );
    }

    template <typename command_t>
        requires ( command_t::has_query )
    auto query( block_query_t, timeout_type time = default_timeout )
        -> std::optional<std::invoke_result_t<decltype( &command_t::query_parser::parse ), std::string_view>>
    {
        if ( auto res = query<scpi::common::opc_cmd>( time ); !res )
        {
            return std::nullopt;
        }

        return std::optional{ query<command_t>( time ) };
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

    template <typename result_t> auto read_impl( timeout_type timeout, auto async_func ) -> result_t;

  private:
    boost::asio::io_service m_service;
    asio::streambuf m_streambuf;
    tcp::endpoint m_endpoint;
    tcp::socket m_sock;
};

template <typename result_t>
auto
lan_device::read_impl( timeout_type timeout, auto async_func ) -> result_t
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
    auto read_handler = [ this, &res, &timer ]( boost::system::error_code code, std::size_t num_transferred ) {
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
        std::copy(
            std::istreambuf_iterator<char>{ &m_streambuf },
            std::istreambuf_iterator<char>{},
            std::back_inserter( res ) );

        assert( m_streambuf.size() == 0 && "Buffer should have been emptied" );
    };

    async_func( m_sock, m_streambuf, read_handler );
    m_service.restart();
    m_service.run();

    return res;
}

} // namespace ds