#include "ApiDataSource.h"

#include "../../../config/config.h"
#include "ApiMappings.h"

#include "../../../data/ObservationRegistry.h"
#include "../../../models/SensorRepository.h"
#include "../../../models/EnergyObservationKeys.h"

#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

static String authHeader()
{
    return String("Bearer ") + ApiConfig::TOKEN;
}

void ApiDataSource::begin()
{
    // No persistent connection — each poll opens and closes its own request.
}

void ApiDataSource::loop()
{
    if (millis() - lastPoll_ < ApiConfig::POLL_MS)
    {
        return;
    }

    lastPoll_ = millis();

    if (fetchRealtimeNext_)
    {
        fetchRealtime();
    }
    else
    {
        fetchDailyTotals();
    }

    fetchRealtimeNext_ = !fetchRealtimeNext_;
}

void ApiDataSource::fetchRealtime()
{
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;

    String url =
        String("https://") +
        ApiConfig::HOST +
        ApiMappings::REALTIME_PATH;

    if (!http.begin(client, url))
    {
        Serial.println("[API] request begin failed");
        return;
    }

    http.addHeader("Authorization", authHeader());

    int code = http.GET();

    if (code != HTTP_CODE_OK)
    {
        Serial.printf("[API] realtime HTTP %d\n", code);
        http.end();
        return;
    }

    StaticJsonDocument<2048> doc;

    DeserializationError err =
        deserializeJson(doc, http.getStream());

    http.end();

    if (err)
    {
        Serial.printf(
            "[API] realtime JSON error: %s\n",
            err.c_str());

        return;
    }

    const float production =
        doc[ApiMappings::PRODUCTION_METER_INDEX]
           [ApiMappings::ACTIVE_POWER_FIELD]
           | 0.0f;

    const float grid =
        doc[ApiMappings::GRID_METER_INDEX]
           [ApiMappings::ACTIVE_POWER_FIELD]
           | 0.0f;

    const ObservationHandle productionHandle =
        ObservationRegistry::resolve(
            EnergyObservations::CURRENT_POWER_PRODUCTION);

    if (!productionHandle.isValid())
    {
        Serial.println(
            "[API] Solar production observation is not registered");

        return;
    }

    SensorRepository::setValue(
        productionHandle,
        production);

    // The current API exposes net grid import as the second meter.
    // Household consumption is derived as production + net import.

    const ObservationHandle consumptionHandle =
        ObservationRegistry::resolve(
            EnergyObservations::CURRENT_POWER_CONSUMPTION);

    if (!consumptionHandle.isValid())
    {
        Serial.println(
            "[API] Solar consumption observation is not registered");

        return;
    }

    SensorRepository::setValue(
        consumptionHandle,
        production + grid);

    // Export and battery are intentionally not populated here until the
    // configured API provides corresponding observations.
}

void ApiDataSource::fetchDailyTotals()
{
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;

    String url =
        String("https://") +
        ApiConfig::HOST +
        ApiMappings::DAILY_TOTALS_PATH;

    if (!http.begin(client, url))
    {
        Serial.println("[API] request begin failed");
        return;
    }

    http.addHeader("Authorization", authHeader());

    int code = http.GET();

    if (code != HTTP_CODE_OK)
    {
        Serial.printf("[API] daily totals HTTP %d\n", code);
        http.end();
        return;
    }

    StaticJsonDocument<512> doc;

    DeserializationError err =
        deserializeJson(doc, http.getStream());

    http.end();

    if (err)
    {
        Serial.printf(
            "[API] daily totals JSON error: %s\n",
            err.c_str());

        return;
    }

    if (doc["production"]["wattHoursToday"].is<float>())
    {
        const ObservationHandle productionHandle =
            ObservationRegistry::resolve(
                EnergyObservations::ENERGY_PRODUCTION_TODAY);

        if (!productionHandle.isValid())
        {
            Serial.println(
                "[API] Solar production-today observation is not registered");
        }
        else
        {
            SensorRepository::setValue(
                productionHandle,
                doc["production"]["wattHoursToday"].as<float>());
        }
    }

    if (doc["consumption"]["wattHoursToday"].is<float>())
    {
        const ObservationHandle consumptionHandle =
            ObservationRegistry::resolve(
                EnergyObservations::ENERGY_CONSUMPTION_TODAY);

        if (!consumptionHandle.isValid())
        {
            Serial.println(
                "[API] Solar consumption-today observation is not registered");
        }
        else
        {
            SensorRepository::setValue(
                consumptionHandle,
                doc["consumption"]["wattHoursToday"].as<float>());
        }
    }

    // Export and battery are intentionally not populated here until the
    // configured API provides corresponding observations.
}