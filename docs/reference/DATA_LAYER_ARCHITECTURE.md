# Data Layer Architecture

## Overview

The data layer decouples data providers from the repository and UI. Any data source — MQTT, HTTP, local I2C sensor — implements the same `IDataSource` interface and writes through the same repository API. Screens are never aware of where data came from, or how many sources are running.

---

## Flow

```
┌─────────────────────┐   ┌─────────────────────┐
│   MqttDataSource    │   │  EnvoyDataSource     │
│  (weather sensors)  │   │  (solar sensors)     │
│  push — broker msg  │   │  pull — HTTP poll    │
└──────────┬──────────┘   └──────────┬───────────┘
           │                         │
           └────────────┬────────────┘
                        │ both implement IDataSource
                        ▼
           ┌────────────────────────┐
           │   DataSourceManager    │  implements IDataSource
           │   delegates begin/loop │  SystemManager sees only this
           └────────────┬───────────┘
                        │ writes via typed API
                        ▼
           ┌────────────────────────────────┐
           │        SensorRepository        │
           │  setValue(uint8_t id, float)   │
           └────────────┬───────────────────┘
                        │ read by
              ┌─────────┴──────────┐
              ▼                    ▼
     ┌────────────────┐   ┌────────────────┐
     │ WeatherScreen  │   │  SolarScreen   │
     └────────────────┘   └────────────────┘
```

---

## Components

### IDataSource — `src/data/IDataSource.h`

The interface every data provider implements. Two methods: `begin()` and `loop()`. `SystemManager` holds a single `IDataSource&` — it never knows what kind of source, or how many, are behind it.

### DataSourceManager — `src/data/DataSourceManager.h`

Composes multiple `IDataSource` implementations into one. Implements `IDataSource` itself, so `SystemManager` requires no changes when sources are added or removed. Sources are registered in `main.cpp` before `SystemManager::begin()` is called.

```cpp
DataSourceManager dataSources;
dataSources.add(mqttData);
dataSources.add(envoyData);
SystemManager::begin(..., dataSources);
```

### MqttDataSource — `src/mqtt/MqttDataSource.h`

Covers weather sensors. Connects via PubSubClient, subscribes to topics defined in `TopicMappings`, parses payloads, and writes to `SensorRepository`. Reconnects automatically. Has no knowledge of WiFi, display, screens, or the Envoy.

### EnvoyDataSource — `src/envoy/EnvoyDataSource.h`

Covers solar/energy sensors. Polls two Enphase Envoy local API endpoints on a configurable interval (`CFG_ENVOY_POLL_MS`, default 10 s). No authentication required. No cloud dependency. No HA dependency.

| Endpoint | Data |
|---|---|
| `GET /ivp/meters/readings` | Real-time power (W) — production CT `[0]`, consumption CT `[1]` |
| `GET /api/v1/production` | Today's energy totals (Wh) — production and consumption |

Writes to `SensorRepository` using sensor IDs from `SolarSensorIds.h`. Has no knowledge of MQTT, display, or screens.

### SensorRepository — `src/models/SensorRepository.h`

The single source of truth for all sensor data. A flat indexed store keyed by `uint8_t` sensor IDs declared per-domain. Exposes a typed write API — data sources never touch internal array indices directly.

### TopicMappings — `src/mqtt/TopicMappings.h`

A flat table binding MQTT topic strings to sensor IDs and field types (`VALUE`, `MIN`, `MAX`, `TREND`). Covers weather sensors only. Solar data has no MQTT entries — it is sourced exclusively from the Envoy local API.

---

## Why Two Sources?

The weather sensors (Kitchen, Pergola) publish to an MQTT broker. The Envoy solar gateway exposes a local HTTP API. These are different transports serving different domains.

The alternative — routing Envoy data through HA and then through MQTT — would introduce two unnecessary dependencies: HA must be running, and manual MQTT topics must be maintained. The Envoy local API is available on the LAN with no intermediary.

`DataSourceManager` allows both transports to coexist without either knowing about the other, and without `SystemManager` knowing how many sources exist.

---

## Adding a New Data Source

1. Create a class implementing `IDataSource` in an appropriate directory.
2. In `begin()`, establish any persistent connection.
3. In `loop()`, check a poll interval (if pull-based) or handle incoming data (if push-based), then write to `SensorRepository` by sensor ID.
4. In `main.cpp`, instantiate the source and call `dataSources.add(yourSource)`.

No changes to `SystemManager`, `SensorRepository`, or any screen.

```cpp
// Example: a future local I2C temperature sensor
class I2CSensorSource : public IDataSource
{
public:
    void begin() override { sensor_.begin(); }
    void loop()  override
    {
        if (millis() - last_ < 5000) return;
        last_ = millis();
        SensorRepository::setValue(SENSOR_ROOM_TEMP, sensor_.readTemperature());
    }
private:
    SomeI2CSensor sensor_;
    unsigned long last_ = 0;
};
```

---

## Adding a New MQTT Sensor

Only three files change:

1. Add a `constexpr uint8_t` ID to the appropriate domain header (e.g. `WeatherSensorIds.h`).
2. Add its display tile to `SensorRepository.cpp` using a designated initialiser.
3. Add its topic rows to `TopicMappings.cpp`.

`MqttDataSource` itself does not change.

---

## Adding a New Envoy Field

Only two files change:

1. Add a `constexpr uint8_t` ID to `SolarSensorIds.h`.
2. Add its display tile to `SensorRepository.cpp`.
3. Add its extraction in `EnvoyDataSource.cpp` (`fetchMeters()` or `fetchProduction()`).

---

## Design Principles Applied

- **Single Responsibility** — `MqttDataSource` knows MQTT; `EnvoyDataSource` knows the Envoy API; `SensorRepository` owns storage; screens own display.
- **Open/Closed** — extend with new data sources without modifying existing code.
- **Dependency Inversion** — `SystemManager` depends on `IDataSource`, not any concrete implementation.
- **Source independence** — sensors do not need to be registered in HA or any broker before the UI can consume them.
