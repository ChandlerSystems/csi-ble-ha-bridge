#pragma once
#include <assert.h>
#include <memory>

namespace SignatureBridge::Core {


#ifndef ARG_UNUSED
    /**
     * @brief Macro used to indicate that an argument is intentionally unused, which can help prevent compiler warnings
     * about unused parameters.
     *
     * @param[in] arg The name of the argument that is unused.
     */
    #define ARG_UNUSED(arg) (void)(arg)
#endif


#ifndef ESUCCESS
    /**
     * @brief Defines a success error code. This makes the code easier to read and understand when checking for success.
     */
    #define ESUCCESS 0
#endif


#ifndef CS_BLE_MANUFACTURER_ID
    /**
     * @brief Defines the Bluetooth SIG assigned Manufacturer ID for Chandler Systems, Inc.
     */
    #define CS_BLE_MANUFACTURER_ID 0x073A
#endif


#ifndef CS_BLE_ADDR_STR_LEN
    /**
     * @brief Defines the length of a string buffer that can hold a BLE address in string format
     * (e.g. "01:23:45:67:89:AB") including the null terminator.
     */
    #define CS_BLE_ADDR_STR_LEN 18
#endif


#ifndef CS_MAX_ADV_NAME_LEN
    /**
     * @brief Defines the maximum length of a Bluetooth Low Energy advertisement name. This is used to ensure that
     * buffers allocated for storing advertisement names are of sufficient size.
     */
    #define CS_MAX_ADV_NAME_LEN 8
#endif


/**
 * @brief Macro used to define a class tag for logging and identification purposes.
 */
#define CS_CLASS_TAG(CLASS_NAME)                                                                                       \
    private:                                                                                                           \
        static constexpr const char* _tag = #CLASS_NAME;                                                               \
    public:                                                                                                            \
        static constexpr const char* Tag() { return _tag; }


/**
 * @def ENSURE_INITIALIZED
 *
 * @brief Macro used to ensure that a component was successfully initialized by checking the INITIALIZED flag. If it's
 *        not then an assert is invoked.
 */
#define CS_ENSURE_INITIALIZED(FUNC_STR, COMPONENT_STR, INITIALIZED)                                                    \
    do {                                                                                                               \
        if (!(INITIALIZED))                                                                                            \
        {                                                                                                              \
            ESP_LOGE(                                                                                                  \
                (COMPONENT_STR),                                                                                       \
                "[%s] The component has not been initialized (%s:%d)",                                                 \
                (FUNC_STR), __FILE__, __LINE__                                                                         \
            );                                                                                                         \
            abort();                                                                                                   \
        }                                                                                                              \
    } while (0)

}
