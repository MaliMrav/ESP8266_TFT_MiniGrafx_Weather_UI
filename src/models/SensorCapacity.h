#pragma once

// SensorCapacity defines the maximum number of sensor tiles the repository
// can hold. It is the only number that needs to change when the system grows.
//
// Individual domains declare their own sensor IDs as constexpr uint8_t
// constants in their own header files (e.g. WeatherSensorIds.h,
// SolarSensorIds.h). No domain needs to know about any other domain's IDs.
//
// SensorRepository validates that each declared sensor ID fits within this
// capacity. Domains own their IDs; the repository does not own domain ranges.

constexpr uint8_t MAX_SENSORS = 32;
