#pragma once

namespace SignatureBridge::Devices::SignatureValve::SignatureCore {

enum class CsSignatureValvePacketKey
{
    Unknown,

    // Auth
    AuthState,
    AuthKey,
    AuthKeyInvalidate,
    AuthApiEnabled,
    AuthApiToken,

    // General
    GenSerialNumberA,
    GenSerialNumberB,
    GenFirmwareVersion,
    GenDeviceType,

    // Device List
    DlRegenMotorInProgress,
    DlIsTwinValve,
    DlValveSeries,

    // Dashboard
    DbHours,
    DbMinutes,
    DbSeconds,
    DbAmPm,
    DbBatteryLevel,
    DbTotalGallons,
    DbTotalGallonsRemaining,
    DbPeakFlowDaily,
    DbWaterHardness,
    DbDayOverride,
    DbWaterUsedToday,
    DbAvgWaterUsed,
    DbCurrentDayOverride,
    DbRegenTimeHours,
    DbRegenTimeMinutes,
    DbRegenTimeAmPm,
    DbRegenTimeRemaining,
    DbRegenTimeType,
    DbRegenCurrentPosition,
    DbRegenInAeration,
    DbRegenSoakTimer,
    DbPrefillEnabled,
    DbPrefillDuration,
    DbPrefillSoakMode,
    DbBrineTankWidth,
    DbBrineTankTotalSaltPounds,
    DbBrineTankRemainingSaltPounds,

    // Settings
    AsDaysUntilRegen,
    AsRegenDayOverride,
    AsAutoReserveMode,
    AsReserveCapacity,
    AsReserveCapacityGallons,
    AsTotalGrainsCapacity,
    AsAerationDays,
    AsChlorinePulses,
    AsDisplayOff,
    AsNumPositions,
    AsPositionTimes,
    AsPositionMinTimes,
    AsPositionMaxTimes,
    AsPositionUnits,
    AsPositionOptions,

    // History
    ShDaysInOperation,
    ShDaysSinceLastRegen,
    ShGallonsSinceLastRegen,
    ShRegenCounter,
    ShRegenCounterResettable,
    ShGallonsTotalizer,
    ShGallonsTotalizerResettable,
    ShErrorLogs,

    // Graphs
    GrPeakFlowDaily,
    GrGallonsUsedDaily,
    GrGallonsUsedBetweenRegens,

    // Dealer Info
    DiName,
    DiPhone,
    DiAddress1,
    DiAddress2,
    DiEmail,
    DiWeb,

    // Global
    GlBleMtuTx,
    GlBleMtuRx,
    GlValveStatus,
    GlValveError,
    GlPresentFlow,
    GlRegenActive,
    GlRegenState,
    GlRegenNow,
    GlRegenNext,
    GlAerateNow,
    GlAerateNext,
    GlFindHome,
    GlInitialPacketSent,
    GlDataFullySent,

    // Deprecated / Unused
    AuthConnectionCounterLegacy,
    DbBrineTankFillHeight,
    DbBrineTankRefillTime
};

}
