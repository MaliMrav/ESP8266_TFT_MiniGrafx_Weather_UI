#pragma once

#include <Arduino.h>

#include "../../../models/WeatherSensorIds.h"
#include "../../../models/SolarObservationKeys.h"
#include "Topics.h"

// TopicMapping binds an MQTT topic string to a domain-owned sensor ID and
// field type. MqttDataSource iterates this table on incoming messages.
//
// The mapping table is the boundary between external MQTT topics and the
// repository's flat runtime store. Domain identity remains in the sensor-ID
// headers; MqttDataSource does not know anything about Weather or Solar.

struct TopicMapping
{
    const char* topic;
    uint8_t     sensorId;

    enum Field { VALUE, MIN, MAX, TREND } field;
};

extern const TopicMapping topicMappings[];
extern const uint8_t      TOPIC_COUNT;
