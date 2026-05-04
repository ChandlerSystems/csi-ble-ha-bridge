#pragma once
#include "CsBleJsonWriteBuilder.h"
#include "CsBleJsonWriteChange.h"
#include "CsDeviceAutoReserveMode.h"
#include "CsDeviceActiveState.h"
#include "CsSignatureValvePacketKeyMapper.h"
#include "CsSignatureValveCyclePosition.h"
#include "CsSignatureValveCyclePositionOptionsSet.h"
#include "CsSignatureValveState.h"

namespace SignatureBridge::Devices::SignatureValve::SignatureCore {

using namespace SignatureBridge::BluetoothCore::BleCore;
using namespace SignatureBridge::Devices::Common;

class CsSignatureValveWriteBuilder
{
public: // Auth Packets
    /**
     * Builds a CsBleJsonWriteCommand to invalidate the authentication key, which can be used to force the device to
     * generate a new key and re-authenticate. This is useful for security purposes, such as when the key may have been
     * compromised or needs to be refreshed.
     *
     * @return The constructed command to invalidate the authentication key.
     */
    static std::optional<CsBleJsonWriteCommand> AuthInvalidateKey()
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::BoolValue(
                Key(CsSignatureValvePacketKey::AuthKeyInvalidate),
                true
            )
        );
    }

    /**
     * Builds a CsBleJsonWriteCommand to enable the API functionality on the device. This generates an API token that
     * can be used for authenticated communication with the device. Enabling the API allows external applications to
     * interact with the device, retrieve data, and perform actions programmatically.
     *
     * @return The constructed command to enable the API functionality.
     */
    static std::optional<CsBleJsonWriteCommand> AuthEnableApi()
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::BoolValue(
                Key(CsSignatureValvePacketKey::AuthApiEnabled),
                true
            )
        );
    }

