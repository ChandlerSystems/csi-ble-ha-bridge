#pragma once
#include "CsSignatureValvePacketKey.h"
#include <string_view>
#include <string>

namespace SignatureBridge::Devices::SignatureValve::SignatureCore {

class CsSignatureValvePacketKeyMapper
{
public:
    static CsSignatureValvePacketKey FromRawKey(std::string_view key);
    static std::string ToRawKey(CsSignatureValvePacketKey key);
};

}
