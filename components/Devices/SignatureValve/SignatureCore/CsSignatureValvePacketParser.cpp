#include "CsDeviceActiveState.h"
#include "CsDeviceAmPm.h"
#include "CsDeviceAutoReserveMode.h"
#include "CsSignatureValveAuthState.h"
#include "CsSignatureValveBrineTank.h"
#include "CsSignatureValveCyclePosition.h"
#include "CsSignatureValveCyclePositionOptionsSet.h"
#include "CsSignatureValveError.h"
#include "CsSignatureValveErrorLogEntry.h"
#include "CsSignatureValvePacketParser.h"
#include "CsSignatureValvePacketKeyMapper.h"
#include "CsSignatureValveRegenState.h"
#include "CsSignatureValveRegenTimeType.h"
#include "CsSignatureValveSeries.h"
#include "CsSignatureValveType.h"
#include "CsCoreUtils.h"
#include <algorithm>
#include <cmath>
#include <esp_log.h>

using namespace SignatureBridge::Core;
using namespace SignatureBridge::Devices::SignatureValve::SignatureCore;

bool CsSignatureValvePacketParser::ApplyChanges(
    CsSignatureValveState& state,
    const std::vector<CsBlePacketChange>& changes
) const
{
    if (changes.empty())
    {
        ESP_LOGW(CsSignatureValvePacketParser::Tag(), "[ApplyChanges] No changes to apply.");
        return false;
    }

    bool changed = false;

    for (const auto& change : changes)
    {
        if (ApplyChange(state, change))
        {
            changed = true;
        }
        else
        {
            ESP_LOGW(
                CsSignatureValvePacketParser::Tag(),
                "Unhandled packet key [Key: %s] [Type: %s]",
                change.Key.c_str(),
                CsBlePacketValue::ToString(change.Value.Type).c_str()
            );
        }
    }

    return changed;
}

bool CsSignatureValvePacketParser::ApplyChange(
    CsSignatureValveState& state,
    const CsBlePacketChange& change
) const
{
    const auto key = CsSignatureValvePacketKeyMapper::FromRawKey(change.Key);

    if (key == CsSignatureValvePacketKey::Unknown)
    {
        ESP_LOGW(CsSignatureValvePacketParser::Tag(), "Unhandled packet key: %s", change.Key.c_str());
        return false;
    }

    if (change.Value.IsInt())
    {
        return ApplyScalarChange(state, key, change.Value);
    }

    if (change.Value.IsArray() || change.Value.IsByteArray())
    {
        return ApplyArrayChange(state, key, change.Value);
    }

    ESP_LOGW(CsSignatureValvePacketParser::Tag(), "Unsupported packet value type for key: %s", change.Key.c_str());
    return false;
}