public: // Dashboard Packets
    /**
     * Builds a CsBleJsonWriteCommand to set the current time on the device, including hours, minutes, and seconds. This
     * is important for ensuring that the device's internal clock is accurate, which can affect time-based features such
     * as days in operation, time until next regeneration, etc.
     *
     * @param[in] hours The current hour to set on the device (0-23).
     * @param[in] minutes The current minute to set on the device (0-59).
     * @param[in] seconds The current second to set on the device (0-59).
     *
     * @return The constructed command to set the device's time.
     */
    static std::optional<CsBleJsonWriteCommand> SetTime(int hours, int minutes, int seconds)
    {
        return CsBleJsonWriteBuilder::BuildMany(
            {
                CsBleJsonWriteChange::IntValue(
                    Key(CsSignatureValvePacketKey::DbHours),
                    hours, 0, 23
                ),
                CsBleJsonWriteChange::IntValue(
                    Key(CsSignatureValvePacketKey::DbMinutes),
                    minutes, 0, 59
                ),
                CsBleJsonWriteChange::IntValue(
                    Key(CsSignatureValvePacketKey::DbSeconds),
                    seconds, 0, 59
                )
             }
        );
    }

    /**
     * Builds a CsBleJsonWriteCommand to set the water hardness level on the device. Water hardness is a measurement
     * (in Grains per Gallon) of dissolved minerals in water. This setting should match the actual hardness measurement
     * from a water test. Changing this setting can affect efficiencies and the softener's ability to make soft water.
     *
     * @param[in] value The water hardness level to set on the device (0-99 gpg).
     *
     * @return The constructed command to set the water hardness level.
     */
    static std::optional<CsBleJsonWriteCommand> SetWaterHardness(int value)
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::IntValue(
                Key(CsSignatureValvePacketKey::DbWaterHardness),
                value, 0, 99
            )
        );
    }

    /**
     * Builds a CsBleJsonWriteCommand to set the day override value on the device. The Regeneration Day Override sets
     * the maximum number of days between regenerations. If a regeneration does not occur because of the gallons used
     * within the set number of days, then regeneration will be forced by the Regeneration Day Override.
     *
     * @param[in] value The day override value to set on the device (0-29 days).
     *
     * @return The constructed command to set the day override value.
     */
    static std::optional<CsBleJsonWriteCommand> SetDayOverride(int value)
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::IntValue(
                Key(CsSignatureValvePacketKey::DbDayOverride),
                value, 0, 29
            )
        );
    }

    /**
     * Builds a CsBleJsonWriteCommand to set the regeneration time on the device, including hours and minutes. The
     * regeneration time is the time of day that your unit will regenerate (or backwash). If water is used in the house
     * during the regeneration process the unit will bypass untreated water. For that reason, the regeneration time
     * should be set to a time of day that water use is least likely.
     *
     * @param[in] hours The hour to set for the regeneration time (0-23).
     * @param[in] minutes The minute to set for the regeneration time (0-59).
     *
     * @return The constructed command to set the regeneration time.
     */
    static std::optional<CsBleJsonWriteCommand> SetRegenTime(int hours, int minutes)
    {
        return CsBleJsonWriteBuilder::BuildMany(
            {
                CsBleJsonWriteChange::IntValue(
                    Key(CsSignatureValvePacketKey::DbRegenTimeHours),
                    hours, 0, 23
                ),
                CsBleJsonWriteChange::IntValue(
                    Key(CsSignatureValvePacketKey::DbRegenTimeMinutes),
                    minutes, 0, 59
                )
             }
        );
    }

    /**
     * Builds a CsBleJsonWriteCommand to set whether brine pre-fill is enabled on the device. Enabling pre-fill will
     * fill the brine tank prior to the scheduled Regeneration time to provide time for the tank to make Brine. In some
     * installations, the pre-fill option reduces salt bridging and makes each regeneration cycle more consistent.
     *
     * @param[in] value true to enable brine pre-fill, false to disable.
     *
     * @return The constructed command to set the brine pre-fill enabled state.
     */
    static std::optional<CsBleJsonWriteCommand> SetPrefillEnabled(bool value)
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::BoolValue(
                Key(CsSignatureValvePacketKey::DbPrefillEnabled),
                value
            )
        );
    }

    /**
     * Builds a CsBleJsonWriteCommand to set the brine pre-fill duration on the device. The pre-fill duration sets how
     * long, in hours, the water in the tank will soak before the scheduled regeneration time.
     *
     * @param[in] value The duration to set for the brine pre-fill (1-4 hours).
     *
     * @return The constructed command to set the brine pre-fill duration.
     */
    static std::optional<CsBleJsonWriteCommand> SetPrefillDuration(int value)
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::IntValue(
                Key(CsSignatureValvePacketKey::DbPrefillDuration),
                value, 1, 4
            )
        );
    }

    /**
     * Builds a CsBleJsonWriteCommand to set the remaining salt pounds in the brine tank on the device. This is used to
     * manually adjust the remaining salt pounds value. This is required as the brine tank doesn't have a way to measure
     * the salt level, so it relies on tracking how much salt is used during regenerations and when the tank is
     * refilled.
     *
     * @note The maximum value for remaining salt pounds is 1500 lbs., but it's in tenths of pounds, so the maximum
     * value to set is 15000 (1500.0 lbs).
     *
     * @param[in] value The remaining salt pounds to set in the brine tank (0-15000).
     *
     * @return The constructed command to set the remaining salt pounds in the brine tank.
     */
    static std::optional<CsBleJsonWriteCommand> SetBrineTankRemainingSaltPounds(int value)
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::IntValue(
                Key(CsSignatureValvePacketKey::DbBrineTankRemainingSaltPounds),
                value, 0, 15000
            )
        );
    }

