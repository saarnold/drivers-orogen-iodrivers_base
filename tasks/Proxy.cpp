/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Proxy.hpp"
#include <iodrivers_base/Driver.hpp>

using namespace iodrivers_base;

namespace
{
    class DummyDriver : public Driver
    {
    public:
        DummyDriver()
            : Driver(iodrivers_base::Proxy::DUMMY_BUFFER_SIZE) {}

        int extractPacket(boost::uint8_t const* buffer, size_t size) const
        {
            return size;
        }
    };
}

Proxy::Proxy(std::string const& name)
    : ProxyBase(name)
{
}

Proxy::Proxy(std::string const& name, RTT::ExecutionEngine* engine)
    : ProxyBase(name, engine)
{
}

Proxy::~Proxy()
{
}

/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See Proxy.hpp for more detailed
// documentation about them.

bool Proxy::configureHook()
{
    buffer_size = createProxyDriver();
    rx_packet.data.reserve(buffer_size);
    tx_packet.data.reserve(buffer_size);
    packet_buffer.resize(buffer_size);
    if (!_io_port.get().empty())
        mDriver->openURI(_io_port.get());
    if (! ProxyBase::configureHook())
        return false;

    return true;
}

int Proxy::createProxyDriver()
{
    setDriver(new DummyDriver);
    return DUMMY_BUFFER_SIZE;
}

bool Proxy::startHook()
{
    if (! ProxyBase::startHook())
        return false;
    return true;
}
void Proxy::updateHook()
{
    ProxyBase::updateHook();

    while (_tx.read(tx_packet, false) == RTT::NewData)
        mDriver->writePacket(tx_packet.data.data(), tx_packet.data.size());
}
void Proxy::processIO()
{
    try
    {
        int packet_size = mDriver->readPacket(packet_buffer.data(), buffer_size);
        rx_packet.time = base::Time::now();
        rx_packet.data.resize(packet_size);
        memcpy(rx_packet.data.data(), packet_buffer.data(), packet_size);
        _rx.write(rx_packet);
    }
    catch (TimeoutError)
    {
    }
}

void Proxy::errorHook()
{
    ProxyBase::errorHook();
}
void Proxy::stopHook()
{
    ProxyBase::stopHook();
}
void Proxy::cleanupHook()
{
    ProxyBase::cleanupHook();
    delete mDriver;
    mDriver = 0;
}