bool CsSignatureValvePacketParser::ApplyScalarChange(
    CsSignatureValveState& state,
    CsSignatureValvePacketKey key,
    const CsBlePacketValue& value
) const
{
    if (!value.IsInt())
    {
        return false;
    }

    const auto raw = value.IntValue;

    switch (key)
    {
        // Auth
        case CsSignatureValvePacketKey::AuthState:
            state.AuthState = CsSignatureValveAuthStateTryFromInt(raw);
            return true;

        case CsSignatureValvePacketKey::AuthApiEnabled:
            state.AuthApiEnabled = CsCoreUtils::Instance().ValueToBool(raw);
            return true;

        // General
        case CsSignatureValvePacketKey::GenSerialNumberA:
            state.GenSerialNumberA = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::GenSerialNumberB:
            state.GenSerialNumberB = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::GenFirmwareVersion:
            state.GenFirmwareVersion = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::GenDeviceType:
            state.GenDeviceType = CsSignatureValveTypeTryFromInt(raw);
            return true;

        // Device List
        case CsSignatureValvePacketKey::DlRegenMotorInProgress:
            state.DlRegenMotorInProgress = CsCoreUtils::Instance().ValueToBool(raw);
            return true;

        case CsSignatureValvePacketKey::DlIsTwinValve:
            state.DlIsTwinValve = CsCoreUtils::Instance().ValueToBool(raw);
            return true;

        case CsSignatureValvePacketKey::DlValveSeries:
            state.DlValveSeries = CsSignatureValveSeriesTryFromInt(raw);
            return true;

        // Dashboard
        case CsSignatureValvePacketKey::DbHours:
            state.DbHours = static_cast<uint8_t>(raw);
            return true;

        case CsSignatureValvePacketKey::DbMinutes:
            state.DbMinutes = static_cast<uint8_t>(raw);
            return true;

        case CsSignatureValvePacketKey::DbSeconds:
            state.DbSeconds = static_cast<uint8_t>(raw);
            return true;

        case CsSignatureValvePacketKey::DbAmPm:
            state.DbAmPm = CsDeviceAmPmTryFromInt(raw);
            return true;

        case CsSignatureValvePacketKey::DbBatteryLevel:
        {
            state.DbBatteryLevel = static_cast<float>(raw) / 1000.0f;
            state.DbBatteryLevelPercent = GetBatteryCapacity(state.DbBatteryLevel);
            return true;
        }

        case CsSignatureValvePacketKey::DbTotalGallons:
            state.DbTotalGallons = static_cast<uint32_t>(
                static_cast<float>(raw) / 100.0f
            );
            return true;

        case CsSignatureValvePacketKey::DbTotalGallonsRemaining:
            state.DbTotalGallonsRemaining = static_cast<uint32_t>(
                static_cast<float>(raw) / 100.0f
            );
            return true;

        case CsSignatureValvePacketKey::DbPeakFlowDaily:
            state.DbPeakFlowDaily = static_cast<float>(raw) / 100.0f;
            return true;

        case CsSignatureValvePacketKey::DbWaterHardness:
            state.DbWaterHardness = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::DbDayOverride:
            state.DbDayOverride = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::DbWaterUsedToday:
            state.DbWaterUsedToday = static_cast<float>(raw) / 100.0f;
            return true;

        case CsSignatureValvePacketKey::DbAvgWaterUsed:
            state.DbAvgWaterUsed = static_cast<float>(raw) / 100.0f;
            return true;

        case CsSignatureValvePacketKey::DbCurrentDayOverride:
            state.DbCurrentDayOverride = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::DbRegenTimeHours:
            state.DbRegenTimeHours = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::DbRegenTimeMinutes:
            state.DbRegenTimeMinutes = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::DbRegenTimeAmPm:
            state.DbRegenTimeAmPm = CsDeviceAmPmTryFromInt(raw);
            return true;

        case CsSignatureValvePacketKey::DbRegenTimeRemaining:
            state.DbRegenTimeRemaining = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::DbRegenTimeType:
            state.DbRegenTimeType = CsSignatureValveRegenTimeTypeTryFromInt(raw);
            return true;

        case CsSignatureValvePacketKey::DbRegenCurrentPosition:
            state.DbRegenCurrentPosition = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::DbRegenInAeration:
            state.DbRegenInAeration = CsCoreUtils::Instance().ValueToBool(raw);
            return true;

        case CsSignatureValvePacketKey::DbRegenSoakTimer:
            state.DbRegenSoakTimer = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::DbPrefillEnabled:
            state.DbPrefillEnabled = CsCoreUtils::Instance().ValueToBool(raw);
            return true;

        case CsSignatureValvePacketKey::DbPrefillDuration:
            state.DbPrefillDuration = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::DbPrefillSoakMode:
            state.DbPrefillSoakMode = CsCoreUtils::Instance().ValueToBool(raw);
            return true;

        case CsSignatureValvePacketKey::DbBrineTankWidth:
            state.DbBrineTankWidth = CsSignatureValveBrineTankTryFromInt(raw);
            state.DbBrineTankIsSetup = (state.DbBrineTankWidth != CsSignatureValveBrineTank::Unknown);
            return true;

        case CsSignatureValvePacketKey::DbBrineTankTotalSaltPounds:
            state.DbBrineTankTotalSaltPounds = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::DbBrineTankRemainingSaltPounds:
            state.DbBrineTankRemainingSaltPounds = static_cast<uint32_t>(
                state.GenFirmwareVersion >= 611 ? std::lround(static_cast<float>(raw) / 10.0f) : raw
            );
            return true;

        // Settings
        case CsSignatureValvePacketKey::AsDaysUntilRegen:
            state.AsDaysUntilRegen = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::AsRegenDayOverride:
            state.AsRegenDayOverride = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::AsAutoReserveMode:
            state.AsAutoReserveMode = CsDeviceAutoReserveModeTryFromInt(raw);
            return true;

        case CsSignatureValvePacketKey::AsReserveCapacity:
            state.AsReserveCapacity = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::AsReserveCapacityGallons:
            state.AsReserveCapacityGallons = static_cast<float>(raw) / 100.0f;
            return true;

        case CsSignatureValvePacketKey::AsTotalGrainsCapacity:
            state.AsTotalGrainsCapacity = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::AsAerationDays:
            state.AsAerationDays = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::AsChlorinePulses:
            state.AsChlorinePulses = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::AsDisplayOff:
            state.AsDisplayOff = CsDeviceActiveStateTryFromInt(raw);
            return true;

        case CsSignatureValvePacketKey::AsNumPositions:
            state.AsNumPositions = static_cast<uint32_t>(raw);
            return true;

        // History
        case CsSignatureValvePacketKey::ShDaysInOperation:
            state.ShDaysInOperation = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::ShDaysSinceLastRegen:
            state.ShDaysSinceLastRegen = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::ShGallonsSinceLastRegen:
            state.ShGallonsSinceLastRegen = static_cast<float>(raw) / 100.0f;
            return true;

        case CsSignatureValvePacketKey::ShRegenCounter:
            state.ShRegenCounter = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::ShRegenCounterResettable:
            state.ShRegenCounterResettable = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::ShGallonsTotalizer:
            state.ShGallonsTotalizer = static_cast<float>(raw) / 100.0f;
            return true;

        case CsSignatureValvePacketKey::ShGallonsTotalizerResettable:
            state.ShGallonsTotalizerResettable = static_cast<float>(raw) / 100.0f;
            return true;

        // Global
        case CsSignatureValvePacketKey::GlBleMtuTx:
            state.GlBleMtuTx = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::GlBleMtuRx:
            state.GlBleMtuRx = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::GlValveStatus:
            state.GlValveStatus = static_cast<uint32_t>(raw);
            return true;

        case CsSignatureValvePacketKey::GlValveError:
            state.GlValveError = CsSignatureValveErrorTryFromInt(raw);
            return true;

        case CsSignatureValvePacketKey::GlPresentFlow:
            state.GlPresentFlow = static_cast<float>(raw) / 100.0f;
            return true;

        case CsSignatureValvePacketKey::GlRegenActive:
            state.GlRegenActive = CsCoreUtils::Instance().ValueToBool(raw);
            return true;

        case CsSignatureValvePacketKey::GlRegenState:
            state.GlRegenState = CsSignatureValveRegenStateTryFromInt(raw);
            return true;

        case CsSignatureValvePacketKey::GlInitialPacketSent:
            state.GlInitialPacketSent = CsCoreUtils::Instance().ValueToBool(raw);
            return true;

        case CsSignatureValvePacketKey::GlDataFullySent:
            state.GlDataFullySent = CsCoreUtils::Instance().ValueToBool(raw);
            return true;

        // Deprecated / Unused
        case CsSignatureValvePacketKey::AuthConnectionCounterLegacy:
        case CsSignatureValvePacketKey::DbBrineTankFillHeight:
        case CsSignatureValvePacketKey::DbBrineTankRefillTime:
            return true;

        default:
            ESP_LOGW(CsSignatureValvePacketParser::Tag(), "Unhandled scalar packet key");
            return false;
    }
}

