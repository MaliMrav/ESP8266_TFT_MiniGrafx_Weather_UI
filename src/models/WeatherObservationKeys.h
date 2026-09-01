#pragma once

#include "../data/ObservationKey.h"

// WeatherObservationKeys declares the semantic observation identities owned
// by the weather domain.
//
// These keys identify observations, not transports or storage.
//
// The current Weather observations pre-date the Home Assistant naming
// convention used by the Solar domain, so they use stable Telemetry semantic
// names here.
//
// A transport may supply these observations through MQTT, API, local I/O,
// or another source without changing their identity.

namespace WeatherObservations
{
    constexpr ObservationKey KITCHEN_TEMPERATURE{
        "weather.kitchen.temperature"
    };

    constexpr ObservationKey PERGOLA_TEMPERATURE{
        "weather.pergola.temperature"
    };

    constexpr ObservationKey KITCHEN_HUMIDITY{
        "weather.kitchen.humidity"
    };

    constexpr ObservationKey PERGOLA_HUMIDITY{
        "weather.pergola.humidity"
    };

    constexpr ObservationKey PRESSURE{
        "weather.pressure"
    };
}