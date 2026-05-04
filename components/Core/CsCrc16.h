#pragma once
#include "CsCore.h"
#include <cstdint>
#include <cstddef>

namespace SignatureBridge::Core {

class CsCrc16
{
    CS_CLASS_TAG(CsCrc16)

public:
    /**
     * Get the singleton instance of CsCrc16.
     *
     * @return Reference to the singleton instance of CsCrc16.
     */
    static CsCrc16& Instance();

private:
    /**
     * Private constructor to enforce singleton pattern. This prevents external code from creating instances of CsCrc16,
     * ensuring that only one instance exists and is accessed through the Instance() method.
     */
    CsCrc16() = default;

public:
    /**
     * Computes the CRC-16/KERMIT (CRC-16/CCITT) checksum for the given data buffer.
     *
     * Adopted from the CRC-16-CCITT algorithm originally written in C by Intel, as included in Zephyr.
     *
     * Copyright (c) 2017 Intel Corporation
     * SPDX-License-Identifier: Apache-2.0
     *
     * @param[in] buffer The byte buffer for which to compute the CRC-16 checksum.
     * @param[in] length The length of the byte buffer.
     * @param[in] seed The initial CRC value (default: 0xFFFF).
     *
     * @return The computed CRC-16 checksum, or 0 if the buffer is empty/null.
     */
    uint16_t Compute(const uint8_t* buffer, size_t length, uint16_t seed = DefaultSeed) const;

    /**
     * Verifies that a transmitted CRC-16 value is valid for the given buffer by re-computing the CRC and checking the
     * residue. For CRC-16/KERMIT the residue is always 0x0000.
     *
     * See https://reveng.sourceforge.io/crc-catalogue/16.htm for more information.
     *
     * @param[in] crc    The CRC value to verify.
     * @param[in] buffer The original message the CRC was computed from.
     * @param[in] length The length of the buffer.
     * @param[in] seed   The seed value used when the CRC was originally computed (default: 0xFFFF).
     *
     * @return True if the CRC is valid for the given buffer; otherwise false.
     */
    bool Verify(uint16_t crc, const uint8_t* buffer, size_t length, uint16_t seed = DefaultSeed) const;

public:
    static constexpr uint16_t DefaultSeed = 0xFFFF;
};

}
