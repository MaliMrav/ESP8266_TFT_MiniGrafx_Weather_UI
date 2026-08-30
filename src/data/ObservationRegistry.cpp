#include "ObservationRegistry.h"

namespace
{
    ObservationKey keys[ObservationRegistry::MAX_OBSERVATIONS] = {
        ObservationKey{""}
    };

    uint8_t count = 0;
}

namespace ObservationRegistry
{
    void initialise()
    {
        count = 0;
    }

    ObservationHandle registerObservation(const ObservationKey& key)
    {
        for (uint8_t i = 0; i < count; ++i)
        {
            if (keys[i] == key)
            {
                return makeObservationHandle(i);
            }
        }

        if (count >= MAX_OBSERVATIONS)
        {
            return ObservationHandle{};
        }

        keys[count] = key;
        return makeObservationHandle(count++);
    }

    ObservationHandle resolve(const ObservationKey& key)
    {
        for (uint8_t i = 0; i < count; ++i)
        {
            if (keys[i] == key)
            {
                return makeObservationHandle(i);
            }
        }

        return ObservationHandle{};
    }
}
