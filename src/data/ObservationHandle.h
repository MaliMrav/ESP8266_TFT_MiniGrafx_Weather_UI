#pragma once

#include <Arduino.h>

// ObservationHandle is an opaque runtime reference to a registered
// observation.
//
// The handle answers:
//
//     "Which runtime observation record are we referring to?"
//
// It deliberately does not expose:
//
//     - the ObservationKey
//     - the source
//     - the repository storage representation
//     - the storage index
//
// The numeric representation is an implementation detail of the runtime.
// Domain code must never construct an ObservationHandle from a number.
//
// Handles are created by the observation registration/resolution layer and
// subsequently passed to runtime consumers such as SensorRepository.
//
// Current implementation:
//     - one byte is sufficient for the current fixed-capacity repository
//
// The representation may change without changing the semantic contract.
//
//     ObservationKey
//           │
//           ▼
//     runtime resolution
//           │
//           ▼
//     ObservationHandle
//           │
//           ▼
//     repository record
//

class ObservationHandle
{
public:
    constexpr ObservationHandle()
        : value_(INVALID)
    {
    }

    constexpr bool isValid() const
    {
        return value_ != INVALID;
    }

    constexpr bool operator==(const ObservationHandle& other) const
    {
        return value_ == other.value_;
    }

    constexpr bool operator!=(const ObservationHandle& other) const
    {
        return !(*this == other);
    }

private:
    static constexpr uint8_t INVALID = 0xFF;

    explicit constexpr ObservationHandle(uint8_t value)
        : value_(value)
    {
    }

    uint8_t value_;

    // Runtime registration/resolution is the only legitimate creator of
    // handles. The repository itself does not define semantic identity.
    friend ObservationHandle makeObservationHandle(uint8_t value);
};


// Internal construction point for the runtime observation registry.
//
// This function exists only as the bridge between runtime allocation and the
// opaque handle type. Application and domain code must not call it directly.
constexpr ObservationHandle makeObservationHandle(uint8_t value)
{
    return ObservationHandle(value);
}