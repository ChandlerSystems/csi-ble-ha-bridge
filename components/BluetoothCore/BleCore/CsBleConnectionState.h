#pragma once

namespace SignatureBridge::BluetoothCore::BleCore {

enum class CsBleConnectionState
{
    /**
     * The device is not currently connected and is not in the process of connecting. This is the default state for a
     * device when it is first discovered and added to the device registry. In this state, the device is idle and can be
     * connected to by the session manager when a connection is requested.
     */
    Idle,

    /**
     * The device is in the process of establishing a connection. This state indicates that a connection attempt is
     * currently underway, but the connection has not yet been established. During this state, the session manager is
     * actively trying to connect to the device, and the outcome of the connection attempt (success or failure) has not
     * yet been determined.
     */
    Connecting,

    /**
     * The device is connected to the remote device. In this state, the device has successfully established a connection
     * and can now communicate with the remote device. The session manager can perform operations such as reading and
     * writing characteristics, subscribing to notifications, and other BLE interactions.
     */
    Connected,

    /**
     * The device is in the process of discovering services on the remote device. In this state, the session manager is
     * actively querying the remote device for its available services, which are collections of characteristics that
     * define the device's capabilities.
     */
    DiscoveringServices,

    /**
     * The device is in the process of discovering characteristics on the remote device. In this state, the session
     * manager is actively querying the remote device for its available characteristics, which are individual data
     * points that can be read from or written to, or that can send notifications.
     */
    DiscoveringCharacteristics,

    /**
     * In this state, the session manager is negotiating the Maximum Transmission Unit (MTU) size, which determines the
     * maximum size of data packets that can be exchanged between the devices. A successful MTU negotiation can improve
     * the efficiency of data transfer by allowing larger packets to be sent, while a failed negotiation may result in
     * a fallback to a default MTU size.
     */
    NegotiatingMtu,

    /**
     * The device is in the process of initializing. In this state, the session manager is performing any necessary
     * initialization steps after the connection has been established and the services and characteristics have been
     * discovered. This may include setting up notifications, writing initial values to characteristics, or performing
     * other setup tasks that are required before the device is ready for normal operation.
     */
    Initializing,

    /**
     * The device is in the process of authenticating. In this state, the session manager is performing authentication
     * steps with the remote device.
     */
    Authenticating,

    /**
     * The device is ready for normal operation. In this state, the session manager has completed all necessary
     * initialization steps, and the device can now perform its intended functions. This is the state where the device
     * is fully operational and can handle regular BLE interactions.
     */
    Ready,

    /**
     * The device is in the process of reconnecting. In this state, the session manager is actively attempting to
     * re-establish a connection with the remote device after a disconnection or connection failure. This may involve
     * retrying connection attempts, handling backoff strategies, and ensuring that the device transitions back to the
     * Ready state once the connection is successfully re-established.
     */
    Reconnecting
};

}
