#include "MqttDataSource.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "../../../config/config.h"
#include "../../../models/SensorRepository.h"
#include "TopicMappings.h"

static WiFiClient   wifiClient;
static PubSubClient mqttClient(wifiClient);

static unsigned long lastReconnectAttempt = 0;
static bool mqttWasConnected = false;

// MQTT client identity is derived from the physical device.
//
// Every Telemetry device therefore gets a unique connection identity:
//
//     telemetry-9fbfb7
//     telemetry-8dc930
//
// Authentication remains shared through MQTT::USERNAME and MQTT::PASSWORD.
static String mqttClientId;

static TrendDirection parseTrend(const String& payload)
{
    String s = payload;
    s.trim();
    s.toLowerCase();

    if (s == "up")   return TREND_UP;
    if (s == "down") return TREND_DOWN;
    if (s == "flat") return TREND_FLAT;

    return TREND_NONE;
}

static void onMessage(
    char* topic,
    byte* payload,
    unsigned int length)
{
    String topicStr = String(topic);

    String payloadStr;
    payloadStr.reserve(length + 1);
    for (unsigned int i = 0; i < length; ++i)
    {
        payloadStr += static_cast<char>(payload[i]);
    }

    for (uint8_t i = 0; i < TOPIC_COUNT; ++i)
    {
        const TopicMapping& m = topicMappings[i];

        if (topicStr != m.topic)
        {
            continue;
        }

        switch (m.field)
        {
            case TopicMapping::VALUE:
                SensorRepository::setValue(
                    m.sensorId,
                    payloadStr.toFloat());
                break;

            case TopicMapping::MIN:
                SensorRepository::setMin(
                    m.sensorId,
                    payloadStr.toFloat());
                break;

            case TopicMapping::MAX:
                SensorRepository::setMax(
                    m.sensorId,
                    payloadStr.toFloat());
                break;

            case TopicMapping::TREND:
                SensorRepository::setTrend(
                    m.sensorId,
                    parseTrend(payloadStr));
                break;
        }

        break;
    }
}

static bool reconnect()
{
    if (!mqttClient.connect(
            mqttClientId.c_str(),
            MQTT::USERNAME,
            MQTT::PASSWORD))
    {
        return false;
    }

    for (uint8_t i = 0; i < TOPIC_COUNT; ++i)
    {
        mqttClient.subscribe(topicMappings[i].topic);
    }

    return true;
}

void MqttDataSource::begin()
{
    mqttClient.setServer(
        MQTT::SERVER,
        MQTT::PORT);

    mqttClient.setCallback(onMessage);
    mqttClient.setKeepAlive(MQTT::KEEPALIVE);

    mqttWasConnected = false;

    // Derive a unique MQTT client ID from the physical device.
    //
    // Example:
    //
    //     telemetry-9fbfb7
    //
    // This deliberately mirrors the device hostname identity.
    mqttClientId =
        String("telemetry-") +
        String(ESP.getChipId(), HEX);
}

void MqttDataSource::loop()
{
    const bool connected = mqttClient.connected();

    if (connected)
    {
        if (!mqttWasConnected)
        {
            Serial.println("[MQTT] Connected");
            mqttWasConnected = true;
        }

        mqttClient.loop();
        return;
    }

    if (mqttWasConnected)
    {
        Serial.println("[MQTT] Disconnected");
        mqttWasConnected = false;
    }

    const unsigned long now = millis();

    if (now - lastReconnectAttempt < MQTT::RECONNECT_MS)
    {
        return;
    }

    lastReconnectAttempt = now;

    if (reconnect())
    {
        lastReconnectAttempt = 0;
        mqttWasConnected = true;

        Serial.print("[MQTT] Connected as ");
        Serial.println(mqttClientId);
    }
}