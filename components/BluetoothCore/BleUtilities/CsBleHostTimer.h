#pragma once
#include <nimble/nimble_port.h>
#include <stdint.h>

namespace SignatureBridge::BluetoothCore::BleUtilities {

/**
 * One-shot timer whose callback runs on the NimBLE host task.
 *
 * The NimBLE host task also delivers every GAP and GATT event, so using this rather than esp_timer (which runs its
 * callbacks on its own task) keeps all of the connection state on a single task. A timer can't then race a connect,
 * disconnect or notification event.
 *
 * The timer is set up on first use, as the NimBLE event queue only exists once NimBLE has been initialized, and the
 * devices are created before that.
 */
class CsBleHostTimer
{
public:
    using Callback = void (*)(void* arg);

public:
    CsBleHostTimer(Callback callback, void* arg)
        : _callback(callback), _arg(arg) { }

    ~CsBleHostTimer()
    {
        if (_initialized)
        {
            ble_npl_callout_stop(&_callout);
            ble_npl_callout_deinit(&_callout);
        }
    }

    CsBleHostTimer(const CsBleHostTimer&) = delete;
    CsBleHostTimer& operator=(const CsBleHostTimer&) = delete;

public:
    /**
     * Starts (or restarts) the timer.
     *
     * @param[in] delayMs The time, in milliseconds, until the callback runs.
     *
     * @return True if the timer was started, false otherwise.
     */
    bool Start(uint32_t delayMs)
    {
        if (!Initialize())
        {
            return false;
        }

        return ble_npl_callout_reset(&_callout, ble_npl_time_ms_to_ticks32(delayMs)) == 0;
    }

    /**
     * Stops the timer, if it's running.
     */
    void Stop()
    {
        if (_initialized)
        {
            ble_npl_callout_stop(&_callout);
        }
    }

private:
    bool Initialize()
    {
        if (_initialized)
        {
            return true;
        }

        struct ble_npl_eventq* queue = nimble_port_get_dflt_eventq();

        if (queue == nullptr)
        {
            return false;
        }

        ble_npl_callout_init(&_callout, queue, &CsBleHostTimer::OnExpired, this);
        _initialized = true;

        return true;
    }

    static void OnExpired(struct ble_npl_event* event)
    {
        auto* timer = static_cast<CsBleHostTimer*>(ble_npl_event_get_arg(event));

        if (timer != nullptr && timer->_callback != nullptr)
        {
            timer->_callback(timer->_arg);
        }
    }

private:
    void* _arg = nullptr;
    bool _initialized = false;
    Callback _callback = nullptr;
    struct ble_npl_callout _callout = {};
};

}
