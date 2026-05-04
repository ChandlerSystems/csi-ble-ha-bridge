#pragma once

namespace SignatureBridge::BluetoothCore::BleCore {

enum class CsBleAuthState
{
    /**
     * The authentication state is unknown. This is the default state before any authentication attempts have been
     * made, or if the authentication state cannot be determined for some reason.
     */
    Unknown,

    /**
     * The device is not authenticated. This state indicates that the device has not successfully completed the
     * authentication process.
     */
    Unauthenticated,

    /**
     * The device is authenticated. This state indicates that the device has successfully completed the authentication
     * process.
     */
    Authenticated,

    /**
     * The device is rate limited. This state indicates that the device has exceeded the allowed number of authentication
     * attempts in a given time period.
     */
    RateLimited,

    /**
     * The authentication request timed out. This state indicates that the device did not receive a response to its
     * authentication request within the expected time frame.
     */
    AuthRequestTimedOut
};

}
