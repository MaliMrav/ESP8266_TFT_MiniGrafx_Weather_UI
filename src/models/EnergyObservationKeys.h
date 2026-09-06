#pragma once

#include "../data/ObservationKey.h"

// EnergyObservationKeys defines the semantic observation identities owned by
// the solar / energy domain.
//
// The names deliberately match the Home Assistant entity IDs already in use.
// This does not create a dependency on Home Assistant.
//
// The same semantic observation may later be supplied by:
//     - MQTT
//     - API
//     - another gateway
//     - a local sensor
//
// The transport is independent of the observation identity.

namespace EnergyObservations
{
    constexpr ObservationKey CURRENT_POWER_PRODUCTION{
        "sensor.envoy_current_power_production"
    };

    constexpr ObservationKey ENERGY_PRODUCTION_TODAY{
        "sensor.envoy_energy_production_today"
    };

    constexpr ObservationKey CURRENT_POWER_CONSUMPTION{
        "sensor.envoy_current_power_consumption"
    };

    constexpr ObservationKey ENERGY_CONSUMPTION_TODAY{
        "sensor.envoy_energy_consumption_today"
    };
}