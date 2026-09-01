#include "SensorRepository.h"

#include "WeatherSensorIds.h"

namespace
{
    // The repository owns storage, not semantic observation identity.
    //
    // Weather remains on the legacy ID-based API during the Zeta migration.
    // New observations are associated with storage through ObservationHandle.
    static SensorTile sensorTiles[MAX_SENSORS] = {

        // ---------------------------------------------------------------------
        // Legacy Weather storage
        // ---------------------------------------------------------------------

        [SENSOR_KITCHEN_TEMP] = {
            "Kitchen Temp",
            "°C",
            TEMP
        },

        [SENSOR_PERGOLA_TEMP] = {
            "Pergola Temp",
            "°C",
            TEMP
        },

        [SENSOR_KITCHEN_HUM] = {
            "Kitchen Hum",
            "%",
            HUMIDITY
        },

        [SENSOR_PERGOLA_HUM] = {
            "Pergola Hum",
            "%",
            HUMIDITY
        },

        [SENSOR_PRESSURE] = {
            "Pressure",
            "hPa",
            PRESSURE
        }
    };

    // Runtime observation handles are the repository's only association to
    // semantic observations.
    //
    // The numeric representation of ObservationHandle remains opaque to the
    // repository's callers.
    static ObservationHandle observationHandles[MAX_SENSORS];

    // Number of storage slots currently populated.
    //
    // The first five slots remain occupied by the legacy Weather domain while
    // Weather is migrated to ObservationHandle.
    //
    // New handle-backed observations are allocated after these legacy slots.
    static constexpr uint8_t LEGACY_SENSOR_STORAGE_COUNT = 5;

    static uint8_t observationCount = LEGACY_SENSOR_STORAGE_COUNT;

    SensorTile* findTile(ObservationHandle handle)
    {
        if (!handle.isValid())
        {
            return nullptr;
        }

        for (uint8_t i = LEGACY_SENSOR_STORAGE_COUNT;
             i < observationCount;
             ++i)
        {
            if (observationHandles[i] == handle)
            {
                return &sensorTiles[i];
            }
        }

        return nullptr;
    }
}

static_assert(
    SENSOR_KITCHEN_TEMP < MAX_SENSORS,
    "Weather sensor ID exceeds MAX_SENSORS capacity");

static_assert(
    SENSOR_PERGOLA_TEMP < MAX_SENSORS,
    "Weather sensor ID exceeds MAX_SENSORS capacity");

static_assert(
    SENSOR_KITCHEN_HUM < MAX_SENSORS,
    "Weather sensor ID exceeds MAX_SENSORS capacity");

static_assert(
    SENSOR_PERGOLA_HUM < MAX_SENSORS,
    "Weather sensor ID exceeds MAX_SENSORS capacity");

static_assert(
    SENSOR_PRESSURE < MAX_SENSORS,
    "Weather sensor ID exceeds MAX_SENSORS capacity");


void SensorRepository::initialise()
{
    observationCount = LEGACY_SENSOR_STORAGE_COUNT;

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

    // Registration is idempotent at the repository boundary.
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


SensorTile* SensorRepository::getTile(
    ObservationHandle handle)
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
// Legacy Weather ID-based API.
//
// Retained during the Zeta migration.
// -----------------------------------------------------------------------------

SensorTile& SensorRepository::getTile(uint8_t id)
{
    return sensorTiles[id];
}


void SensorRepository::setValue(
    uint8_t id,
    float value)
{
    if (isnan(value))
    {
        return;
    }

    sensorTiles[id].value = value;
    sensorTiles[id].valid = true;
}


void SensorRepository::setMin(
    uint8_t id,
    float value)
{
    if (!isnan(value))
    {
        sensorTiles[id].minVal = value;
    }
}


void SensorRepository::setMax(
    uint8_t id,
    float value)
{
    if (!isnan(value))
    {
        sensorTiles[id].maxVal = value;
    }
}


void SensorRepository::setTrend(
    uint8_t id,
    TrendDirection trend)
{
    sensorTiles[id].trend = trend;
}