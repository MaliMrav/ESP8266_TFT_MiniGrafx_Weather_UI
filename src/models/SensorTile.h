#pragma once

// SensorTile is the display model for a single sensor reading.
//
// It holds everything a screen needs to render one data tile:
// label, unit, current value, daily min/max, trend direction, and
// a validity flag that screens use to show placeholder text until
// real data arrives.
//
// SensorTile is a plain data struct. It has no behaviour of its own.
// Data sources write to it via SensorRepository. Screens read it via
// SensorRepository::getTile().

enum SensorType {
  TEMP,
  HUMIDITY,
  PRESSURE,
  ENERGY_W,   // instantaneous power in Watts
  ENERGY_WH   // accumulated energy in Watt-hours
};

enum TrendDirection {
  TREND_NONE,
  TREND_UP,
  TREND_DOWN,
  TREND_FLAT
};

struct SensorTile {

  const char* label;
  const char* unit;

  SensorType type;

  float value;
  float minVal;
  float maxVal;

  TrendDirection trend;
  bool valid;
};