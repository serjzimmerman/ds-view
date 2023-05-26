#include "dsview/dslib.hpp"
#include "dsview/dslib/scpi/commands/ds1000.hpp"

#include <fmt/core.h>
#include <fmt/format.h>

#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>

using namespace std::literals;

auto
main() -> int
{
    using namespace ds::scpi;

    auto device = std::make_unique<ds::lan_device>( "192.168.50.78" );
    auto res = device->query<common::idn_cmd, common::idn_cmd>( ds::block_query );

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

    auto screenshot = device->query<ds1000::display::data_cmd>();
    std::ofstream{ "screenshot.bmp32" } << screenshot;
}
