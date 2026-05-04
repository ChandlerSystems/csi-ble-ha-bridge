#pragma once
#include "CsDeviceActiveState.h"
#include "CsDeviceAmPm.h"
#include "CsDeviceAutoReserveMode.h"
#include "CsSignatureValveAuthState.h"
#include "CsSignatureValveBrineTank.h"
#include "CsSignatureValveCyclePosition.h"
#include "CsSignatureValveCyclePositionOptionsSet.h"
#include "CsSignatureValveError.h"
#include "CsSignatureValveErrorLogEntry.h"
#include "CsSignatureValveRegenState.h"
#include "CsSignatureValveRegenTimeType.h"
#include "CsSignatureValveSeries.h"
#include "CsSignatureValveType.h"
#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace SignatureBridge::Devices::SignatureValve::SignatureCore {

using namespace SignatureBridge::Devices::Common;

struct CsSignatureValveState
{
    // Auth
    CsSignatureValveAuthState AuthState = CsSignatureValveAuthState::Unknown;
    uint32_t AuthConnectionCounter = 0;
    bool AuthApiEnabled = false;
    std::string AuthApiToken;

    // General
    uint32_t GenSerialNumberA = 0;
    uint32_t GenSerialNumberB = 0;
    uint32_t GenFirmwareVersion = 0;
    CsSignatureValveType GenDeviceType = CsSignatureValveType::Unknown;

    // Dealer Info
    std::string DiName;
    std::string DiPhone;
    std::string DiAddress1;
    std::string DiAddress2;
    std::string DiEmail;
    std::string DiWeb;

    // Device List
    bool DlRegenMotorInProgress = false;
    bool DlIsTwinValve = false;
    CsSignatureValveSeries DlValveSeries = CsSignatureValveSeries::Unknown;

    // Dashboard
    uint8_t DbHours = 0;
    uint8_t DbMinutes = 0;
    uint8_t DbSeconds = 0;
    CsDeviceAmPm DbAmPm = CsDeviceAmPm::Unknown;
    float DbBatteryLevel = 0.0f;
    uint8_t DbBatteryLevelPercent = 0;
    uint32_t DbTotalGallons = 0;
    uint32_t DbTotalGallonsRemaining = 0;
    float DbPeakFlowDaily = 0.0f;
    uint32_t DbWaterHardness = 0;
    uint32_t DbDayOverride = 0;
    float DbWaterUsedToday = 0.0f;
    float DbAvgWaterUsed = 0.0f;
    uint32_t DbCurrentDayOverride = 0;
    uint32_t DbRegenTimeHours = 0;
    uint32_t DbRegenTimeMinutes = 0;
    CsDeviceAmPm DbRegenTimeAmPm = CsDeviceAmPm::Unknown;
    uint32_t DbRegenTimeRemaining = 0;
    CsSignatureValveRegenTimeType DbRegenTimeType = CsSignatureValveRegenTimeType::Seconds;
    uint32_t DbRegenCurrentPosition = 0;
    bool DbRegenInAeration = false;
    uint32_t DbRegenSoakTimer = 0;
    bool DbPrefillEnabled = false;
    uint32_t DbPrefillDuration = 0;
    bool DbPrefillSoakMode = false;
    bool DbBrineTankIsSetup = false;
    CsSignatureValveBrineTank DbBrineTankWidth = CsSignatureValveBrineTank::Unknown;
    uint32_t DbBrineTankTotalSaltPounds = 0;
    uint32_t DbBrineTankRemainingSaltPounds = 0;

    // Settings
    uint32_t AsDaysUntilRegen = 0;
    uint32_t AsRegenDayOverride = 0;
    CsDeviceAutoReserveMode AsAutoReserveMode = CsDeviceAutoReserveMode::Manual;
    uint32_t AsReserveCapacity = 0;
    float AsReserveCapacityGallons = 0.0f;
    uint32_t AsTotalGrainsCapacity = 0;
    uint32_t AsAerationDays = 0;
    uint32_t AsChlorinePulses = 0;
    CsDeviceActiveState AsDisplayOff = CsDeviceActiveState::Inactive;
    uint32_t AsNumPositions = 0;
    std::vector<int32_t> AsPositionTimes;
    std::vector<int32_t> AsPositionMinTimes;
    std::vector<int32_t> AsPositionMaxTimes;
    std::vector<int32_t> AsPositionUnits;
    std::unordered_map<CsSignatureValveCyclePosition, CsSignatureValveCyclePositionOptionsSet> AsPositionOptions;

    // History
    uint32_t ShDaysInOperation = 0;
    uint32_t ShDaysSinceLastRegen = 0;
    float ShGallonsSinceLastRegen = 0.0f;
    uint32_t ShRegenCounter = 0;
    uint32_t ShRegenCounterResettable = 0;
    float ShGallonsTotalizer = 0.0f;
    float ShGallonsTotalizerResettable = 0.0f;
    bool ShHasErrorLogs = false;
    std::vector<CsSignatureValveErrorLogEntry> ShErrorLogs;

    // Graphs
    std::vector<float> GrPeakFlowDaily;
    std::vector<float> GrGallonsUsedDaily;
    std::vector<float> GrGallonsUsedBetweenRegens;

    // Global
    uint32_t GlBleMtuTx = 0;
    uint32_t GlBleMtuRx = 0;
    uint32_t GlValveStatus = 0;
    CsSignatureValveError GlValveError = CsSignatureValveError::NoError;
    float GlPresentFlow = 0.0f;
    bool GlLeaseModeEnabled = false;
    bool GlLeaseModeActive = false;
    bool GlRegenActive = false;
    CsSignatureValveRegenState GlRegenState = CsSignatureValveRegenState::Idle;
    bool GlBatteryReadEnabled = false;
    bool GlInitialPacketSent = false;
    bool GlDataFullySent = false;
};

}
