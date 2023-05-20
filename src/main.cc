#include "dsview/dslib.hpp"

#include <fmt/core.h>
#include <fmt/format.h>

#include <array>
#include <chrono>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>

using namespace std::literals;

auto
main() -> int
{
    using ds::scpi::common::idn_cmd;

    auto device = std::make_unique<ds::lan_device>( "192.168.50.78" );
    auto res = device->query<idn_cmd, idn_cmd>( ds::block_query );

    if ( !res )
    {
        std::cout << "Unavailable\n";
        return 0;
    }

    auto [ parsed1, parsed2 ] = res.value();

    auto print_info = []( std::string_view msg, ds::scpi::common::identify_result parsed ) {
        std::cout << fmt::format(
            "Message: {}, Model: {}, Serial number: {}, Software version: {}\n",
            msg,
            to_string( parsed.model ),
            parsed.serial_number,
            parsed.software_version );
    };

    print_info( "First", parsed1 );
    print_info( "Second", parsed2 );
}
