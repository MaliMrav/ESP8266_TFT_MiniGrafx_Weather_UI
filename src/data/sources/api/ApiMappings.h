#pragma once

#include <Arduino.h>

// ApiMappings isolates provider-specific API resources and field locations
// from the generic API source mechanism.
//
// This file describes the structure of the configured API only.
// Semantic observation identity belongs to the domain-owned
// EnergyObservationKeys.h.
//
// A different API provider can replace this mapping without changing:
//     - SensorRepository
//     - Screens
//     - IDataSource
//

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