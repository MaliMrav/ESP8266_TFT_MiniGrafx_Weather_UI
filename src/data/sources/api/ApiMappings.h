#pragma once

// ApiMappings isolates provider-specific API resources and field locations
// from the generic API source mechanism.
//
// The current mapping describes the local solar API currently configured
// for this firmware. A different API provider can replace this mapping
// without changing SensorRepository, Screens, or IDataSource.
//
// Domain identity remains in the domain-owned SensorIds headers.

#include <Arduino.h>
#include "../../../models/SolarSensorIds.h"

namespace ApiMappings
{
    constexpr const char* REALTIME_PATH =
        "/ivp/meters/readings";

    constexpr const char* DAILY_TOTALS_PATH =
        "/api/v1/production";

    constexpr uint8_t PRODUCTION_METER_INDEX = 0;
    constexpr uint8_t GRID_METER_INDEX       = 1;

    constexpr const char* ACTIVE_POWER_FIELD =
        "activePower";

    constexpr const char* PRODUCTION_WATT_HOURS_TODAY =
        "production.wattHoursToday";

    constexpr const char* CONSUMPTION_WATT_HOURS_TODAY =
        "consumption.wattHoursToday";
}
