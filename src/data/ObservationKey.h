#pragma once

#include <string_view>

// ObservationKey identifies an observation by its semantic identity.
//
// The key answers:
//
//     "What observation are we talking about?"
//
// It does not identify:
//
//     - a data source
//     - a physical device
//     - a repository storage slot
//     - a runtime handle
//
// An ObservationKey does not own its text.
//
// The referenced text must therefore have a lifetime at least as long
// as the ObservationKey is used. Domain keys are expected to be declared
// from static string literals:
//
//     constexpr ObservationKey key{
//         "sensor.bar_switch_panel_energy_power"
//     };
//
// Existing external identities, such as Home Assistant entity IDs,
// may be used directly where they already provide a meaningful,
// stable semantic identity.
//
// The same ObservationKey may be resolved through different source
// mechanisms:
//
//     MQTT
//     API
//     I2C
//     One-Wire
//     Modbus
//
// Runtime resolution is deliberately outside this contract:
//
//     ObservationKey
//           │
//           ▼
//     source resolution
//           │
//           ▼
//     ObservationHandle
//           │
//           ▼
//     repository storage
//

struct ObservationKey
{
    std::string_view value;

    constexpr explicit ObservationKey(
        std::string_view value)
        : value(value)
    {
    }

    constexpr std::string_view view() const
    {
        return value;
    }

    constexpr const char* c_str() const
    {
        return value.data();
    }

    constexpr std::size_t size() const
    {
        return value.size();
    }

    constexpr bool operator==(
        const ObservationKey& other) const
    {
        return value == other.value;
    }

    constexpr bool operator!=(
        const ObservationKey& other) const
    {
        return !(*this == other);
    }
};