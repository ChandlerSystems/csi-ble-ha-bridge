#include "CsCoreUtils.h"
#include <cctype>
#include <sstream>
#include <iomanip>

using namespace SignatureBridge::Core;

CsCoreUtils& CsCoreUtils::Instance()
{
    static CsCoreUtils instance;
    return instance;
}

bool CsCoreUtils::ValueToBool(const int32_t value) const
{
    return value != 0;
}

std::string CsCoreUtils::FirmwareVersionToString(const uint32_t version) const
{
    const int major = version / 100;
    const int minor = version % 100;

    return std::to_string(major) + "." + std::to_string(minor);
}

bool CsCoreUtils::TryPacketValueArrayToBytes(
    const std::vector<CsBlePacketValue>& values,
    std::vector<uint8_t>& bytes,
    const bool trimTrailingZeros
) const
{
    bytes.clear();
    bytes.reserve(values.size());

    for (const auto& value : values)
    {
        if (!value.IsInt())
        {
            return false;
        }

        if (value.IntValue < 0 || value.IntValue > 255)
        {
            return false;
        }

        bytes.push_back(static_cast<uint8_t>(value.IntValue));
    }

    if (trimTrailingZeros)
    {
        while (!bytes.empty() && bytes.back() == 0U)
        {
            bytes.pop_back();
        }
    }

    return true;
}

std::string CsCoreUtils::BytesToHexString(const std::vector<uint8_t>& data, bool spaced)
{
    if (data.empty())
    {
        return spaced ? "<empty>" : "";
    }

    static constexpr char Hex[] = "0123456789ABCDEF";

    std::string result;
    result.reserve(spaced ? data.size() * 3 : data.size() * 2);

    for (size_t i = 0; i < data.size(); ++i)
    {
        const auto byte = data[i];

        result.push_back(Hex[(byte >> 4U) & 0x0FU]);
        result.push_back(Hex[byte & 0x0FU]);

        if (spaced && i < data.size() - 1)
        {
            result.push_back(' ');
        }
    }

    return result;
}

std::string CsCoreUtils::BytesToHexAsciiString(const uint8_t* buffer, size_t length) const
{
    if (buffer == nullptr || length == 0U)
    {
        return "<empty>";
    }

    std::ostringstream hexStream;
    std::string ascii;
    ascii.reserve(length);

    for (size_t i = 0; i < length; ++i)
    {
        if (i > 0)
        {
            hexStream << ' ';
        }

        hexStream << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(buffer[i]);

        const unsigned char ch = buffer[i];
        ascii.push_back(std::isprint(ch) ? static_cast<char>(ch) : '.');
    }

    return hexStream.str() + " | " + ascii;
}

std::string CsCoreUtils::BytesToHexAsciiString(const std::vector<uint8_t>& bytes) const
{
    return BytesToHexAsciiString(bytes.data(), bytes.size());
}

std::string CsCoreUtils::Utf8BytesToString(const std::vector<uint8_t>& bytes) const
{
    return std::string(bytes.begin(), bytes.end());
}

uint16_t CsCoreUtils::ReadLittleEndianU16(const uint8_t* buffer, const size_t length)
{
    return buffer == nullptr || length < 2U ?
        0 :
        static_cast<uint16_t>(buffer[0]) | (static_cast<uint16_t>(buffer[1]) << 8U);
}

void CsCoreUtils::WriteLittleEndianU16(uint8_t* buffer, const uint16_t value) const
{
    buffer[0] = static_cast<uint8_t>(value & 0xFF);
    buffer[1] = static_cast<uint8_t>((value >> 8) & 0xFF);
}

uint16_t CsCoreUtils::ReadBigEndianU16(const uint8_t* buffer, const size_t length) const
{
    return buffer == nullptr || length < 2U ?
        0 :
        static_cast<uint16_t>((static_cast<uint16_t>(buffer[0]) << 8) | buffer[1]);
}

void CsCoreUtils::WriteBigEndianU16(uint8_t* buffer, const uint16_t value) const
{
    buffer[0] = static_cast<uint8_t>((value >> 8) & 0xFF);
    buffer[1] = static_cast<uint8_t>(value & 0xFF);
}

uint32_t CsCoreUtils::ReadLittleEndianU32(const uint8_t* buffer, const size_t length)
{
    if (buffer == nullptr || length < 4U)
    {
        return 0;
    }

    return static_cast<uint32_t>(buffer[0]) |
           (static_cast<uint32_t>(buffer[1]) << 8) |
           (static_cast<uint32_t>(buffer[2]) << 16) |
           (static_cast<uint32_t>(buffer[3]) << 24);
}

void CsCoreUtils::WriteLittleEndianU32(uint8_t* buffer, const uint32_t value) const
{
    if (buffer == nullptr)
    {
        return;
    }

    buffer[0] = static_cast<uint8_t>(value & 0xFF);
    buffer[1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    buffer[2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    buffer[3] = static_cast<uint8_t>((value >> 24) & 0xFF);
}

uint32_t CsCoreUtils::ReadBigEndianU32(const uint8_t* buffer, const size_t length) const
{
    if (buffer == nullptr || length < 4U)
    {
        return 0;
    }

    return (static_cast<uint32_t>(buffer[0]) << 24) |
           (static_cast<uint32_t>(buffer[1]) << 16) |
           (static_cast<uint32_t>(buffer[2]) << 8) |
            static_cast<uint32_t>(buffer[3]);
}

void CsCoreUtils::WriteBigEndianU32(uint8_t* buffer, const uint32_t value) const
{
    if (buffer == nullptr)
    {
        return;
    }

    buffer[0] = static_cast<uint8_t>((value >> 24) & 0xFF);
    buffer[1] = static_cast<uint8_t>((value >> 16) & 0xFF);
    buffer[2] = static_cast<uint8_t>((value >> 8) & 0xFF);
    buffer[3] = static_cast<uint8_t>(value & 0xFF);
}
