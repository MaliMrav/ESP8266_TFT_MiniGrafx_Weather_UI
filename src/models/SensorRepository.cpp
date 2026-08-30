#include "SensorRepository.h"
#include "WeatherSensorIds.h"
#include "SolarSensorIds.h"

namespace
{
    // Existing fixed storage remains the repository's private implementation.
    static SensorTile sensorTiles[MAX_SENSORS] = {

        // Weather domain
        [SENSOR_KITCHEN_TEMP] = { "Kitchen Temp",  "\u00b0C", TEMP     },
        [SENSOR_PERGOLA_TEMP] = { "Pergola Temp",  "\u00b0C", TEMP     },
        [SENSOR_KITCHEN_HUM]  = { "Kitchen Hum",   "%",       HUMIDITY },
        [SENSOR_PERGOLA_HUM]  = { "Pergola Hum",   "%",       HUMIDITY },
        [SENSOR_PRESSURE]     = { "Pressure",      "hPa",     PRESSURE },

        // Solar domain
        [SENSOR_SOLAR_POWER_NOW]          = { "Production",     "W",  ENERGY_W  },
        [SENSOR_CONSUMPTION_POWER_NOW]    = { "Consumption",    "W",  ENERGY_W  },
        [SENSOR_EXPORT_POWER_NOW]         = { "Export",         "W",  ENERGY_W  },
        [SENSOR_BATTERY_POWER_NOW]        = { "Battery",        "W",  ENERGY_W  },
        [SENSOR_SOLAR_ENERGY_TODAY]       = { "Prod Today",     "Wh", ENERGY_WH },
        [SENSOR_CONSUMPTION_ENERGY_TODAY] = { "Cons Today",     "Wh", ENERGY_WH },
        [SENSOR_EXPORT_ENERGY_TODAY]      = { "Export Today",   "Wh", ENERGY_WH },
        [SENSOR_BATTERY_ENERGY_TODAY]     = { "Battery Today",  "Wh", ENERGY_WH },
    };

    // Runtime observation handles are the repository's only association to
    // semantic observations. Storage order remains private and independent
    // of semantic identity.
    static ObservationHandle observationHandles[MAX_SENSORS];
    static uint8_t observationCount = 0;

    SensorTile* findTile(ObservationHandle handle)
    {
        for (uint8_t i = 0; i < observationCount; ++i)
        {
            if (observationHandles[i] == handle)
            {
                return &sensorTiles[i];
            }
        }

        return nullptr;
    }
}

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
    observationCount = 0;

    for (uint8_t i = 0; i < MAX_SENSORS; ++i)
    {
        observationHandles[i] = ObservationHandle{};

        sensorTiles[i].value  = NAN;
        sensorTiles[i].minVal = NAN;
        sensorTiles[i].maxVal = NAN;
        sensorTiles[i].trend  = TREND_NONE;
        sensorTiles[i].valid  = false;
    }
}

bool SensorRepository::registerObservation(
    ObservationHandle handle,
    const SensorTile& tile)
{
    if (!handle.isValid())
    {
        return false;
    }

    // Registration is idempotent at the repository boundary as well.
    // Existing storage is retained for an already-bound handle.
    if (findTile(handle))
    {
        return true;
    }

    if (observationCount >= MAX_SENSORS)
    {
        return false;
    }

    sensorTiles[observationCount] = tile;
    observationHandles[observationCount] = handle;
    ++observationCount;

    return true;
}

SensorTile* SensorRepository::getTile(ObservationHandle handle)
{
    return findTile(handle);
}

bool SensorRepository::setValue(
    ObservationHandle handle,
    float value)
{
    if (isnan(value))
    {
        return false;
    }

    SensorTile* tile = findTile(handle);

    if (!tile)
    {
        return false;
    }

    tile->value = value;
    tile->valid = true;
    return true;
}

bool SensorRepository::setMin(
    ObservationHandle handle,
    float value)
{
    SensorTile* tile = findTile(handle);

    if (!tile || isnan(value))
    {
        return false;
    }

    tile->minVal = value;
    return true;
}

bool SensorRepository::setMax(
    ObservationHandle handle,
    float value)
{
    SensorTile* tile = findTile(handle);

    if (!tile || isnan(value))
    {
        return false;
    }

    tile->maxVal = value;
    return true;
}

bool SensorRepository::setTrend(
    ObservationHandle handle,
    TrendDirection trend)
{
    SensorTile* tile = findTile(handle);

    if (!tile)
    {
        return false;
    }

    tile->trend = trend;
    return true;
}

// -----------------------------------------------------------------------------
// Legacy ID-based API retained during the Zeta migration.
// -----------------------------------------------------------------------------

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
