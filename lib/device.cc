#include "dsview/dslib/device.hpp"

#include <boost/format.hpp>

#include <algorithm>
#include <cassert>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <streambuf>
#include <string_view>

namespace ds
{

auto
lan_device::resolve( std::string_view host, std::string_view port ) -> tcp::endpoint
{
    auto resolver = tcp::resolver{ m_service };
    auto result = resolver.resolve( tcp::v4(), host, port );

    if ( result.empty() )
    {
        throw std::runtime_error{ str( boost::format( "Could not resolve address %s" ) % host ) };
    }

    return result.begin()->endpoint();
}

lan_device::lan_device( std::string_view host, std::string_view port )
    : m_endpoint{ resolve( host, port ) },
      m_sock{ m_service }
{
    m_sock.open( tcp::v4() );
    m_sock.connect( m_endpoint );
}

void
lan_device::write( buffer_view data )
{
    asio::write( m_sock, asio::buffer( data ), asio::transfer_all() );
}

auto
lan_device::read_until( as_vector_t, timeout_type timeout, std::string_view delim ) -> buffer_type
{
    return read_impl<buffer_type>( timeout, delim.length(), [ delim ]( auto& sock, auto& buf, auto&& handler ) {
        return asio::async_read_until( sock, buf, delim, handler );
    } );
}

auto
lan_device::read_n( as_vector_t, std::size_t n, timeout_type timeout ) -> buffer_type
{
    return read_impl<buffer_type>( timeout, 0, [ n ]( auto& sock, auto& buf, auto&& handler ) {
        return asio::async_read( sock, buf, asio::transfer_exactly( n ), handler );
    } );
}

auto
lan_device::read_until( as_string_t, timeout_type timeout, std::string_view delim ) -> std::string
{
    return read_impl<std::string>( timeout, delim.length(), [ delim ]( auto& sock, auto& buf, auto&& handler ) {
        return asio::async_read_until( sock, buf, delim, handler );
    } );
}

auto
lan_device::read_n( as_string_t, std::size_t n, timeout_type timeout ) -> std::string
{
    return read_impl<std::string>( timeout, 0, [ n ]( auto& sock, auto& buf, auto&& handler ) {
        return asio::async_read( sock, buf, asio::transfer_exactly( n ), handler );
    } );
}

} // namespace ds