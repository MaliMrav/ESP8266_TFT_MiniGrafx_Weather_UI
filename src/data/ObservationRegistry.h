#pragma once

#include "ObservationHandle.h"
#include "ObservationKey.h"

// ObservationRegistry maps stable semantic ObservationKeys to opaque
// runtime ObservationHandles.
//
// The registry is deliberately a fixed-capacity runtime structure.
// Registration is deterministic, allocation-free, and independent of any
// particular data source or repository storage implementation.
//
// An ObservationKey is non-owning. The text referenced by a registered key
// must therefore remain valid for as long as the key is registered.
//
// Registration is idempotent: registering an already-known key returns the
// existing handle rather than allocating another runtime record.
//
// Resolution is intentionally a separate operation so sources may resolve an
// already-registered observation without changing the registry.
//
//     ObservationKey
//           │
//           ▼
//     ObservationRegistry
//           │
//           ▼
//     ObservationHandle
//
namespace ObservationRegistry
{
    // Current maximum number of runtime observations.
    constexpr uint8_t MAX_OBSERVATIONS = 32;

    // Reset the registry to its empty state.
    void initialise();

    // Register a semantic observation key.
    //
    // If the key already exists, its existing handle is returned.
    // If the registry is full, an invalid handle is returned.
    ObservationHandle registerObservation(const ObservationKey& key);

    // Resolve an already-registered key.
    //
    // Returns an invalid handle when the key is not registered.
    ObservationHandle resolve(const ObservationKey& key);
}