public: // Settings Packets
    /**
     * Builds a CsBleJsonWriteCommand to set the regeneration day override value on the device. The Regeneration Day
     * Override sets the maximum number of days between regenerations. If a regeneration does not occur because of the
     * gallons used within the set number of days, then regeneration will be forced by the Regeneration Day Override.
     *
     * @param[in] value The day override value to set on the device (0-29 days).
     *
     * @return The constructed command to set the regeneration day override value.
     */
    static std::optional<CsBleJsonWriteCommand> SetRegenDayOverride(int value)
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::IntValue(
                Key(CsSignatureValvePacketKey::AsRegenDayOverride),
                value, 0, 29
            )
        );
    }

    /**
     * Builds a CsBleJsonWriteCommand to set the auto reserve mode on the device. The Reserve Capacity determines when
     * your softener regenerates, which renews its capacity to soften water. Because it is best to regenerate at a time
     * when water is not being used (so that hard water does not bypass your softener and enter your service water
     * pipes), the reserve capacity should be set to the amount of water that might get used in 1 day. The automatic
     * reserve capacity (ARC) is set to automatic mode by default.
     *
     * When the ARC calculation is started on a new unit, it will use the conservative amount of 25% of the set
     * capacity for the reserve amount, until there are 7 days of water usage data available to begin doing calculations
     * for the ARC. Once there are 7 days of data available, the reserve capacity will automatically adjust based on
     * usage to achieve the most efficient use of the softener’s capacity while making sure that it is unlikely to run
     * out of softening capacity before regeneration occurs. The app will display the calculated reserve capacity in
     * gallons of water and percentage.
     *
     * If the ARC is turned to manual mode, the user can input the reserve capacity in percentage, and the app will
     * show the calculated gallons based on that percentage.
     *
     * @param[in] value The auto reserve mode to set on the device (Manual or Automatic).
     *
     * @return The constructed command to set the auto reserve mode.
     */
    static std::optional<CsBleJsonWriteCommand> SetAutoReserveMode(CsDeviceAutoReserveMode value)
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::IntValue(
                Key(CsSignatureValvePacketKey::AsAutoReserveMode),
                static_cast<int>(value),
                static_cast<int>(CsDeviceAutoReserveMode::Manual),
                static_cast<int>(CsDeviceAutoReserveMode::Automatic)
            )
        );
    }

    /**
     * Builds a CsBleJsonWriteCommand to set the reserve capacity on the device. When using a metered softener, the
     * meter counts the gallons of water used to determine when the resin capacity is depleted. However, because the
     * softener should only regenerate at a certain time of the day, the reserve capacity is used to allow the softener
     * to continue to treat water until the regeneration time occurs. The reserve capacity can be adjusted lower to use
     * the salt more efficiently, or higher to ensure there will not be hard water breakthrough.
     *
     * @param[in] value The reserve capacity to set on the device (0-49%).
     *
     * @return The constructed command to set the reserve capacity.
     */
    static std::optional<CsBleJsonWriteCommand> SetReserveCapacity(int value)
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::IntValue(
                Key(CsSignatureValvePacketKey::AsReserveCapacity),
                value, 0, 49
            )
        );
    }

    /**
     * Builds a CsBleJsonWriteCommand to set the total resin grains capacity on the device. This setting is set to the
     * physical capacity in treatable grains of hardness that your softener can treat before needing to regenerate.
     * This setting should only be adjusted by your professional water dealer.
     *
     * @param value The total resin grains capacity to set on the device (0-399).
     *
     * @return The constructed command to set the total resin grains capacity.
     */
    static std::optional<CsBleJsonWriteCommand> SetTotalResinGrainsCapacity(int value)
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::IntValue(
                Key(CsSignatureValvePacketKey::AsTotalGrainsCapacity),
                value, 0, 399
            )
        );
    }

    /**
     * Builds a CsBleJsonWriteCommand to set the air recharge frequency on the device. The Air Recharge Frequency sets
     * the number of days between air recharge cycles. These cycles refill the filter tank with air, which is used to
     * oxidize iron and other contaminates in order to filter them out of the water. This setting should always be set
     * to 1 day, unless advised otherwise by your professional water dealer.
     *
     * @param[in] value The air recharge frequency to set on the device (0-9 days).
     *
     * @return The constructed command to set the air recharge frequency.
     */
    static std::optional<CsBleJsonWriteCommand> SetAirRechargeFrequency(int value)
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::IntValue(
                Key(CsSignatureValvePacketKey::AsAerationDays),
                value, 0, 9
            )
        );
    }

    /**
     * Builds a CsBleJsonWriteCommand to set the number of chlorine pulses on the device. The Chemical Draw setting
     * determines how many times the oxidizing chemical is injected into the tank during regeneration. This requires
     * that the filter has the Oxyclean option. Setting it to 0 will disable this option. Turning this setting on will
     * inject approximately 8 - 16oz (1 sets it to 8oz., 4 sets it to 16oz.) of the oxidizing chemical solution into
     * the filter to disinfect the media bed and improve filtration.
     *
     * @param[in] value The number of chlorine pulses to set on the device (0-4).
     *
     * @return The constructed command to set the number of chlorine pulses.
     */
    static std::optional<CsBleJsonWriteCommand> SetChlorinePulses(int value)
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::IntValue(
                Key(CsSignatureValvePacketKey::AsChlorinePulses),
                value, 0, 4
            )
        );
    }

    /**
     * Builds a CsBleJsonWriteCommand to set the display on/off state on the device. This can be used to turn on or off
     * the display.
     *
     * @param[in] value The display state to set on the device.
     *
     * @return The constructed command to set the display state.
     */
    static std::optional<CsBleJsonWriteCommand> SetDisplayState(CsDeviceActiveState value)
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::IntValue(
                Key(CsSignatureValvePacketKey::AsDisplayOff),
                static_cast<int>(value),
                static_cast<int>(CsDeviceActiveState::Inactive),
                static_cast<int>(CsDeviceActiveState::Active)
            )
        );
    }

    /**
     * Builds a CsBleJsonWriteCommand to set the cycle position value on the device. This can be used to adjust the
     * time or salt pounds for a specific cycle position.
     *
     * @param[in] state The current state of the signature valve.
     * @param[in] position The cycle position to set.
     * @param[in] value The value to set for the specified cycle position.
     *
     * @return The constructed command to set the cycle position value, or std::nullopt if the position is invalid or
     *         options are not available.
     */
    static std::optional<CsBleJsonWriteCommand> SetCyclePosition(
        const CsSignatureValveState& state,
        CsSignatureValveCyclePosition position,
        int value
    )
    {
        const auto positionIndex = static_cast<size_t>(position);

        if (position == CsSignatureValveCyclePosition::Unknown ||
            positionIndex <= 1U ||
            positionIndex >= state.AsPositionTimes.size())
        {
            return std::nullopt;
        }

        const auto optionsIterator = state.AsPositionOptions.find(position);

        if (optionsIterator == state.AsPositionOptions.end())
        {
            return std::nullopt;
        }

        const CsSignatureValveCyclePositionOptionsSet& options = optionsIterator->second;
        const int maxValue = options.Contains(CsSignatureValveCyclePositionOption::TimeIsSaltPounds) ? 99 : 199;
        const int actualValue = std::clamp(value, 0, maxValue);

        std::vector<int> positionTimes(
            state.AsPositionTimes.begin(),
            state.AsPositionTimes.end()
        );

        positionTimes[positionIndex] = actualValue;

        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::ArrayIntValue(
                Key(CsSignatureValvePacketKey::AsPositionTimes),
                positionTimes,
                state.AsNumPositions
            )
        );
    }

