# Data Layer Architecture

## Overview

The data layer separates **where observations come from** from **how the application consumes them**.

A data source provides observations through the common `IDataSource` contract.

`SensorRepository` is the shared runtime store.

Screens consume the repository.

No Screen needs to know whether its observations came from MQTT, an API, a local sensor, serial, Modbus, or another future source.

---

## Flow

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

Multiple sources may coexist:

```text
                     DataSourceManager
                            │
              ┌─────────────┴─────────────┐
              ▼                           ▼
        MqttDataSource              ApiDataSource
              │                           │
         TopicMappings               ApiMappings
              │                           │
              └─────────────┬─────────────┘
                            ▼
                    SensorRepository
                            │
                  ┌─────────┴─────────┐
                  ▼                   ▼
           WeatherScreen        SolarScreen
```

---

## IDataSource

`IDataSource` is the contract every data provider implements.

Its job is limited to source lifecycle and observation delivery.

The application does not need to know which concrete source is behind the contract.

---

## DataSourceManager

`DataSourceManager` composes multiple `IDataSource` implementations into one source capability.

The composition root decides which sources are active for a particular build or device.

The rest of the application does not need to know.

---

## Source Mechanisms

Sources are organised by **mechanism**, not by product.

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

A product or external system is not itself the architectural source category.

For example:

```text
API
 ├── Envoy
 ├── AlphaESS
 └── another provider
```

The API source mechanism remains the boundary.

---

## MQTT Data Source

`MqttDataSource` consumes MQTT messages and maps them to domain-owned sensor IDs.

It knows:

- MQTT connection details
- subscriptions
- payload parsing
- topic mappings

It does not know:

- WeatherScreen
- SolarScreen
- screen layout
- UI behaviour

The same MQTT source can populate multiple domains:

```text
MqttDataSource
    │
    ├── Weather topic mappings
    └── Solar topic mappings
```

`TopicMappings` is the translation table between external MQTT topics and domain-owned sensor IDs.

---

## API Data Source

`ApiDataSource` provides the API-backed source mechanism.

Its responsibility is communication with an external API and conversion of API responses into observations.

The current provider-specific endpoint knowledge belongs in the API mapping layer rather than in the source tree's top-level architecture.

An API-backed implementation might talk to Envoy today and AlphaESS tomorrow without changing the domain model or screen architecture.

---

## Domain Sensor Identity

Sensor identity belongs to the domain.

Current domain contracts include:

```text
WeatherSensorIds.h
SolarSensorIds.h
```

Each declares stable integer IDs.

`SensorRepository` remains a flat indexed store.

The important relationship is:

```text
Domain
   │
   └── owns sensor identity
             │
             ▼
       SensorRepository
             ▲
             │
       Data sources
```

This removes positional coupling.

A Screen or source must use explicit IDs rather than assuming that a domain occupies a range of storage slots.

---

## Solar Data

Solar is a domain, not a data source.

The current Solar domain includes:

```text
Production
Consumption
Export
Battery
```

with current and today's values.

Those observations may be supplied by MQTT or an API.

The Solar Screen does not change when the source mechanism changes.

---

## Platform Constraints

The ESP8266 has a constrained runtime heap.

TLS-backed HTTPS can consume substantial memory and compete with the display framebuffer and MQTT stack.

This is a platform constraint, not a Solar-domain constraint.

Source composition can therefore differ by target hardware without changing the domain or repository model.

---

## Retained MQTT Data

MQTT is push-based.

A newly connected device may otherwise wait until the publisher sends its next update.

Retained messages allow the broker to deliver the last known value immediately to a new subscriber.

This avoids introducing a device-side cache merely to compensate for source timing.

> **The source should provide a current observation as early as reasonably possible.**

---

## Adding a New Domain

A new domain should be able to introduce:

```text
NewDomainSensorIds.h
NewDomainScreen
optional MQTT mappings
optional API mappings
```

without modifying unrelated domains.

The domain owns its identity.

The repository provides storage.

The source mechanism provides observations.

The Screen presents them.

---

## Adding a New Source Mechanism

A future source should follow:

```text
src/data/sources/<mechanism>/
```

and implement `IDataSource`.

Examples:

```text
mqtt/
api/
serial/
modbus/
```

A new source mechanism should not require changes to existing Screens merely because it supplies the same domain observations through a different transport.

---

## Architectural Principles

- **One Responsibility Per Capability** — sources obtain/transport observations; the repository stores them; Screens present them.
- **Source Independence** — domains do not depend on a particular transport or product.
- **Domain-Owned Identity** — sensor IDs live with the domain that owns the observation.
- **No Positional Coupling** — Screens and sources use explicit IDs rather than array ranges.
- **Composition Over Knowledge** — `DataSourceManager` composes sources without making the application depend on concrete implementations.
