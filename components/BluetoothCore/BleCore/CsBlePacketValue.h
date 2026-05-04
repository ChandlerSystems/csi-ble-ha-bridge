#pragma once
#include "CsBlePacketValueType.h"
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

namespace SignatureBridge::BluetoothCore::BleCore {

struct CsBlePacketObjectEntry;

struct CsBlePacketValue
{
public:
    CsBlePacketValueType Type = CsBlePacketValueType::Unknown;
    int32_t IntValue = 0;
    std::vector<uint8_t> ByteArrayValue;
    std::vector<CsBlePacketValue> ArrayValue;
    std::vector<std::pair<std::string, CsBlePacketValue>> ObjectValue;

public:
    [[nodiscard]] bool IsInt() const;
    [[nodiscard]] bool IsByteArray() const;
    [[nodiscard]] bool IsArray() const;
    [[nodiscard]] bool IsObject() const;
    [[nodiscard]] bool IsArrayOfInt() const;

public:
    [[nodiscard]] std::vector<int32_t> ToIntArray() const;
    static const std::string ToString(CsBlePacketValueType type);
    static std::string ToDebugString(const CsBlePacketValue& value, size_t maxDepth = 4U);

private:
    static void AppendDebugString(
        std::ostringstream& stream,
        const CsBlePacketValue& value,
        size_t depth,
        size_t maxDepth = 4U
    );
};

struct CsBlePacketObjectEntry
{
    std::string Key;
    CsBlePacketValue Value;
};

}