public: // History Packets
    /**
     * Builds a CsBleJsonWriteCommand to reset the regeneration counter on the device. The regeneration counter tracks
     * how many regenerations have occurred since the last reset.
     *
     * @return The constructed command to reset the regeneration counter.
     */
    static std::optional<CsBleJsonWriteCommand> ResetRegenCounter()
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::BoolValue(
                Key(CsSignatureValvePacketKey::ShRegenCounterResettable),
                true
            )
        );
    }

    /**
     * Builds a CsBleJsonWriteCommand to reset the gallons totalizer on the device. The gallons totalizer tracks how
     * many gallons have been used since the last reset.
     *
     * @return The constructed command to reset the gallons totalizer.
     */
    static std::optional<CsBleJsonWriteCommand> ResetGallonsTotalizer()
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::BoolValue(
                Key(CsSignatureValvePacketKey::ShGallonsTotalizerResettable),
                true
            )
        );
    }

public: // Dealer Information Packets
    /**
     * Builds a CsBleJsonWriteCommand to set the dealer information on the device, including name, phone number,
     * address, email, and website.
     *
     * @param[in] name The name of the dealer.
     * @param[in] phone The phone number of the dealer.
     * @param[in] address1 The first line of the dealer's address.
     * @param[in] address2 The second line of the dealer's address (optional).
     * @param[in] email The email address of the dealer.
     * @param[in] web The website URL of the dealer.
     *
     * @return The constructed command to set the dealer information.
     */
    static std::optional<CsBleJsonWriteCommand> SetDealerInformation(
        const std::string& name,
        const std::string& phone,
        const std::string& address1,
        const std::string& address2,
        const std::string& email,
        const std::string& web
    )
    {
        return CsBleJsonWriteBuilder::BuildMany(
            {
                CsBleJsonWriteChange::ArrayIntValue(
                    Key(CsSignatureValvePacketKey::DiName),
                    ArrayIntValue(name),
                    std::min(static_cast<size_t>(100), name.size())
                ),
                CsBleJsonWriteChange::ArrayIntValue(
                    Key(CsSignatureValvePacketKey::DiPhone),
                    ArrayIntValue(phone),
                    std::min(static_cast<size_t>(50), phone.size())
                ),
                CsBleJsonWriteChange::ArrayIntValue(
                    Key(CsSignatureValvePacketKey::DiAddress1),
                    ArrayIntValue(address1),
                    std::min(static_cast<size_t>(100), address1.size())
                ),
                CsBleJsonWriteChange::ArrayIntValue(
                    Key(CsSignatureValvePacketKey::DiAddress2),
                    ArrayIntValue(address2),
                    std::min(static_cast<size_t>(100), address2.size())
                ),
                CsBleJsonWriteChange::ArrayIntValue(
                    Key(CsSignatureValvePacketKey::DiEmail),
                    ArrayIntValue(email),
                    std::min(static_cast<size_t>(100), email.size())
                ),
                CsBleJsonWriteChange::ArrayIntValue(
                    Key(CsSignatureValvePacketKey::DiWeb),
                    ArrayIntValue(web),
                    std::min(static_cast<size_t>(100), web.size())
                )
             }
        );
    }

