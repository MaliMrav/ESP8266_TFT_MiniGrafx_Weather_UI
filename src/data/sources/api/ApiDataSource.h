#pragma once

// ApiDataSource is the API implementation of IDataSource.
//
// Responsibilities:
//   - establish communication with the configured API
//   - retrieve configured API resources
//   - apply the field mapping defined by ApiMappings
//   - write observations to SensorRepository
//
// The source knows how to communicate with an API.
// It has no knowledge of screens or presentation.
//
// Provider-specific endpoint and field details are isolated in
// ApiMappings.h.

#include "../../../data/IDataSource.h"

class ApiDataSource : public IDataSource
{
public:
    void begin() override;
    void loop()  override;

private:
    unsigned long lastPoll_ = 0;
    bool fetchRealtimeNext_ = true;

    void fetchRealtime();
    void fetchDailyTotals();
};
