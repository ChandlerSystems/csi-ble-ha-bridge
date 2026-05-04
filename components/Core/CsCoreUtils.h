#pragma once
#include "CsBlePacketValue.h"
#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace SignatureBridge::Core {

using namespace SignatureBridge::BluetoothCore::BleCore;

class CsCoreUtils
{
public:
    /**
     * Get the instance of the CsCoreUtils singleton.
     *
     * @return A reference to the CsCoreUtils instance.
     */
    static CsCoreUtils& Instance();

private:
    /**
     * Private constructor to enforce singleton pattern. This prevents external code from creating instances of
     * CsCoreUtils, ensuring that only one instance exists and is accessed through the Instance() method.
     */
    CsCoreUtils() = default;

public:
    /**
     * Converts an integer value to a boolean. In this implementation, any non-zero value is considered true, while zero
     * is considered false. This can be useful for interpreting values from BLE advertisements or characteristics that
     * may represent boolean states as integers.
     *
     * @param[in] value The integer value to convert.
     *
     * @return True if the value is non-zero, false otherwise.
     */
    bool ValueToBool(const int32_t value) const;

    /**
     * Converts a firmware version represented as an integer into a human-readable string format.
     *
     * @param[in] version The integer value representing the firmware version.
     *
     * @return A string representation of the firmware version.
     */
    std::string FirmwareVersionToString(const uint32_t version) const;

    /**
     * Converts an array of CsBlePacketValue objects, which are expected to be integers in the range of 0-255, into a
     * vector of bytes. Optionally, trailing zeros can be trimmed from the resulting byte vector.
     *
     * @param[in] values The array of CsBlePacketValue objects to convert.
     * @param[out] bytes The vector to store the resulting bytes.
     * @param[in] trimTrailingZeros Whether to remove trailing zeros from the resulting byte vector.
     * @return True if the conversion was successful, false otherwise.
     */
    bool TryPacketValueArrayToBytes(
        const std::vector<CsBlePacketValue>& values,
        std::vector<uint8_t>& bytes,
        bool trimTrailingZeros = false
    ) const;

public:
    /**
     * Converts a vector of bytes into a hexadecimal string representation. Each byte is represented as two hexadecimal
     * characters, and bytes are separated by spaces. For example, a vector containing the bytes {0x01, 0x23, 0x45}
     * would be converted to the string "01 23 45". If the input vector is empty, the function returns the string
     * "<empty>".
     *
     * @param[in] data The vector of bytes to convert.
     * @return A string representing the hexadecimal values of the bytes.
     */
    std::string BytesToHexString(const std::vector<uint8_t>& data, bool spaced = true);

    /**
     * Converts a byte buffer into a hexadecimal string representation.
     *
     * @param[in] buffer The byte buffer to convert.
     * @param[in] length The length of the byte buffer.
     *
     * @return A string representing the hexadecimal values of the bytes on success, or "<empty>" if the input vector
     *         is empty.
     */
    std::string BytesToHexAsciiString(const uint8_t* buffer, size_t length) const;

    /**
     * Converts a vector of bytes into a hexadecimal string representation.
     *
     * @param[in] bytes The vector of bytes to convert.
     *
     * @return A string representing the hexadecimal values of the bytes on success, or "<empty>" if the input vector
     *         is empty.
     */
    std::string BytesToHexAsciiString(const std::vector<uint8_t>& bytes) const;

    /**
     * Converts a vector of bytes that represent UTF-8 encoded data into a standard string.
     *
     * @param[in] bytes The vector of bytes to convert.
     *
     * @return A string representing the UTF-8 encoded data.
     */
    std::string Utf8BytesToString(const std::vector<uint8_t>& bytes) const;

public:
    /**
     * Reads a little-endian unsigned 16-bit integer from the given byte buffer. This is commonly used for parsing
     * values from BLE advertisements and characteristics that are encoded in little-endian format.
     *
     * @param[in] buffer The byte buffer containing the little-endian encoded data.
     * @param[in] length The length of the byte buffer.
     *
     * @return The decoded 16-bit unsigned integer.
     */
    uint16_t ReadLittleEndianU16(const uint8_t* buffer, const size_t length);

    /**
     * Writes a 16-bit unsigned integer to the given byte buffer in little-endian format. This is useful for
     * constructing packets or messages that require little-endian encoding.
     *
     * @param[out] buffer The byte buffer where the little-endian encoded value will be written. Must have space for at
     *                    least 2 bytes.
     * @param[in] value The 16-bit unsigned integer value to encode and write to the buffer.
     */
    void WriteLittleEndianU16(uint8_t* buffer, const uint16_t value) const;