public: // Global Packets
    /**
     * Builds a CsBleJsonWriteCommand to trigger an immediate regeneration on the device. This will cause the device to
     * perform the regeneration process immediately.
     *
     * @return The constructed command to trigger an immediate regeneration.
     */
    static std::optional<CsBleJsonWriteCommand> RegenNow()
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::BoolValue(
                Key(CsSignatureValvePacketKey::GlRegenNow),
                true
            )
        );
    }

    /**
     * Builds a CsBleJsonWriteCommand to schedule a regeneration for later on the device. This will set the device to
     * perform the regeneration process at the next scheduled time, rather than immediately.
     *
     * @return The constructed command to schedule a regeneration for later.
     */
    static std::optional<CsBleJsonWriteCommand> RegenNext()
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::BoolValue(
                Key(CsSignatureValvePacketKey::GlRegenNext),
                true
            )
        );
    }

    /**
     * Builds a CsBleJsonWriteCommand to trigger an immediate aeration cycle on the device. This will cause the device
     * to perform the aeration process immediately.
     *
     * @return The constructed command to trigger an immediate aeration cycle.
     */
    static std::optional<CsBleJsonWriteCommand> AerateNow()
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::BoolValue(
                Key(CsSignatureValvePacketKey::GlAerateNow),
                true
            )
        );
    }

    /**
     * Builds a CsBleJsonWriteCommand to schedule an aeration cycle for later on the device. This will set the device to
     * perform the aeration process at the next scheduled time, rather than immediately.
     *
     * @return The constructed command to schedule an aeration cycle for later.
     */
    static std::optional<CsBleJsonWriteCommand> AerateNext()
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::BoolValue(
                Key(CsSignatureValvePacketKey::GlAerateNext),
                true
            )
        );
    }

    /**
     * Builds a CsBleJsonWriteCommand to trigger the "Find Home" functionality on the device. This will ensure that the
     * encoder is properly aligned with the home position.
     *
     * @return The constructed command to trigger the "Find Home" functionality.
     */
    static std::optional<CsBleJsonWriteCommand> FindHome()
    {
        return CsBleJsonWriteBuilder::BuildSingle(
            CsBleJsonWriteChange::BoolValue(
                Key(CsSignatureValvePacketKey::GlFindHome),
                true
            )
        );
    }

private:
    static std::string Key(CsSignatureValvePacketKey key)
    {
        return CsSignatureValvePacketKeyMapper::ToRawKey(key);
    }

    static std::vector<int> ArrayIntValue(const std::string& str)
    {
        std::vector<int> result;
        result.reserve(str.size());

        for (unsigned char c : str)
        {
            result.push_back(static_cast<int>(c));
        }

        return result;
    }
};

}
