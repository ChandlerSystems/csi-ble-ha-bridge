#include "CsBlePacketValue.h"

using namespace SignatureBridge::BluetoothCore::BleCore;

[[nodiscard]] bool CsBlePacketValue::IsInt() const
{
    return Type == CsBlePacketValueType::Int;
}

[[nodiscard]] bool CsBlePacketValue::IsByteArray() const
{
    return Type == CsBlePacketValueType::ByteArray;
}

[[nodiscard]] bool CsBlePacketValue::IsArray() const
{
    return Type == CsBlePacketValueType::Array;
}

[[nodiscard]] bool CsBlePacketValue::IsObject() const
{
    return Type == CsBlePacketValueType::Object;
}

[[nodiscard]] bool CsBlePacketValue::IsArrayOfInt() const
{
    if (!IsArray())
    {
        return false;
    }

    for (const auto& item : ArrayValue)
    {
        if (!item.IsInt())
        {
            return false;
        }
    }

    return true;
}

[[nodiscard]] std::vector<int32_t> CsBlePacketValue::ToIntArray() const
{
    std::vector<int32_t> values;

    if (!IsArrayOfInt())
    {
        return values;
    }

    values.reserve(ArrayValue.size());

    for (const auto& item : ArrayValue)
    {
        values.push_back(item.IntValue);
    }

    return values;
}

const std::string CsBlePacketValue::ToString(CsBlePacketValueType type)
{
    switch (type)
    {
        case CsBlePacketValueType::Int:       return "Int";
        case CsBlePacketValueType::ByteArray: return "ByteArray";
        case CsBlePacketValueType::Array:     return "Array";
        case CsBlePacketValueType::Object:    return "Object";
        case CsBlePacketValueType::Unknown:   return "Unknown";
        default:                              return "Invalid";
    }
}

std::string CsBlePacketValue::ToDebugString(const CsBlePacketValue& value, size_t maxDepth)
{
    std::ostringstream stream;
    AppendDebugString(stream, value, 0U, maxDepth);
    return stream.str();
}

void CsBlePacketValue::AppendDebugString(
    std::ostringstream& stream,
    const CsBlePacketValue& value,
    size_t depth,
    size_t maxDepth
)
{
    if (depth >= maxDepth)
    {
        stream << "...";
        return;
    }

    switch (value.Type)
    {
        case CsBlePacketValueType::Int:
            stream << value.IntValue;
            break;

        case CsBlePacketValueType::ByteArray:
        {
            stream << "\"";

            for (const auto byte : value.ByteArrayValue)
            {
                const bool isPrintable = (byte >= ' ' && byte <= '~');

                if (isPrintable)
                {
                    stream << static_cast<char>(byte);
                }
                else
                {
                    stream << ".";
                }
            }

            stream << "\"";
            break;
        }

        case CsBlePacketValueType::Array:
        {
            stream << "[";

            for (size_t i = 0; i < value.ArrayValue.size(); ++i)
            {
                if (i > 0U)
                {
                    stream << ", ";
                }

                AppendDebugString(stream, value.ArrayValue[i], depth + 1U, maxDepth);
            }

            stream << "]";
            break;
        }

        case CsBlePacketValueType::Object:
        {
            stream << "{";

            for (size_t i = 0; i < value.ObjectValue.size(); ++i)
            {
                const auto& entry = value.ObjectValue[i];

                if (i > 0U)
                {
                    stream << ", ";
                }

                stream << entry.first << ": ";
                AppendDebugString(stream, entry.second, depth + 1U, maxDepth);
            }

            stream << "}";
            break;
        }

        case CsBlePacketValueType::Unknown:
        default:
            stream << "?";
            break;
    }
}
