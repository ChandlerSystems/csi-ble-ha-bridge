#pragma once
#include <cstdint>
#include <esp_log.h>
#include <string>

namespace SignatureBridge::Devices::SignatureValve::SignatureCore {

namespace CsSignatureValveRegenStepStrings
{
    constexpr const char* Unknown = "Unknown";
    constexpr const char* BrineSoak = "Brine Soak";
    constexpr const char* Service = "Service";
    constexpr const char* Standby = "Standby";
    constexpr const char* Backwash = "Backwash";
    constexpr const char* Backwash2 = "Second Backwash";
    constexpr const char* BrineDraw = "Brine Draw";
    constexpr const char* RapidRinse = "Rapid Rinse";
    constexpr const char* BrineFill = "Brine Fill";
    constexpr const char* Rest = "Rest";
    constexpr const char* ChlorineFeed = "Chlorine Feed";
    constexpr const char* AirRelease = "Air Release";
    constexpr const char* AirDraw = "Air Draw";
    constexpr const char* AirChlorineDraw = "Air / Chlorine Draw";
    constexpr const char* ChlorineRinse = "Chlorine Rinse";
    constexpr const char* ChlorineContact = "Chlorine Contact";
    constexpr const char* Decompress = "Decompress";
    constexpr const char* Shutoff = "Shutoff";
}

enum class CsSignatureValveType : uint8_t
{
    Unknown,
    MeteredSoftener01,
    TimeclockSoftener02,
    MeteredSoftener03,
    BackwashingFilter04,
    BackwashingFilter05,
    HydroxRFilter06,
    ReactRFilter07,
    UltraFilter08,
    AerationFilter09,
    AerationFilter10,
    AerationFilter11,
    AerationFilter12,
    AerationFilter13,
    AerationFilter14,
    AerationFilter15,
    AerationFilter16,
    MeteredSoftener17,
    BackwashingFilter18,
    MeteredSoftener19,
    BackwashingFilter20,
    MeteredSoftener21,
    BackwashingFilter22,
    AerationFilter23,
    AerationFilter24,
    AerationFilter25,
    BackwashingFilter26,
    BackwashingFilter27
};

inline CsSignatureValveType CsSignatureValveTypeTryFromInt(int32_t value)
{
    switch (value)
    {
        case 0: return CsSignatureValveType::Unknown;
        case 1: return CsSignatureValveType::MeteredSoftener01  ;
        case 2: return CsSignatureValveType::TimeclockSoftener02;
        case 3: return CsSignatureValveType::MeteredSoftener03;
        case 4: return CsSignatureValveType::BackwashingFilter04;
        case 5: return CsSignatureValveType::BackwashingFilter05;
        case 6: return CsSignatureValveType::HydroxRFilter06;
        case 7: return CsSignatureValveType::ReactRFilter07;
        case 8: return CsSignatureValveType::UltraFilter08;
        case 9: return CsSignatureValveType::AerationFilter09;
        case 10: return CsSignatureValveType::AerationFilter10;
        case 11: return CsSignatureValveType::AerationFilter11;
        case 12: return CsSignatureValveType::AerationFilter12;
        case 13: return CsSignatureValveType::AerationFilter13;
        case 14: return CsSignatureValveType::AerationFilter14;
        case 15: return CsSignatureValveType::AerationFilter15;
        case 16: return CsSignatureValveType::AerationFilter16;
        case 17: return CsSignatureValveType::MeteredSoftener17;
        case 18: return CsSignatureValveType::BackwashingFilter18;
        case 19: return CsSignatureValveType::MeteredSoftener19;
        case 20: return CsSignatureValveType::BackwashingFilter20;
        case 21: return CsSignatureValveType::MeteredSoftener21;
        case 22: return CsSignatureValveType::BackwashingFilter22;
        case 23: return CsSignatureValveType::AerationFilter23;
        case 24: return CsSignatureValveType::AerationFilter24;
        case 25: return CsSignatureValveType::AerationFilter25;
        case 26: return CsSignatureValveType::BackwashingFilter26;
        case 27: return CsSignatureValveType::BackwashingFilter27;

        default:
            ESP_LOGW("CsSignatureValveType", "Unknown valve type value: %u", value);
            return CsSignatureValveType::Unknown;
    }
}

inline bool CsSignatureValveTypeIsMeteredSoftener(
    CsSignatureValveType valveType,
    bool isTwinValve)
{
    if (isTwinValve)
    {
        return true;
    }

    return valveType == CsSignatureValveType::MeteredSoftener01 ||
           valveType == CsSignatureValveType::MeteredSoftener03 ||
           valveType == CsSignatureValveType::MeteredSoftener17 ||
           valveType == CsSignatureValveType::MeteredSoftener19 ||
           valveType == CsSignatureValveType::MeteredSoftener21;
}

inline bool CsSignatureValveTypeIsCommercial(
    CsSignatureValveType valveType,
    bool isTwinValve)
{
    if (isTwinValve)
    {
        return false;
    }

    return valveType == CsSignatureValveType::MeteredSoftener17 ||
           valveType == CsSignatureValveType::BackwashingFilter18 ||
           valveType == CsSignatureValveType::AerationFilter25;
}

inline bool CsSignatureValveTypeIsSdValve(CsSignatureValveType valveType, bool isTwinValve)
{
    if (isTwinValve)
    {
        return true;
    }

    return valveType == CsSignatureValveType::MeteredSoftener19 ||
           valveType == CsSignatureValveType::BackwashingFilter20 ||
           valveType == CsSignatureValveType::MeteredSoftener21 ||
           valveType == CsSignatureValveType::BackwashingFilter22 ||
           valveType == CsSignatureValveType::AerationFilter23 ||
           valveType == CsSignatureValveType::AerationFilter24;
}

inline bool CsSignatureValveTypeIsSdMeteredSoftener(
    CsSignatureValveType valveType,
    bool isTwinValve)
{
    if (isTwinValve)
    {
        return true;
    }

    return valveType == CsSignatureValveType::MeteredSoftener17 ||
           valveType == CsSignatureValveType::MeteredSoftener19 ||
           valveType == CsSignatureValveType::MeteredSoftener21;
}

inline bool CsSignatureValveTypeIsCommercialOrSdValve(CsSignatureValveType valveType, bool isTwinValve)
{
    return CsSignatureValveTypeIsCommercial(valveType, isTwinValve) ||
           CsSignatureValveTypeIsSdValve(valveType, isTwinValve);
}

inline bool CsSignatureValveTypeIsAerationValve(CsSignatureValveType valveType, bool isTwinValve)
{
    if (isTwinValve)
    {
        return false;
    }

    switch (valveType)
    {
        case CsSignatureValveType::MeteredSoftener01:
        case CsSignatureValveType::TimeclockSoftener02:
        case CsSignatureValveType::MeteredSoftener03:
        case CsSignatureValveType::BackwashingFilter04:
        case CsSignatureValveType::BackwashingFilter05:
        case CsSignatureValveType::HydroxRFilter06:
        case CsSignatureValveType::ReactRFilter07:
        case CsSignatureValveType::UltraFilter08:
        case CsSignatureValveType::MeteredSoftener17:
        case CsSignatureValveType::BackwashingFilter18:
        case CsSignatureValveType::MeteredSoftener19:
        case CsSignatureValveType::BackwashingFilter20:
        case CsSignatureValveType::MeteredSoftener21:
        case CsSignatureValveType::BackwashingFilter22:
        case CsSignatureValveType::BackwashingFilter26:
        case CsSignatureValveType::BackwashingFilter27:
            return false;

        default:
            return true;
    }
}

inline bool CsSignatureValveTypeHasAerationCycle(CsSignatureValveType valveType, bool isTwinValve)
{
    if (isTwinValve)
    {
        return true;
    }

    switch (valveType)
    {
        case CsSignatureValveType::AerationFilter09:
        case CsSignatureValveType::AerationFilter10:
        case CsSignatureValveType::AerationFilter11:
        case CsSignatureValveType::AerationFilter12:
        case CsSignatureValveType::AerationFilter13:
        case CsSignatureValveType::AerationFilter14:
        case CsSignatureValveType::AerationFilter15:
        case CsSignatureValveType::AerationFilter16:
        case CsSignatureValveType::AerationFilter25:
            return true;

        default:
            return false;
    }
}

inline bool CsSignatureValveTypeHasSaltDose(CsSignatureValveType valveType, bool isTwinValve)
{
    return isTwinValve ||
           valveType == CsSignatureValveType::MeteredSoftener17 ||
           valveType == CsSignatureValveType::MeteredSoftener19 ||
           valveType == CsSignatureValveType::MeteredSoftener21;
}

inline std::string CsSignatureValveTypeToString(CsSignatureValveType valveType)
{
    switch (valveType)
    {
        case CsSignatureValveType::MeteredSoftener01:
        case CsSignatureValveType::MeteredSoftener03:
        case CsSignatureValveType::MeteredSoftener17:
        case CsSignatureValveType::MeteredSoftener19:
        case CsSignatureValveType::MeteredSoftener21:
            return "Metered Softener";

        case CsSignatureValveType::TimeclockSoftener02:
            return "Timeclock Softener";

        case CsSignatureValveType::BackwashingFilter04:
        case CsSignatureValveType::BackwashingFilter05:
        case CsSignatureValveType::BackwashingFilter18:
        case CsSignatureValveType::BackwashingFilter20:
        case CsSignatureValveType::BackwashingFilter22:
        case CsSignatureValveType::BackwashingFilter26:
        case CsSignatureValveType::BackwashingFilter27:
            return "Backwashing Filter";

        case CsSignatureValveType::HydroxRFilter06:
            return "HydroxR Filter";

        case CsSignatureValveType::ReactRFilter07:
            return "ReactR Filter";

        case CsSignatureValveType::UltraFilter08:
            return "Ultra Filter";

        case CsSignatureValveType::AerationFilter09:
        case CsSignatureValveType::AerationFilter10:
        case CsSignatureValveType::AerationFilter11:
        case CsSignatureValveType::AerationFilter12:
        case CsSignatureValveType::AerationFilter13:
        case CsSignatureValveType::AerationFilter14:
        case CsSignatureValveType::AerationFilter15:
        case CsSignatureValveType::AerationFilter16:
        case CsSignatureValveType::AerationFilter23:
        case CsSignatureValveType::AerationFilter24:
        case CsSignatureValveType::AerationFilter25:
            return "Aeration Filter";

        default:
            return "Unknown";
    }
}

inline const char* CsSignatureValveTypeGetRegenStepString(
    CsSignatureValveType valveType,
    int32_t firmwareVersion,
    int32_t regenPosition,
    int32_t soakTimer,
    bool isTwinValve,
    bool isPreFillSoakMode
)
{
    constexpr int32_t ShutoffPosition = 9;

    // Pre-fill soak override
    if (isPreFillSoakMode && soakTimer != 0)
    {
        return CsSignatureValveRegenStepStrings::BrineSoak;
    }

    if (regenPosition == ShutoffPosition)
    {
        return CsSignatureValveRegenStepStrings::Shutoff;
    }

    // SD Metered Softener
    if (CsSignatureValveTypeIsSdMeteredSoftener(valveType, isTwinValve))
    {
        switch (regenPosition)
        {
            case 0: return isTwinValve
                        ? CsSignatureValveRegenStepStrings::Standby
                        : CsSignatureValveRegenStepStrings::Service;
            case 1: return CsSignatureValveRegenStepStrings::Backwash;
            case 2: return CsSignatureValveRegenStepStrings::BrineDraw;
            case 3: return CsSignatureValveRegenStepStrings::Backwash2;
            case 4: return CsSignatureValveRegenStepStrings::RapidRinse;
            case 5: return CsSignatureValveRegenStepStrings::BrineFill;
            default: return CsSignatureValveRegenStepStrings::Unknown;
        }
    }

    // Firmware-based decompress logic
    if (firmwareVersion >= 610 &&
        (valveType == CsSignatureValveType::AerationFilter10 ||
         valveType == CsSignatureValveType::AerationFilter12 ||
         valveType == CsSignatureValveType::AerationFilter16))
    {
        switch (regenPosition)
        {
            case 0: return CsSignatureValveRegenStepStrings::Service;
            case 1: return CsSignatureValveRegenStepStrings::Decompress;
            case 2: return CsSignatureValveRegenStepStrings::AirRelease;
            case 3: return CsSignatureValveRegenStepStrings::Backwash;
            case 4: return CsSignatureValveRegenStepStrings::Rest;
            case 5: return CsSignatureValveRegenStepStrings::AirDraw;
            case 6: return CsSignatureValveRegenStepStrings::RapidRinse;
            default: return CsSignatureValveRegenStepStrings::Unknown;
        }
    }

    switch (valveType)
    {
        // Softener types
        case CsSignatureValveType::MeteredSoftener01:
        case CsSignatureValveType::TimeclockSoftener02:
        case CsSignatureValveType::MeteredSoftener03:
            switch (regenPosition)
            {
                case 0: return CsSignatureValveRegenStepStrings::Service;
                case 1: return CsSignatureValveRegenStepStrings::Backwash;
                case 2: return CsSignatureValveRegenStepStrings::BrineDraw;
                case 3: return CsSignatureValveRegenStepStrings::RapidRinse;
                case 4: return CsSignatureValveRegenStepStrings::BrineFill;
                default: return CsSignatureValveRegenStepStrings::Unknown;
            }

        // Backwashing filters
        case CsSignatureValveType::BackwashingFilter04:
        case CsSignatureValveType::BackwashingFilter05:
        case CsSignatureValveType::HydroxRFilter06:
        case CsSignatureValveType::ReactRFilter07:
        case CsSignatureValveType::BackwashingFilter18:
        case CsSignatureValveType::BackwashingFilter20:
        case CsSignatureValveType::BackwashingFilter22:
        case CsSignatureValveType::BackwashingFilter26:
        case CsSignatureValveType::BackwashingFilter27:
            switch (regenPosition)
            {
                case 0: return CsSignatureValveRegenStepStrings::Service;
                case 1: return CsSignatureValveRegenStepStrings::Backwash;
                case 2: return CsSignatureValveRegenStepStrings::Rest;
                case 3: return CsSignatureValveRegenStepStrings::RapidRinse;
                default: return CsSignatureValveRegenStepStrings::Unknown;
            }

        // Ultra filter
        case CsSignatureValveType::UltraFilter08:
            switch (regenPosition)
            {
                case 0: return CsSignatureValveRegenStepStrings::Service;
                case 1: return CsSignatureValveRegenStepStrings::ChlorineFeed;
                case 2: return CsSignatureValveRegenStepStrings::Rest;
                case 3: return CsSignatureValveRegenStepStrings::Backwash;
                default: return CsSignatureValveRegenStepStrings::Unknown;
            }

        // Aeration filters (basic)
        case CsSignatureValveType::AerationFilter09:
        case CsSignatureValveType::AerationFilter11:
            switch (regenPosition)
            {
                case 0: return CsSignatureValveRegenStepStrings::Service;
                case 1: return CsSignatureValveRegenStepStrings::AirRelease;
                case 2: return CsSignatureValveRegenStepStrings::Backwash;
                case 3: return CsSignatureValveRegenStepStrings::Rest;
                case 4: return CsSignatureValveRegenStepStrings::RapidRinse;
                case 5: return CsSignatureValveRegenStepStrings::AirDraw;
                default: return CsSignatureValveRegenStepStrings::Unknown;
            }

        // Aeration variants (no decompress)
        case CsSignatureValveType::AerationFilter10:
        case CsSignatureValveType::AerationFilter12:
        case CsSignatureValveType::AerationFilter16:
            switch (regenPosition)
            {
                case 0: return CsSignatureValveRegenStepStrings::Service;
                case 1: return CsSignatureValveRegenStepStrings::AirRelease;
                case 2: return CsSignatureValveRegenStepStrings::Backwash;
                case 3: return CsSignatureValveRegenStepStrings::Rest;
                case 4: return CsSignatureValveRegenStepStrings::AirDraw;
                case 5: return CsSignatureValveRegenStepStrings::RapidRinse;
                default: return CsSignatureValveRegenStepStrings::Unknown;
            }

        default:
            return CsSignatureValveRegenStepStrings::Unknown;
    }
}

}
