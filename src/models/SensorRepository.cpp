#include "SensorRepository.h"

namespace
{
    // The repository owns storage, not semantic observation identity.
    //
    // Storage is allocated as observations are registered at runtime.
    static SensorTile sensorTiles[MAX_SENSORS];
    static ObservationHandle observationHandles[MAX_SENSORS];

    static uint8_t observationCount = 0;

    SensorTile* findTile(
        ObservationHandle handle)
    {
        if (!handle.isValid())
        {
            return nullptr;
        }

        for (uint8_t i = 0;
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

void SensorRepository::initialise()
{
    observationCount = 0;

    for (uint8_t i = 0;
         i < MAX_SENSORS;
         ++i)
    {
        sensorTiles[i] = SensorTile{
            nullptr,
            nullptr,
            TEMP,
            NAN,
            NAN,
            NAN,
            TREND_NONE,
            false
        };

        observationHandles[i] =
            ObservationHandle{};
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

    SensorTile* tile =
        findTile(handle);

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
    SensorTile* tile =
        findTile(handle);

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
    SensorTile* tile =
        findTile(handle);

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
    SensorTile* tile =
        findTile(handle);

    if (!tile)
    {
        return false;
    }

    tile->trend = trend;

    return true;
}