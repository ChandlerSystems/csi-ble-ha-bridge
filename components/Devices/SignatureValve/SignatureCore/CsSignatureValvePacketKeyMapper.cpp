#include "CsSignatureValvePacketKeyMapper.h"

using namespace SignatureBridge::Devices::SignatureValve::SignatureCore;

CsSignatureValvePacketKey CsSignatureValvePacketKeyMapper::FromRawKey(std::string_view key)
{
    // Auth
    if (key == "as")   return CsSignatureValvePacketKey::AuthState;
    if (key == "ak")   return CsSignatureValvePacketKey::AuthKey;
    if (key == "aki")  return CsSignatureValvePacketKey::AuthKeyInvalidate;
    if (key == "ae")   return CsSignatureValvePacketKey::AuthApiEnabled;
    if (key == "at")   return CsSignatureValvePacketKey::AuthApiToken;

    // General
    if (key == "gesa") return CsSignatureValvePacketKey::GenSerialNumberA;
    if (key == "dlsa") return CsSignatureValvePacketKey::GenSerialNumberA; // legacy
    if (key == "gesb") return CsSignatureValvePacketKey::GenSerialNumberB;
    if (key == "dlsb") return CsSignatureValvePacketKey::GenSerialNumberB; // legacy
    if (key == "gef")  return CsSignatureValvePacketKey::GenFirmwareVersion;
    if (key == "dlf")  return CsSignatureValvePacketKey::GenFirmwareVersion; // legacy
    if (key == "gedt") return CsSignatureValvePacketKey::GenDeviceType;
    if (key == "dlvt") return CsSignatureValvePacketKey::GenDeviceType; // legacy

    // Device List
    if (key == "dlr")  return CsSignatureValvePacketKey::DlRegenMotorInProgress;
    if (key == "dli")  return CsSignatureValvePacketKey::DlIsTwinValve;
    if (key == "dlvs") return CsSignatureValvePacketKey::DlValveSeries;

    // Dashboard
    if (key == "dh")   return CsSignatureValvePacketKey::DbHours;
    if (key == "dm")   return CsSignatureValvePacketKey::DbMinutes;
    if (key == "ds")   return CsSignatureValvePacketKey::DbSeconds;
    if (key == "da")   return CsSignatureValvePacketKey::DbAmPm;
    if (key == "dbl")  return CsSignatureValvePacketKey::DbBatteryLevel;
    if (key == "dtg")  return CsSignatureValvePacketKey::DbTotalGallons;
    if (key == "dtgr") return CsSignatureValvePacketKey::DbTotalGallonsRemaining;
    if (key == "dpfd") return CsSignatureValvePacketKey::DbPeakFlowDaily;
    if (key == "dwh")  return CsSignatureValvePacketKey::DbWaterHardness;
    if (key == "ddo")  return CsSignatureValvePacketKey::DbDayOverride;
    if (key == "dwu")  return CsSignatureValvePacketKey::DbWaterUsedToday;
    if (key == "dwau") return CsSignatureValvePacketKey::DbAvgWaterUsed;
    if (key == "dcdo") return CsSignatureValvePacketKey::DbCurrentDayOverride;
    if (key == "drth") return CsSignatureValvePacketKey::DbRegenTimeHours;
    if (key == "drtm") return CsSignatureValvePacketKey::DbRegenTimeMinutes;
    if (key == "drta") return CsSignatureValvePacketKey::DbRegenTimeAmPm;
    if (key == "drtr") return CsSignatureValvePacketKey::DbRegenTimeRemaining;
    if (key == "drtt") return CsSignatureValvePacketKey::DbRegenTimeType;
    if (key == "drcp") return CsSignatureValvePacketKey::DbRegenCurrentPosition;
    if (key == "dria") return CsSignatureValvePacketKey::DbRegenInAeration;
    if (key == "drst") return CsSignatureValvePacketKey::DbRegenSoakTimer;
    if (key == "dpe")  return CsSignatureValvePacketKey::DbPrefillEnabled;
    if (key == "dpd")  return CsSignatureValvePacketKey::DbPrefillDuration;
    if (key == "dps")  return CsSignatureValvePacketKey::DbPrefillSoakMode;
    if (key == "dbtw") return CsSignatureValvePacketKey::DbBrineTankWidth;
    if (key == "dbts") return CsSignatureValvePacketKey::DbBrineTankTotalSaltPounds;
    if (key == "dbtr") return CsSignatureValvePacketKey::DbBrineTankRemainingSaltPounds;

    // Settings
    if (key == "asd")  return CsSignatureValvePacketKey::AsDaysUntilRegen;
    if (key == "asr")  return CsSignatureValvePacketKey::AsRegenDayOverride;
    if (key == "asar") return CsSignatureValvePacketKey::AsAutoReserveMode;
    if (key == "asrc") return CsSignatureValvePacketKey::AsReserveCapacity;
    if (key == "asrg") return CsSignatureValvePacketKey::AsReserveCapacityGallons;
    if (key == "astg") return CsSignatureValvePacketKey::AsTotalGrainsCapacity;
    if (key == "asad") return CsSignatureValvePacketKey::AsAerationDays;
    if (key == "ascp") return CsSignatureValvePacketKey::AsChlorinePulses;
    if (key == "asdo") return CsSignatureValvePacketKey::AsDisplayOff;
    if (key == "asnp") return CsSignatureValvePacketKey::AsNumPositions;
    if (key == "aspt") return CsSignatureValvePacketKey::AsPositionTimes;
    if (key == "aspn") return CsSignatureValvePacketKey::AsPositionMinTimes;
    if (key == "aspx") return CsSignatureValvePacketKey::AsPositionMaxTimes;
    if (key == "aspu") return CsSignatureValvePacketKey::AsPositionUnits;
    if (key == "aspo") return CsSignatureValvePacketKey::AsPositionOptions;

    // History
    if (key == "shdo") return CsSignatureValvePacketKey::ShDaysInOperation;
    if (key == "shdr") return CsSignatureValvePacketKey::ShDaysSinceLastRegen;
    if (key == "shgs") return CsSignatureValvePacketKey::ShGallonsSinceLastRegen;
    if (key == "shrc") return CsSignatureValvePacketKey::ShRegenCounter;
    if (key == "shrr") return CsSignatureValvePacketKey::ShRegenCounterResettable;
    if (key == "shgt") return CsSignatureValvePacketKey::ShGallonsTotalizer;
    if (key == "shgr") return CsSignatureValvePacketKey::ShGallonsTotalizerResettable;
    if (key == "shel") return CsSignatureValvePacketKey::ShErrorLogs;

    // Graphs
    if (key == "grp")  return CsSignatureValvePacketKey::GrPeakFlowDaily;
    if (key == "ggd")  return CsSignatureValvePacketKey::GrGallonsUsedDaily;
    if (key == "ggr")  return CsSignatureValvePacketKey::GrGallonsUsedBetweenRegens;

    // Dealer Info
    if (key == "din")  return CsSignatureValvePacketKey::DiName;
    if (key == "dip")  return CsSignatureValvePacketKey::DiPhone;
    if (key == "dia1") return CsSignatureValvePacketKey::DiAddress1;
    if (key == "dia2") return CsSignatureValvePacketKey::DiAddress2;
    if (key == "die")  return CsSignatureValvePacketKey::DiEmail;
    if (key == "diw")  return CsSignatureValvePacketKey::DiWeb;

    // Global
    if (key == "gmtx") return CsSignatureValvePacketKey::GlBleMtuTx;
    if (key == "gmrx") return CsSignatureValvePacketKey::GlBleMtuRx;
    if (key == "gvs")  return CsSignatureValvePacketKey::GlValveStatus;
    if (key == "gve")  return CsSignatureValvePacketKey::GlValveError;
    if (key == "gpf")  return CsSignatureValvePacketKey::GlPresentFlow;
    if (key == "gra")  return CsSignatureValvePacketKey::GlRegenActive;
    if (key == "grs")  return CsSignatureValvePacketKey::GlRegenState;
    if (key == "grn")  return CsSignatureValvePacketKey::GlRegenNow;
    if (key == "grl")  return CsSignatureValvePacketKey::GlRegenNext;
    if (key == "gan")  return CsSignatureValvePacketKey::GlAerateNow;
    if (key == "gal")  return CsSignatureValvePacketKey::GlAerateNext;
    if (key == "gfh")  return CsSignatureValvePacketKey::GlFindHome;
    if (key == "gips") return CsSignatureValvePacketKey::GlInitialPacketSent;
    if (key == "gdfs") return CsSignatureValvePacketKey::GlDataFullySent;

    // Deprecated / Unused
    if (key == "dlc") return CsSignatureValvePacketKey::AuthConnectionCounterLegacy;
    if (key == "dbth") return CsSignatureValvePacketKey::DbBrineTankFillHeight;
    if (key == "dbrt") return CsSignatureValvePacketKey::DbBrineTankRefillTime;

    return CsSignatureValvePacketKey::Unknown;
}

