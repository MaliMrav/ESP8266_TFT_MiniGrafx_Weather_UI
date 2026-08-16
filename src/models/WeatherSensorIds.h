#pragma once

#include <Arduino.h>

// WeatherSensorIds declares the sensor IDs owned by the weather domain.
//
// These IDs are stable integer indices into SensorRepository.
// They are defined here, close to the screens and data sources that use them,
// rather than in a global enum that every domain must include.
//
// To add a weather sensor: add its ID here, add its tile in
// SensorRepository.cpp, and add its topic bindings in TopicMappings.cpp.

constexpr uint8_t SENSOR_KITCHEN_TEMP  = 0;
constexpr uint8_t SENSOR_PERGOLA_TEMP  = 1;
constexpr uint8_t SENSOR_KITCHEN_HUM   = 2;
constexpr uint8_t SENSOR_PERGOLA_HUM   = 3;
constexpr uint8_t SENSOR_PRESSURE      = 4;

constexpr uint8_t WEATHER_SENSOR_COUNT = 5;
