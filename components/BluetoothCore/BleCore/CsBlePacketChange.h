#pragma once
#include "CsBlePacketValue.h"
#include <string>

namespace SignatureBridge::BluetoothCore::BleCore {

struct CsBlePacketChange
{
    /**
     * The key of the packet change. This is used to identify the specific attribute or property that has changed.
     */
    std::string Key;

    /**
     * The value of the packet change. This represents the new value of the attribute or property.
     */
    CsBlePacketValue Value;
};

}
