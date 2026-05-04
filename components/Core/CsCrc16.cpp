#include "CsCrc16.h"
#include <esp_log.h>
#include <cstdio>
#include <vector>

using namespace SignatureBridge::Core;

CsCrc16& CsCrc16::Instance()
{
    static CsCrc16 instance;
    return instance;
}

uint16_t CsCrc16::Compute(const uint8_t* buffer, size_t length, uint16_t seed) const
{
    if (buffer == nullptr || length == 0)
    {
        return 0;
    }

    uint16_t crc = seed;

    for (size_t i = 0; i < length; ++i)
    {
        crc = (crc >> 8) | (crc << 8);
        crc ^= buffer[i];
        crc ^= (crc & 0xFF) >> 4;
        crc ^= (crc << 12);
        crc ^= (crc & 0xFF) << 5;
    }

    return crc;
}

bool CsCrc16::Verify(uint16_t crc, const uint8_t* buffer, size_t length, uint16_t seed) const
{
    const uint16_t computedCrc = Compute(buffer, length, seed);

    if (crc != computedCrc)
    {
        ESP_LOGI(
            CsCrc16::Tag(),
            "Invalid CRC [Expected: 0x%04X (%u)] [Actual: 0x%04X (%u)]",
            crc, crc,
            computedCrc, computedCrc
        );

        return false;
    }

    // Append the CRC (big-endian) to the buffer and recompute — residue must be 0x0000.
    std::vector<uint8_t> bufferWithCrc(buffer, buffer + length);
    bufferWithCrc.push_back(static_cast<uint8_t>((crc >> 8) & 0xFF));
    bufferWithCrc.push_back(static_cast<uint8_t>(crc & 0xFF));

    return Compute(bufferWithCrc.data(), bufferWithCrc.size(), seed) == 0x0000;
}
