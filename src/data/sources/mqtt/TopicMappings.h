#pragma once

#include <Arduino.h>

#include "../../../data/ObservationKey.h"
#include "../../../models/WeatherObservationKeys.h"
#include "../../../models/SolarObservationKeys.h"

#include "Topics.h"

// TopicMapping binds an MQTT transport topic to a semantic observation.
//
// MqttDataSource resolves the ObservationKey through ObservationRegistry
// before writing to SensorRepository.
//
// This boundary deliberately knows about:
//     - MQTT transport
//     - semantic observation identity
//
// It does not know about:
//     - repository storage
//     - storage slots
//     - ObservationHandle representation
//     - screen implementation

struct TopicMapping
{
    const char* topic;
    ObservationKey observation;

    enum Field
    {
        VALUE,
        MIN,
        MAX,
        TREND
    } field;
};

extern const TopicMapping topicMappings[];
extern const uint8_t TOPIC_COUNT;