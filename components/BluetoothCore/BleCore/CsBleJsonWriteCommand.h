#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace SignatureBridge::BluetoothCore::BleCore {

class CsBleJsonWriteCommand
{
public:
    /**
     * Constructs a CsBleJsonWriteCommand with the specified JSON string.
     *
     * @param[in] json The JSON string.
     */
    explicit CsBleJsonWriteCommand(std::string json): _json(std::move(json)) { }

public:
    /**
     * Gets the JSON string associated with this command.
     *
     * @return The JSON string.
     */
    const std::string& Json() const
    {
        return _json;
    }

    /**
     * Converts the JSON string to a vector of bytes, suitable for transmission over Bluetooth.
     *
     * @return The vector of bytes.
     */
    std::vector<uint8_t> ToBytes() const
    {
        return std::vector<uint8_t>(_json.begin(), _json.end());
    }

private:
    std::string _json;
};

}
