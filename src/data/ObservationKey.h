#pragma once

#include "../data/ObservationKey.h"

// SolarObservationKeys declares the semantic identities owned by the
// solar / energy domain.
//
// The key names intentionally match the Home Assistant entity identities
// already in use. These strings identify observations; they do not identify
// a transport or storage location.
//
// Transport remains independent:
//     MQTT
//     API
//     local sensor
//     future source
//
// Runtime storage is independent:
//     ObservationHandle
//     SensorRepository

namespace SolarObservations
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