bool CsSignatureValvePacketParser::ApplyArrayChange(
    CsSignatureValveState& state,
    CsSignatureValvePacketKey key,
    const CsBlePacketValue& value
) const
{
    switch (key)
    {
        case CsSignatureValvePacketKey::DiName:
            return TryApplyDealerInfoString(value, state.DiName);

        case CsSignatureValvePacketKey::DiPhone:
            return TryApplyDealerInfoString(value, state.DiPhone);

        case CsSignatureValvePacketKey::DiAddress1:
            return TryApplyDealerInfoString(value, state.DiAddress1);

        case CsSignatureValvePacketKey::DiAddress2:
            return TryApplyDealerInfoString(value, state.DiAddress2);

        case CsSignatureValvePacketKey::DiEmail:
            return TryApplyDealerInfoString(value, state.DiEmail);

        case CsSignatureValvePacketKey::DiWeb:
        {
            if (!TryApplyDealerInfoString(value, state.DiWeb))
            {
                return false;
            }

            state.GlDataFullySent = true;
            return true;
        }

        case CsSignatureValvePacketKey::AuthApiToken:
        {
            if (!value.IsArray())
            {
                return false;
            }

            std::vector<uint8_t> bytes;

            if (!CsCoreUtils::Instance().TryPacketValueArrayToBytes(
                value.ArrayValue,
                bytes,
                true
            ))
            {
                return false;
            }

            const auto token = CsCoreUtils::Instance().BytesToHexString(bytes, false);

            if (token.empty())
            {
                return false;
            }

            state.AuthApiToken = token;
            return true;
        }

        case CsSignatureValvePacketKey::AsPositionTimes:
            if (!value.IsArrayOfInt()) return false;
            state.AsPositionTimes = value.ToIntArray();
            return true;

        case CsSignatureValvePacketKey::AsPositionMinTimes:
            if (!value.IsArrayOfInt()) return false;
            state.AsPositionMinTimes = value.ToIntArray();
            return true;

        case CsSignatureValvePacketKey::AsPositionMaxTimes:
            if (!value.IsArrayOfInt()) return false;
            state.AsPositionMaxTimes = value.ToIntArray();
            return true;

        case CsSignatureValvePacketKey::AsPositionUnits:
            if (!value.IsArrayOfInt()) return false;
            state.AsPositionUnits = value.ToIntArray();
            return true;

        case CsSignatureValvePacketKey::AsPositionOptions:
        {
            if (!value.IsArrayOfInt()) return false;

            state.AsPositionOptions.clear();
            const auto values = value.ToIntArray();

            for (size_t i = 1U; i < values.size() && i <= 8U; ++i)
            {
                CsSignatureValveCyclePositionOptionsSet options;
                options.SetRaw(static_cast<uint16_t>(values[i]));

                state.AsPositionOptions[
                    static_cast<CsSignatureValveCyclePosition>(i)
                ] = options;
            }

            return true;
        }

        case CsSignatureValvePacketKey::GrPeakFlowDaily:
            if (!value.IsArrayOfInt()) return false;
            state.GrPeakFlowDaily = ConvertPeakFlowGraph(value.ToIntArray());
            return true;

        case CsSignatureValvePacketKey::GrGallonsUsedDaily:
            if (!value.IsArrayOfInt()) return false;
            state.GrGallonsUsedDaily = ConvertGallonsGraph(state, value.ToIntArray());
            return true;

        case CsSignatureValvePacketKey::GrGallonsUsedBetweenRegens:
            if (!value.IsArrayOfInt()) return false;
            state.GrGallonsUsedBetweenRegens = ConvertGallonsGraph(state, value.ToIntArray());
            return true;

        case CsSignatureValvePacketKey::ShErrorLogs:
        {
            if (!value.IsArray()) return false;

            state.ShErrorLogs.clear();
            bool hasValidEntry = false;

            for (const auto& item : value.ArrayValue)
            {
                if (!item.IsObject()) continue;

                CsSignatureValveErrorLogEntry entry;

                for (const auto& field : item.ObjectValue)
                {
                    if (!field.second.IsInt()) continue;

                    if (field.first == "d") entry.Day = field.second.IntValue;
                    else if (field.first == "h") entry.Hour = field.second.IntValue;
                    else if (field.first == "m") entry.Minute = field.second.IntValue;
                    else if (field.first == "s") entry.Second = field.second.IntValue;
                    else if (field.first == "e") entry.Error = CsSignatureValveErrorTryFromInt(field.second.IntValue);
                }

                if (entry.HasValidData())
                {
                    hasValidEntry = true;
                }

                state.ShErrorLogs.push_back(entry);
            }

            state.ShHasErrorLogs = hasValidEntry;
            return true;
        }

        default:
            ESP_LOGW(CsSignatureValvePacketParser::Tag(), "Unhandled array packet key");
            return false;
    }
}