std::string CsSignatureValvePacketKeyMapper::ToRawKey(CsSignatureValvePacketKey key)
{
    // Auth
    if (key == CsSignatureValvePacketKey::AuthState) return std::string("as");
    if (key == CsSignatureValvePacketKey::AuthKey) return std::string("ak");
    if (key == CsSignatureValvePacketKey::AuthKeyInvalidate) return std::string("aki");
    if (key == CsSignatureValvePacketKey::AuthApiEnabled) return std::string("ae");
    if (key == CsSignatureValvePacketKey::AuthApiToken) return std::string("at");

    // General
    if (key == CsSignatureValvePacketKey::GenSerialNumberA) return std::string("gesa");
    if (key == CsSignatureValvePacketKey::GenSerialNumberB) return std::string("gesb");
    if (key == CsSignatureValvePacketKey::GenFirmwareVersion) return std::string("gef");
    if (key == CsSignatureValvePacketKey::GenDeviceType) return std::string("gedt");

    // Device List
    if (key == CsSignatureValvePacketKey::DlRegenMotorInProgress) return std::string("dlr");
    if (key == CsSignatureValvePacketKey::DlIsTwinValve) return std::string("dli");
    if (key == CsSignatureValvePacketKey::DlValveSeries) return std::string("dlvs");

    // Dashboard
    if (key == CsSignatureValvePacketKey::DbHours) return std::string("dh");
    if (key == CsSignatureValvePacketKey::DbMinutes) return std::string("dm");
    if (key == CsSignatureValvePacketKey::DbSeconds) return std::string("ds");
    if (key == CsSignatureValvePacketKey::DbAmPm) return std::string("da");
    if (key == CsSignatureValvePacketKey::DbBatteryLevel) return std::string("dbl");
    if (key == CsSignatureValvePacketKey::DbTotalGallons) return std::string("dtg");
    if (key == CsSignatureValvePacketKey::DbTotalGallonsRemaining) return std::string("dtgr");
    if (key == CsSignatureValvePacketKey::DbPeakFlowDaily) return std::string("dpfd");
    if (key == CsSignatureValvePacketKey::DbWaterHardness) return std::string("dwh");
    if (key == CsSignatureValvePacketKey::DbDayOverride) return std::string("ddo");
    if (key == CsSignatureValvePacketKey::DbWaterUsedToday) return std::string("dwu");
    if (key == CsSignatureValvePacketKey::DbAvgWaterUsed) return std::string("dwau");
    if (key == CsSignatureValvePacketKey::DbCurrentDayOverride) return std::string("dcdo");
    if (key == CsSignatureValvePacketKey::DbRegenTimeHours) return std::string("drth");
    if (key == CsSignatureValvePacketKey::DbRegenTimeMinutes) return std::string("drtm");
    if (key == CsSignatureValvePacketKey::DbRegenTimeAmPm) return std::string("drta");
    if (key == CsSignatureValvePacketKey::DbRegenTimeRemaining) return std::string("drtr");
    if (key == CsSignatureValvePacketKey::DbRegenTimeType) return std::string("drtt");
    if (key == CsSignatureValvePacketKey::DbRegenCurrentPosition) return std::string("drcp");
    if (key == CsSignatureValvePacketKey::DbRegenInAeration) return std::string("dria");
    if (key == CsSignatureValvePacketKey::DbRegenSoakTimer) return std::string("drst");
    if (key == CsSignatureValvePacketKey::DbPrefillEnabled) return std::string("dpe");
    if (key == CsSignatureValvePacketKey::DbPrefillDuration) return std::string("dpd");
    if (key == CsSignatureValvePacketKey::DbPrefillSoakMode) return std::string("dps");
    if (key == CsSignatureValvePacketKey::DbBrineTankWidth) return std::string("dbtw");
    if (key == CsSignatureValvePacketKey::DbBrineTankTotalSaltPounds) return std::string("dbts");
    if (key == CsSignatureValvePacketKey::DbBrineTankRemainingSaltPounds) return std::string("dbtr");

    // Settings
    if (key == CsSignatureValvePacketKey::AsDaysUntilRegen) return std::string("asd");
    if (key == CsSignatureValvePacketKey::AsRegenDayOverride) return std::string("asr");
    if (key == CsSignatureValvePacketKey::AsAutoReserveMode) return std::string("asar");
    if (key == CsSignatureValvePacketKey::AsReserveCapacity) return std::string("asrc");
    if (key == CsSignatureValvePacketKey::AsReserveCapacityGallons) return std::string("asrg");
    if (key == CsSignatureValvePacketKey::AsTotalGrainsCapacity) return std::string("astg");
    if (key == CsSignatureValvePacketKey::AsAerationDays) return std::string("asad");
    if (key == CsSignatureValvePacketKey::AsChlorinePulses) return std::string("ascp");
    if (key == CsSignatureValvePacketKey::AsDisplayOff) return std::string("asdo");
    if (key == CsSignatureValvePacketKey::AsNumPositions) return std::string("asnp");
    if (key == CsSignatureValvePacketKey::AsPositionTimes) return std::string("aspt");
    if (key == CsSignatureValvePacketKey::AsPositionMinTimes) return std::string("aspn");
    if (key == CsSignatureValvePacketKey::AsPositionMaxTimes) return std::string("aspx");
    if (key == CsSignatureValvePacketKey::AsPositionUnits) return std::string("aspu");
    if (key == CsSignatureValvePacketKey::AsPositionOptions) return std::string("aspo");

    // History
    if (key == CsSignatureValvePacketKey::ShDaysInOperation) return std::string("shdo");
    if (key == CsSignatureValvePacketKey::ShDaysSinceLastRegen) return std::string("shdr");
    if (key == CsSignatureValvePacketKey::ShGallonsSinceLastRegen) return std::string("shgs");
    if (key == CsSignatureValvePacketKey::ShRegenCounter) return std::string("shrc");
    if (key == CsSignatureValvePacketKey::ShRegenCounterResettable) return std::string("shrr");
    if (key == CsSignatureValvePacketKey::ShGallonsTotalizer) return std::string("shgt");
    if (key == CsSignatureValvePacketKey::ShGallonsTotalizerResettable) return std::string("shgr");
    if (key == CsSignatureValvePacketKey::ShErrorLogs) return std::string("shel");

    // Graphs
    if (key == CsSignatureValvePacketKey::GrPeakFlowDaily) return std::string("grp");
    if (key == CsSignatureValvePacketKey::GrGallonsUsedDaily) return std::string("ggd");
    if (key == CsSignatureValvePacketKey::GrGallonsUsedBetweenRegens) return std::string("ggr");

    // Dealer Info
    if (key == CsSignatureValvePacketKey::DiName) return std::string("din");
    if (key == CsSignatureValvePacketKey::DiPhone) return std::string("dip");
    if (key == CsSignatureValvePacketKey::DiAddress1) return std::string("dia1");
    if (key == CsSignatureValvePacketKey::DiAddress2) return std::string("dia2");
    if (key == CsSignatureValvePacketKey::DiEmail) return std::string("die");
    if (key == CsSignatureValvePacketKey::DiWeb) return std::string("diw");

    // Global
    if (key == CsSignatureValvePacketKey::GlBleMtuTx) return std::string("gmtx");
    if (key == CsSignatureValvePacketKey::GlBleMtuRx) return std::string("gmrx");
    if (key == CsSignatureValvePacketKey::GlValveStatus) return std::string("gvs");
    if (key == CsSignatureValvePacketKey::GlValveError) return std::string("gve");
    if (key == CsSignatureValvePacketKey::GlPresentFlow) return std::string("gpf");
    if (key == CsSignatureValvePacketKey::GlRegenActive) return std::string("gra");
    if (key == CsSignatureValvePacketKey::GlRegenState) return std::string("grs");
    if (key == CsSignatureValvePacketKey::GlRegenNow) return std::string("grn");
    if (key == CsSignatureValvePacketKey::GlRegenNext) return std::string("grl");
    if (key == CsSignatureValvePacketKey::GlAerateNow) return std::string("gan");
    if (key == CsSignatureValvePacketKey::GlAerateNext) return std::string("gal");
    if (key == CsSignatureValvePacketKey::GlFindHome) return std::string("gfh");
    if (key == CsSignatureValvePacketKey::GlInitialPacketSent) return std::string("gips");
    if (key == CsSignatureValvePacketKey::GlDataFullySent) return std::string("gdfs");

    // Deprecated / Unused
    if (key == CsSignatureValvePacketKey::AuthConnectionCounterLegacy) return std::string("dlc");
    if (key == CsSignatureValvePacketKey::DbBrineTankFillHeight) return std::string("dbth");
    if (key == CsSignatureValvePacketKey::DbBrineTankRefillTime) return std::string("dbrt");

    return std::string("Unknown");
}
