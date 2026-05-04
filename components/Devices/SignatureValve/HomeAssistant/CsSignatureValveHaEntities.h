#pragma once

#include "CsSignatureValveHaEntityDefinition.h"

#include <cstddef>

namespace SignatureBridge::Devices::SignatureValve::HomeAssistant
{
    const CsSignatureValveHaEntityDefinition* GetSignatureValveHaEntities(size_t& count);
}