uint8_t CsSignatureValvePacketParser::GetBatteryCapacity(float volts)
{
    if (volts >= 9.50f) return 100;
    if (volts >= 8.91f) return static_cast<uint8_t>(100.0f - (9.5f - volts) * 8.78f);
    if (volts >= 8.48f) return static_cast<uint8_t>(94.78f - (8.91f - volts) * 30.26f);
    if (volts >= 7.43f) return static_cast<uint8_t>(81.84f - (8.48f - volts) * 60.47f);
    if (volts >= 6.50f) return static_cast<uint8_t>(18.68f - (7.43f - volts) * 20.02f);
    return 0;
}

std::vector<float> CsSignatureValvePacketParser::ConvertPeakFlowGraph(const std::vector<int32_t>& values)
{
    std::vector<float> result;
    result.reserve(values.size());

    for (const auto value : values)
    {
        result.push_back(static_cast<float>(value) / 100.0f);
    }

    return result;
}

std::vector<float> CsSignatureValvePacketParser::ConvertGallonsGraph(
    const CsSignatureValveState& state,
    const std::vector<int32_t>& values
)
{
    std::vector<float> result;
    result.reserve(values.size());

    for (const auto value : values)
    {
        result.push_back(static_cast<float>(value) / 100.0f);
    }

    return result;
}

bool CsSignatureValvePacketParser::TryApplyDealerInfoString(const CsBlePacketValue& value, std::string& target)
{
    if (!value.IsArray())
    {
        return false;
    }

    std::vector<uint8_t> bytes;

    if (!CsCoreUtils::Instance().TryPacketValueArrayToBytes(value.ArrayValue, bytes, true))
    {
        return false;
    }

    target = CsCoreUtils::Instance().Utf8BytesToString(bytes);
    return true;
}
