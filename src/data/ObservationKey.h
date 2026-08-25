#pragma once

#include <Arduino.h>

// ObservationKey identifies an observation by its semantic name.
//
// The key is part of the domain-facing architecture. It answers:
//
//     "What observation are we talking about?"
//
// It does not identify:
//
//     - a data source
//     - a repository slot
//     - a runtime handle
//     - a physical device
//
// A key may correspond to an existing external identity, such as a
// Home Assistant entity ID:
//
//     sensor.bar_switch_panel_energy_power
//
// The same ObservationKey may be resolved from different source
// mechanisms (MQTT, API, I2C, One-Wire, Modbus, etc.).
//
// ObservationKey therefore represents meaning, not storage.
//
// Runtime resolution:
//
//     ObservationKey
//           │
//           ▼
//     ObservationHandle
//           │
//           ▼
//     Repository storage
//

struct ObservationKey
{
    const char* value;

    constexpr ObservationKey(const char* value)
        : value(value)
    {
    }

    constexpr const char* c_str() const
    {
        return value;
    }
};