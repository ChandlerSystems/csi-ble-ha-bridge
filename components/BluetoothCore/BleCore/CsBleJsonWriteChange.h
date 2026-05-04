#pragma once
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace SignatureBridge::BluetoothCore::BleCore {

class CsBleJsonWriteChange
{
public:
    /**
     * Formats an integer value as a JSON key-value pair, ensuring the value is within the specified range.
     *
     * @param[in] key The JSON key.
     * @param[in] value The integer value.
     * @param[in] minValue The minimum allowed value.
     * @param[in] maxValue The maximum allowed value.
     *
     * @return The formatted JSON key-value pair.
     */
    static std::string IntValue(const std::string& key, int value, int minValue, int maxValue)
    {
        value = std::clamp(value, minValue, maxValue);
        return "\"" + key + "\":" + std::to_string(value);
    }

    /**
     * Formats a boolean value as a JSON key-value pair, representing true as 1 and false as 0.
     *
     * @param[in] key The JSON key.
     * @param[in] value The boolean value.
     *
     * @return The formatted JSON key-value pair.
     */
    static std::string BoolValue(const std::string& key, bool value)
    {
        return IntValue(key, value ? 1 : 0, 0, 1);
    }

    /**
     * Formats an array of integers as a JSON key-value pair, ensuring the number of values does not exceed the
     * specified maximum length.
     *
     * @param[in] key The JSON key.
     * @param[in] values The array of integer values.
     * @param[in] maxLength The maximum number of values to include.
     *
     * @return The formatted JSON key-value pair.
     */
    static std::string ArrayIntValue(const std::string& key, const std::vector<int>& values, size_t maxLength)
    {
        std::ostringstream json;
        json << "\"" << key << "\":[";

        const size_t count = std::min(values.size(), maxLength);

        for (size_t i = 0; i < count; ++i)
        {
            if (i > 0)
            {
                json << ",";
            }

            json << values[i];
        }

        json << "]";
        return json.str();
    }
};

}
