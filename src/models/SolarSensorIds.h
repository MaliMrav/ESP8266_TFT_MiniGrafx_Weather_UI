#pragma once

#include <Arduino.h>

// SolarSensorIds declares the sensor IDs owned by the solar/energy domain.
//
// These IDs are stable integer indices into SensorRepository.
// They are defined here, close to the screens and data sources that use them,
// rather than in a global enum that every domain must include.
//
// The repository remains a flat runtime store. Domain identity is provided by
// these explicit IDs rather than by ranges or positional assumptions.
//
// To add a solar sensor: add its ID here, add its tile in
// SensorRepository.cpp, and add its topic binding in TopicMappings.cpp.

constexpr uint8_t SENSOR_SOLAR_POWER_NOW       = 5;
constexpr uint8_t SENSOR_CONSUMPTION_POWER_NOW = 6;
constexpr uint8_t SENSOR_EXPORT_POWER_NOW      = 7;
constexpr uint8_t SENSOR_BATTERY_POWER_NOW     = 8;

constexpr uint8_t SENSOR_SOLAR_ENERGY_TODAY       = 9;
constexpr uint8_t SENSOR_CONSUMPTION_ENERGY_TODAY = 10;
constexpr uint8_t SENSOR_EXPORT_ENERGY_TODAY      = 11;
constexpr uint8_t SENSOR_BATTERY_ENERGY_TODAY     = 12;

constexpr uint8_t SOLAR_SENSOR_COUNT = 8;
