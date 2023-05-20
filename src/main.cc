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
    auto device = std::make_unique<ds::lan_device>( "192.168.50.78" );

    auto parsed = device->query<ds::scpi::common::idn_cmd>( ds::block_query, ds::lan_device::no_timeout );
    if ( !parsed )
    {
        std::cout << "Unavailable\n";
        return 0;
    }

    std::cout << fmt::format(
        "Model: {}, Serial number: {}, Software version: {}\n",
        to_string( parsed->model ),
        parsed->serial_number,
        parsed->software_version );

    device->submit<ds::scpi::common::rst_cmd>();
}
