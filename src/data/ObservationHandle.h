#pragma once

#include <Arduino.h>

class ObservationHandle;

// Internal construction point used by the observation registry.
//
// Application and domain code must not construct handles from numeric values.
// The numeric representation remains an implementation detail.
constexpr ObservationHandle makeObservationHandle(uint8_t value);


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
// Domain code must not construct an ObservationHandle from a number.
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
//     repository storage
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

    // Only the runtime registry/factory may create a valid handle.
    friend constexpr ObservationHandle makeObservationHandle(
        uint8_t value);
};


// Internal construction point for the runtime observation registry.
constexpr ObservationHandle makeObservationHandle(uint8_t value)
{
    return ObservationHandle(value);
}