    /**
     * Reads a big-endian unsigned 16-bit integer from the given byte buffer. This can be used for parsing values that
     * are encoded in big-endian format.
     *
     * @param[in] buffer The byte buffer containing the big-endian encoded data.
     * @param[in] length The length of the byte buffer.
     *
     * @return The decoded 16-bit unsigned integer.
     */
    uint16_t ReadBigEndianU16(const uint8_t* buffer, const size_t length) const;

    /**
     * Writes a 16-bit unsigned integer to the given byte buffer in big-endian format. This is useful for constructing
     * packets or messages that require big-endian encoding.
     *
     * @param[out] buffer The byte buffer where the big-endian encoded value will be written. Must have space for at
     *                    least 2 bytes.
     * @param[in] value The 16-bit unsigned integer value to encode and write to the buffer.
     */
    void WriteBigEndianU16(uint8_t* buffer, const uint16_t value) const;

public:
    /**
     * Reads a little-endian unsigned 32-bit integer from the given byte buffer. This is commonly used for parsing
     * values from BLE advertisements and characteristics that are encoded in little-endian format.
     *
     * @param[in] buffer The byte buffer containing the little-endian encoded data.
     * @param[in] length The length of the byte buffer.
     *
     * @return The decoded 32-bit unsigned integer.
     */
    uint32_t ReadLittleEndianU32(const uint8_t* buffer, const size_t length);

    /**
     * Writes a 32-bit unsigned integer to the given byte buffer in little-endian format. This is useful for
     * constructing packets or messages that require little-endian encoding.
     *
     * @param[out] buffer The byte buffer where the little-endian encoded value will be written. Must have space for at
     *                    least 4 bytes.
     * @param[in] value The 32-bit unsigned integer value to encode and write to the buffer.
     */
    void WriteLittleEndianU32(uint8_t* buffer, const uint32_t value) const;

    /**
     * Reads a big-endian unsigned 32-bit integer from the given byte buffer. This can be used for parsing values that
     * are encoded in big-endian format.
     *
     * @param[in] buffer The byte buffer containing the big-endian encoded data.
     * @param[in] length The length of the byte buffer.
     *
     * @return The decoded 32-bit unsigned integer.
     */
    uint32_t ReadBigEndianU32(const uint8_t* buffer, const size_t length) const;

    /**
     * Writes a 32-bit unsigned integer to the given byte buffer in big-endian format. This is useful for constructing
     * packets or messages that require big-endian encoding.
     *
     * @param[out] buffer The byte buffer where the big-endian encoded value will be written. Must have space for at
     *                    least 4 bytes.
     * @param[in] value The 32-bit unsigned integer value to encode and write to the buffer.
     */
    void WriteBigEndianU32(uint8_t* buffer, const uint32_t value) const;

public:
    /**
     * Converts a byte value into an array of bits, where each element in the array represents a single bit of the byte.
     * The least significant bit (LSB) is at index 0, and the most significant bit (MSB) is at index 7.
     *
     * @param[in] value The byte value to convert.
     *
     * @return An array of bits representing the byte value.
     */
    static inline std::array<uint8_t, 8> ToBits(const uint8_t value)
    {
        return {
            static_cast<uint8_t>((value >> 0) & 1),
            static_cast<uint8_t>((value >> 1) & 1),
            static_cast<uint8_t>((value >> 2) & 1),
            static_cast<uint8_t>((value >> 3) & 1),
            static_cast<uint8_t>((value >> 4) & 1),
            static_cast<uint8_t>((value >> 5) & 1),
            static_cast<uint8_t>((value >> 6) & 1),
            static_cast<uint8_t>((value >> 7) & 1),
        };
    }

    /**
     * Gets the value of a specific bit from a byte. The index is zero-based, where index 0 corresponds to the least
     * significant bit (LSB) and index 7 corresponds to the most significant bit (MSB).
     *
     * @param[in] value The byte value from which to extract the bit.
     * @param[in] index The zero-based index of the bit to retrieve.
     *
     * @return True if the specified bit is set, false otherwise.
     */
    static inline bool GetBit(const uint8_t value, const uint8_t index)
    {
        return (value >> index) & 0x01;
    }
};

}
