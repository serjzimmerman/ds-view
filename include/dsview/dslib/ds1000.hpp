#pragma once

#include "device.hpp"

#include <memory>

namespace ds
{

class ds1000
{
  private:
    ds1000( std::unique_ptr<idevice> device_ptr )
        : m_device{ std::move( device_ptr ) }
    {
    }

  public:
    static auto open_lan( std::string_view host, std::string_view port ) -> ds1000
    {
        return ds1000{ std::make_unique<lan_device>( host, port ) };
    }

  private:
    std::unique_ptr<idevice> m_device;
};

}; // namespace ds