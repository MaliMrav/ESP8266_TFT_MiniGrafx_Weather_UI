# Sprint Zeta — Domain Data and Source Architecture

## Objective

> **Separate domain identity from storage position, and separate data-source mechanisms from the products that happen to provide data.**

Sprint Epsilon established that a new Screen can introduce its own local context without changing the framework's interaction vocabulary.

Solar Energy exposed the equivalent pressure in the data layer.

The project now had:

```text
Weather
Solar Energy
```

with different measurements, while the original data model and source layout had grown around the original weather application.

The question became:

> **How can new application domains introduce their own observations and data sources without creating positional coupling or product-specific architecture?**

---

# Phase 1 — Introduce the Solar Domain

**Status: Complete**

The Solar Energy screen became a first-class application Screen.

Its domain includes:

- solar production
- household consumption
- battery storage
- grid export

`SolarScreen` consumes `SensorRepository` only.

It does not know whether those observations came from MQTT, an API, a local device, or another future source.

---

# Phase 2 — Per-Domain Sensor Identity

**Status: Complete**

## The Pressure

The original design used one flat `SensorIds.h` enum for every domain.

That worked while Weather was the only domain.

Once Solar was introduced, the code was vulnerable to assumptions such as:

```text
first five sensors = Weather
next sensors       = Solar
```

That is positional coupling disguised as named coupling.

## The Decision

Sensor identity is declared per domain:

```text
WeatherSensorIds.h
SolarSensorIds.h
```

Each domain owns its stable integer IDs.

`SensorRepository` remains a flat runtime store with fixed capacity, but it has no knowledge of domain boundaries.

Screens and data sources use explicit IDs rather than ranges.

The result is:

```text
Domain
   │
   └── owns sensor identity
             │
             ▼
       SensorRepository
```

Adding a new domain therefore does not require modifying unrelated domain identity.

---

# Phase 3 — Make MQTT a General Source Mechanism

**Status: Complete / current direction**

The original MQTT implementation was effectively documented as a Weather source.

Solar exposed why that was too narrow.

MQTT is a **source mechanism**, not a domain.

The intended model is:

```text
MqttDataSource
      │
      ├── Weather topic mappings
      └── Solar topic mappings
              │
              ▼
       SensorRepository
```

`MqttDataSource` does not know about Screens or domains. `TopicMappings` binds external MQTT topics to domain-owned sensor IDs.

A future domain can therefore consume MQTT without creating another MQTT implementation.

---

# Phase 4 — Source Architecture Must Describe Mechanisms, Not Products

**Status: Architectural decision / implementation follow-up**

The source tree had begun to tell the wrong story:

```text
src/
├── mqtt/
└── envoy/
```

This implied that Envoy was an architectural category alongside MQTT.

It is not.

The architecture is about **how data is obtained**:

```text
src/data/sources/
├── mqtt/
└── api/
```

Future mechanisms may include:

```text
serial/
modbus/
websocket/
```

The concrete external product is knowledge inside the source implementation or its mappings.

For example:

```text
API
 ├── Envoy
 ├── AlphaESS
 └── other provider
```

The source category remains `api`.

This is **protocol/mechanism, not product**.

---

# Phase 5 — Keep Sources Independent of Screens

The desired flow is:

```text
External System
      │
      ▼
Data Source
      │
      ▼
Domain Mapping
      │
      ▼
SensorRepository
      │
      ▼
Application Screen
```

For MQTT:

```text
MQTT broker
      │
      ▼
MqttDataSource
      │
      ▼
TopicMappings
      │
      ▼
Domain Sensor IDs
      │
      ▼
SensorRepository
      │
      ▼
Screen
```

For an API:

```text
External API
      │
      ▼
ApiDataSource
      │
      ▼
ApiMappings
      │
      ▼
Domain Sensor IDs
      │
      ▼
SensorRepository
      │
      ▼
Screen
```

The Screen does not change when the source mechanism changes.

---

# Phase 6 — Platform Constraints Influence Composition

The ESP8266 has a constrained runtime heap.

TLS-backed HTTPS can consume substantial memory and compete with the display framebuffer and MQTT stack.

That is a platform constraint, not a domain-model constraint.

The architecture therefore allows composition to select the source mechanism appropriate to the target platform:

```text
ESP8266
    └── MQTT path

ESP32-class hardware
    └── API/TLS path may be viable
```

The Solar domain remains unchanged.

The repository remains unchanged.

Only source composition changes.

---

# Phase 7 — Availability and Retained MQTT Data

MQTT is push-based.

A newly connected device may otherwise wait until the publisher sends its next update.

Retained MQTT messages allow the broker to deliver the last known value immediately to a new subscriber.

This avoids introducing a device-side cache merely to compensate for source timing.

The preferred boundary is:

> **The source should provide a current observation as early as reasonably possible.**

---

# Phase 8 — Source Tree Refactoring

**Status: Follow-up implementation**

The intended source tree is:

```text
src/
│
├── data/
│   ├── IDataSource.h
│   ├── DataSourceManager.h
│   ├── DataSourceManager.cpp
│   │
│   └── sources/
│       ├── mqtt/
│       │   ├── MqttDataSource.h
│       │   ├── MqttDataSource.cpp
│       │   ├── Topics.h
│       │   ├── TopicMappings.h
│       │   └── TopicMappings.cpp
│       │
│       └── api/
│           ├── ApiDataSource.h
│           ├── ApiDataSource.cpp
│           └── ApiMappings.h
│
├── models/
│   ├── SensorRepository.h
│   ├── SensorRepository.cpp
│   ├── SensorTile.h
│   ├── SensorCapacity.h
│   ├── WeatherSensorIds.h
│   └── SolarSensorIds.h
│
├── screens/
│   ├── WeatherScreen.*
│   ├── SolarScreen.*
│   └── ...
```

The important rule is:

> **`data/sources/` is organised by mechanism, not by product, device, screen, or domain.**

No `EnvoyDataSource` directory should become a permanent architectural category.

Likewise, development-only raw-value display diagnostics should not become part of the generic API source contract merely because they were useful on a device without convenient serial output.

---

# Definition of Done

Sprint Zeta is complete when:

- [x] Solar is an independent application domain.
- [x] Solar has explicit domain-owned sensor IDs.
- [x] Screens consume explicit IDs rather than positional ranges.
- [x] `SensorRepository` remains a shared flat runtime store.
- [x] MQTT is treated as a general source mechanism.
- [ ] Solar MQTT mappings use the real HA topics.
- [ ] API sources are organised by API mechanism rather than product name.
- [ ] The active source tree no longer defines product names as top-level source categories.
- [ ] The Data Layer reference describes MQTT and API mechanisms accurately.
- [ ] Platform-specific source composition is documented without coupling the domain model to one product.

---

# Architectural Lesson

The Solar work reinforced two related principles.

First:

> **Domain identity should not be positional.**

Second:

> **Source architecture should describe mechanisms, not products.**

Together they give us:

```text
Domain
   │
   ▼
Domain-owned identity
   │
   ▼
SensorRepository
   ▲
   │
Data-source mechanism
   │
   ├── MQTT
   ├── API
   ├── Serial
   └── Modbus
```

The Screen knows its domain.

The Repository knows its observations.

The Data Source knows how to obtain data.

The source does not become the domain.

The domain does not become the transport.

That separation is what allows Telemetry to grow beyond the weather station that started it.
