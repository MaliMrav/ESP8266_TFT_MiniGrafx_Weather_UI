#include "SensorRepository.h"
#include "WeatherSensorIds.h"
#include "SolarSensorIds.h"

// The tile array is indexed by domain-declared sensor ID constants.
// Slots not assigned to a domain are left zero-initialised.
// Each domain owns its own ID vocabulary; the repository remains a flat store.
static SensorTile sensorTiles[MAX_SENSORS] = {

    // Weather domain
    [SENSOR_KITCHEN_TEMP] = { "Kitchen Temp",  "\u00b0C", TEMP     },
    [SENSOR_PERGOLA_TEMP] = { "Pergola Temp",  "\u00b0C", TEMP     },
    [SENSOR_KITCHEN_HUM]  = { "Kitchen Hum",   "%",       HUMIDITY },
    [SENSOR_PERGOLA_HUM]  = { "Pergola Hum",   "%",       HUMIDITY },
    [SENSOR_PRESSURE]     = { "Pressure",      "hPa",     PRESSURE },

    // Solar domain
    [SENSOR_SOLAR_POWER_NOW]          = { "Production",      "W",  ENERGY_W  },
    [SENSOR_CONSUMPTION_POWER_NOW]    = { "Consumption",     "W",  ENERGY_W  },
    [SENSOR_EXPORT_POWER_NOW]         = { "Export",          "W",  ENERGY_W  },
    [SENSOR_BATTERY_POWER_NOW]        = { "Battery",         "W",  ENERGY_W  },
    [SENSOR_SOLAR_ENERGY_TODAY]       = { "Prod Today",      "Wh", ENERGY_WH },
    [SENSOR_CONSUMPTION_ENERGY_TODAY] = { "Cons Today",      "Wh", ENERGY_WH },
    [SENSOR_EXPORT_ENERGY_TODAY]      = { "Export Today",   "Wh", ENERGY_WH },
    [SENSOR_BATTERY_ENERGY_TODAY]     = { "Battery Today",  "Wh", ENERGY_WH },
};

static_assert(SENSOR_PRESSURE < MAX_SENSORS,
              "Weather sensor IDs exceed MAX_SENSORS capacity");

static_assert(SENSOR_SOLAR_POWER_NOW < MAX_SENSORS,
              "Solar sensor IDs exceed MAX_SENSORS capacity");
static_assert(SENSOR_CONSUMPTION_POWER_NOW < MAX_SENSORS,
              "Solar sensor IDs exceed MAX_SENSORS capacity");
static_assert(SENSOR_EXPORT_POWER_NOW < MAX_SENSORS,
              "Solar sensor IDs exceed MAX_SENSORS capacity");
static_assert(SENSOR_BATTERY_POWER_NOW < MAX_SENSORS,
              "Solar sensor IDs exceed MAX_SENSORS capacity");
static_assert(SENSOR_SOLAR_ENERGY_TODAY < MAX_SENSORS,
              "Solar sensor IDs exceed MAX_SENSORS capacity");
static_assert(SENSOR_CONSUMPTION_ENERGY_TODAY < MAX_SENSORS,
              "Solar sensor IDs exceed MAX_SENSORS capacity");
static_assert(SENSOR_EXPORT_ENERGY_TODAY < MAX_SENSORS,
              "Solar sensor IDs exceed MAX_SENSORS capacity");
static_assert(SENSOR_BATTERY_ENERGY_TODAY < MAX_SENSORS,
              "Solar sensor IDs exceed MAX_SENSORS capacity");

void SensorRepository::initialise()
{
    for (auto& tile : sensorTiles)
    {
        tile.value  = NAN;
        tile.minVal = NAN;
        tile.maxVal = NAN;
        tile.trend  = TREND_NONE;
        tile.valid  = false;
    }
}

SensorTile& SensorRepository::getTile(uint8_t id)
{
    return sensorTiles[id];
}

void SensorRepository::setValue(uint8_t id, float value)
{
    if (isnan(value)) return;
    sensorTiles[id].value = value;
    sensorTiles[id].valid = true;
}

void SensorRepository::setMin(uint8_t id, float value)
{
    if (!isnan(value)) sensorTiles[id].minVal = value;
}

void SensorRepository::setMax(uint8_t id, float value)
{
    if (!isnan(value)) sensorTiles[id].maxVal = value;
}

void SensorRepository::setTrend(uint8_t id, TrendDirection trend)
{
    sensorTiles[id].trend = trend;
}
