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
// ObservationKey does not own its text.
//
// Domain keys are expected to reference stable text, normally string literals:
//
//     constexpr ObservationKey key{
//         "sensor.envoy_current_power_production"
//     };
//
// Runtime resolution is deliberately outside this type:
//
//     ObservationKey
//           │
//           ▼
//     ObservationRegistry
//           │
//           ▼
//     ObservationHandle
//           │
//           ▼
//     SensorRepository
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