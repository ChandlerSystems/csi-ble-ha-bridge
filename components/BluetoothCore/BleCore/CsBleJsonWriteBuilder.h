#pragma once
#include "CsBleJsonWriteCommand.h"
#include <sstream>
#include <string>
#include <vector>

namespace SignatureBridge::BluetoothCore::BleCore {

class CsBleJsonWriteBuilder
{
public:
    /**
     * Builds a CsBleJsonWriteCommand from a single JSON change string.
     *
     * @param[in] change The JSON change string.
     *
     * @return The constructed command.
     */
    static CsBleJsonWriteCommand BuildSingle(const std::string& change)
    {
        return CsBleJsonWriteCommand("{" + change + "}");
    }

    /**
     * Builds a CsBleJsonWriteCommand from multiple JSON change strings, combining them into a single JSON object.
     *
     * @param[in] changes The vector of JSON change strings.
     *
     * @return The constructed command.
     */
    static CsBleJsonWriteCommand BuildMany(const std::vector<std::string>& changes)
    {
        std::ostringstream json;
        json << "{";

        for (size_t i = 0; i < changes.size(); ++i)
        {
            if (i > 0)
            {
                json << ",";
            }

            json << changes[i];
        }

        json << "}";
        return CsBleJsonWriteCommand(json.str());
    }
};

}
