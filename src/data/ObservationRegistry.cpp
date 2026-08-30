#include "ObservationRegistry.h"

namespace
{
    std::string_view keys[ObservationRegistry::MAX_OBSERVATIONS];

    uint8_t count = 0;
}

namespace ObservationRegistry
{
    void initialise()
    {
        count = 0;

        for (auto& key : keys)
        {
            key = std::string_view{};
        }
    }

    ObservationHandle registerObservation(
        const ObservationKey& key)
    {
        for (uint8_t i = 0; i < count; ++i)
        {
            if (keys[i] == key.view())
            {
                return makeObservationHandle(i);
            }
        }

        if (count >= MAX_OBSERVATIONS)
        {
            return ObservationHandle{};
        }

        keys[count] = key.view();

        return makeObservationHandle(count++);
    }

    ObservationHandle resolve(const ObservationKey& key)
    {
        for (uint8_t i = 0; i < count; ++i)
        {
            if (keys[i] == key.view())
            {
                return makeObservationHandle(i);
            }
        }

        return ObservationHandle{};
    }
}