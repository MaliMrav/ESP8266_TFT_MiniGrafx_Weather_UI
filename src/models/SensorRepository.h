#pragma once

// SensorRepository is the single source of truth for all sensor data.
//
// Responsibilities:
//   - own the fixed-capacity array of SensorTiles
//   - expose a typed write API so data sources update values by sensor ID
//   - expose a read API for screens to retrieve individual tiles by ID
//
// Sensor IDs are declared per-domain (WeatherSensorIds.h, SolarSensorIds.h,
// etc.) as constexpr uint8_t constants. The repository has no knowledge of
// domains — it is a flat indexed store. Domains are responsible for
// assigning non-overlapping IDs within MAX_SENSORS.
//
// The write API (setValue, setMin, setMax, setTrend) is the only legitimate
// way for data sources to mutate tile state. Direct array access from
// outside this module is intentionally not supported.

#include <Arduino.h>
#include "SensorTile.h"
#include "SensorCapacity.h"

namespace SensorRepository
{
    void        initialise();

    // Returns a single tile by its domain-declared sensor ID.
    SensorTile& getTile(uint8_t id);

    // Typed write API — the only way data sources should write to the repository.
    // Domain sensor ID constants are the stable identity; data sources never
    // touch tile indices directly.
    void setValue(uint8_t id, float value);
    void setMin  (uint8_t id, float value);
    void setMax  (uint8_t id, float value);
    void setTrend(uint8_t id, TrendDirection trend);
}