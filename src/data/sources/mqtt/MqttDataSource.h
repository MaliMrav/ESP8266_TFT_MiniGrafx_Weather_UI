#pragma once

#include "../../IDataSource.h"

// MqttDataSource is the MQTT implementation of IDataSource.
//
// Responsibilities:
//   - connect to the MQTT broker
//   - subscribe to configured topics (TopicMappings)
//   - parse incoming payloads and write to SensorRepository
//   - reconnect automatically if the connection drops
//
// It has no knowledge of WiFi, display, or screens.

class MqttDataSource : public IDataSource
{
public:
    void begin() override;
    void loop()  override;
};
