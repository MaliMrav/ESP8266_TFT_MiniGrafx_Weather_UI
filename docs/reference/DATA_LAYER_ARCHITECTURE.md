# Data Layer Architecture

## Overview

The data layer separates **where observations come from** from **what the application knows about them** and **how they are presented**.

Any data provider implements `IDataSource` and writes observations through `SensorRepository`. Screens do not know which source supplied an observation.

The source tree is organised by **source mechanism**, not by product, device, screen, or domain.

```text
src/data/
├── IDataSource.h
├── DataSourceManager.h
├── DataSourceManager.cpp
└── sources/
    ├── mqtt/
    └── api/
```

Weather and Solar are domains. MQTT and API are source mechanisms.

---

## Flow

```text
┌─────────────────────┐   ┌─────────────────────┐
│   MqttDataSource    │   │    ApiDataSource    │
│    MQTT source      │   │     API source      │
│  push — broker msg  │   │  pull — API poll    │
└──────────┬──────────┘   └──────────┬──────────┘
           │                         │
           └────────────┬────────────┘
                        │ both implement IDataSource
                        ▼
           ┌────────────────────────┐
           │   DataSourceManager    │
           │  delegates begin/loop │
           └────────────┬───────────┘
                        │
                        ▼
           ┌────────────────────────────────┐
           │        SensorRepository        │
           │     flat runtime sensor store  │
           └────────────┬───────────────────┘
                        │
              ┌─────────┴──────────┐
              ▼                    ▼
     ┌────────────────┐   ┌────────────────┐
     │ WeatherScreen  │   │  SolarScreen   │
     └────────────────┘   └────────────────┘
```

---

## IDataSource

`src/data/IDataSource.h`

`IDataSource` is the contract every data provider implements.

It exposes only:

```cpp
begin()
loop()
```

The source establishes its own connection, polls or receives data, and writes observations to `SensorRepository`.

It has no knowledge of screens or presentation.

Examples include:

- MQTT
- API
- Serial
- Modbus
- local sensor interfaces

---

## DataSourceManager

`src/data/DataSourceManager.h`

`DataSourceManager` composes multiple `IDataSource` implementations into one source.

```cpp
DataSourceManager dataSources;

dataSources.add(mqttData);
dataSources.add(apiData);
```

`SystemManager` therefore depends only on `IDataSource`.

Adding or removing a concrete source does not require `SystemManager` to understand that source.

---

## MqttDataSource

`src/data/sources/mqtt/MqttDataSource.h`

`MqttDataSource` implements the MQTT source mechanism.

It:

- connects to the MQTT broker
- subscribes to the configured topics
- parses incoming payloads
- maps payloads to domain-owned sensor IDs
- writes observations to `SensorRepository`
- reconnects when the broker connection is lost

It does not know what Weather or Solar means to the UI.

The topic-to-sensor mapping is defined by `TopicMappings`.

One MQTT source may provide observations for multiple domains.

---

## ApiDataSource

`src/data/sources/api/ApiDataSource.h`

`ApiDataSource` implements the API source mechanism.

It:

- connects to the configured API
- retrieves configured API resources
- extracts mapped observations
- writes those observations to `SensorRepository`

Provider-specific endpoint paths and response-field locations are isolated in `ApiMappings.h`.

The API source therefore describes **how to communicate with an API**, while the mapping describes the current external API contract.

A future API provider can replace or extend the mapping without changing the domain sensor IDs or screen contracts.

---

## SensorRepository

`src/models/SensorRepository.h`

`SensorRepository` is the runtime store for observations.

It remains a flat store indexed by `uint8_t` sensor IDs.

Domain ownership is not expressed by array position.

Instead:

```text
models/
├── WeatherSensorIds.h
└── SolarSensorIds.h
```

declare the IDs belonging to each domain.

A screen references the explicit IDs it needs.

A data source writes to the same explicit IDs.

No source or screen relies on ranges such as "the first five sensors belong to Weather".

---

## TopicMappings

`src/data/sources/mqtt/TopicMappings.h`

`TopicMappings` binds MQTT topics to domain-owned sensor IDs and field types.

For example:

```text
MQTT topic
    │
    ▼
TopicMapping
    ├── topic
    ├── sensorId
    └── field
            │
            ▼
    SensorRepository
```

The mapping table can contain Weather and Solar entries together.

`MqttDataSource` does not need to change when a new domain is added; only the mapping table and the appropriate domain sensor IDs need to change.

---

## ApiMappings

`src/data/sources/api/ApiMappings.h`

`ApiMappings` isolates the current provider's API resources and response fields from the API source mechanism.

This is deliberately a mapping boundary rather than a new domain abstraction.

The current API mapping supplies the Solar observations required by the project.

If the external API changes, its endpoint and field mapping can change without changing:

- `SensorRepository`
- `SolarSensorIds.h`
- `SolarScreen`
- `IDataSource`

---

## Why the Source Tree Is Organised This Way

The source tree deliberately separates three different concepts:

```text
Source mechanism
    MQTT
    API
    Serial
    Modbus
        │
        ▼
Observation
        │
        ▼
Domain identity
    Weather
    Solar
        │
        ▼
SensorRepository
        │
        ▼
Presentation
    WeatherScreen
    SolarScreen
```

The source mechanism answers:

> **How did we obtain the observation?**

The domain identity answers:

> **What observation is this?**

The screen answers:

> **How should this information be presented and interpreted?**

Those responsibilities should not be collapsed into a single class or directory.

---

## Adding a New Data Source

1. Create an `IDataSource` implementation under the appropriate source-mechanism directory.
2. Establish or poll the source in `begin()` / `loop()`.
3. Write observations through `SensorRepository` using domain-owned sensor IDs.
4. Register the source in `main.cpp` through `DataSourceManager`.

No screen changes are required merely because a different source supplies the data.

---

## Adding a New Sensor to an Existing Source

For MQTT:

1. Add the domain-owned sensor ID.
2. Add its repository tile/configuration.
3. Add its MQTT topic mapping.

`MqttDataSource` itself remains unchanged.

For an API-backed observation:

1. Add the domain-owned sensor ID.
2. Add its repository tile/configuration.
3. Add or update the appropriate API mapping/extraction.

`ApiDataSource` itself should remain a source-mechanism implementation rather than becoming domain-specific.

---

## Design Principles Applied

- **One Source of Truth** — `SensorRepository` is the runtime store.
- **Separation of Responsibilities** — source mechanisms, domain identity, and presentation remain separate.
- **Open for Extension** — new source mechanisms and domains can be added without restructuring existing screens.
- **Dependency Inversion** — `SystemManager` consumes `IDataSource`, not concrete sources.
- **No Positional Coupling** — screens and sources use explicit domain-owned sensor IDs.
- **Protocol, Not Product** — the source tree describes MQTT/API/Serial/Modbus capabilities, not Enphase/AlphaESS or individual devices.
