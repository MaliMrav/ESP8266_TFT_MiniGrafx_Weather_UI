#pragma once

// SensorRepository is the runtime store for observation data.
//
// The repository owns storage.
// It does not own semantic observation identity.
//
// Domain code identifies observations with ObservationKey and obtains an
// ObservationHandle through ObservationRegistry. The repository then binds
// that handle to one of its private storage slots.
//
//     ObservationKey
//           │
//           ▼
//     ObservationRegistry
//           │
//           ▼
//     ObservationHandle
//           │
//           ▼
//     SensorRepository
//           │
//           ▼
//     private storage slot
//
// Legacy uint8_t ID APIs remain temporarily during the Zeta migration.
// They will be removed once existing domains have migrated to handles.

#include <Arduino.h>
#include "SensorTile.h"
#include "SensorCapacity.h"
#include "../data/ObservationHandle.h"

namespace SensorRepository
{
    void initialise();

    // Bind a runtime observation handle to repository storage.
    //
    // Returns false if the handle is invalid or no storage slot is available.
    bool registerObservation(
        ObservationHandle handle,
        const SensorTile& tile);

    // Retrieve a registered observation by its opaque runtime handle.
    //
    // Returns nullptr when the handle has not been registered.
    SensorTile* getTile(ObservationHandle handle);

    // Typed write API using the opaque runtime handle.
    bool setValue(ObservationHandle handle, float value);
    bool setMin  (ObservationHandle handle, float value);
    bool setMax  (ObservationHandle handle, float value);
    bool setTrend(ObservationHandle handle, TrendDirection trend);
}
