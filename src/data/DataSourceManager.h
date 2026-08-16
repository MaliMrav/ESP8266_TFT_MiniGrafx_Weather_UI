#pragma once

// DataSourceManager composes multiple IDataSource implementations into one.
//
// SystemManager holds a single IDataSource reference. DataSourceManager
// satisfies that contract while delegating begin() and loop() to every
// registered source in order.
//
// This allows MQTT, HTTP, local sensors, and any future source to coexist
// without SystemManager knowing how many sources exist or what they are.
//
// Usage:
//   DataSourceManager sources;
//   sources.add(mqttData);
//   sources.add(apiData);
//   SystemManager::begin(..., sources);

#include <Arduino.h>
#include "IDataSource.h"

class DataSourceManager : public IDataSource
{
public:
    static constexpr uint8_t MAX_SOURCES = 4;

    // Registers a source. Sources are called in registration order.
    // Returns false if capacity is exceeded (source is not added).
    bool add(IDataSource& source);

    void begin() override;
    void loop()  override;

private:
    IDataSource* sources_[MAX_SOURCES] = {};
    uint8_t      count_                = 0;
};
