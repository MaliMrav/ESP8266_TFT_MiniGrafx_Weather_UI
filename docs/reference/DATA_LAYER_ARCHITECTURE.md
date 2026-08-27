# Data Layer Architecture

## Overview

The data layer separates **what the application knows** from **how observations are obtained** and **where they are stored**.

A data source implements the common `IDataSource` contract.

`DataSourceManager` composes multiple sources.

`SensorRepository` stores observations for the application.

Screens consume observations without knowing which source supplied them.

The architecture is deliberately independent of vendor, device, transport, and target platform.

---

## The Data Flow

```text
External / Local Source
          │
          ▼
     Data Source
          │
          ▼
   Source Mapping
          │
          ▼
   Observation Identity
          │
          ▼
   SensorRepository
          │
          ▼
      Application
          │
          ▼
        Screen
```

A single application context may consume observations from multiple sources:

```text
                     DataSourceManager
                            │
          ┌─────────────────┼─────────────────┐
          ▼                 ▼                 ▼
        MQTT              I²C               API
          │                 │                 │
     topic mapping      local mapping      API mapping
          │                 │                 │
          └─────────────────┼─────────────────┘
                            ▼
                    SensorRepository
                            │
                            ▼
                       Active Screen
```

The same observation may also have multiple candidate sources. Source selection is a separate resolution concern; it is not part of observation identity.

---

## `IDataSource`

`IDataSource` is the common source contract.

A source is responsible for obtaining observations and making them available to the application data layer.

It does not own Screen behaviour and does not dictate where an observation is presented.

---

## `DataSourceManager`

`DataSourceManager` composes multiple `IDataSource` implementations into one source capability.

The composition root decides which sources are active for a particular PlatformIO environment.

Conceptually:

```cpp
DataSourceManager sources;
sources.add(mqttData);
sources.add(apiData);
```

The application does not need to know which or how many source implementations are behind the manager.

---

## Source Mechanisms

Sources are organised by **mechanism**, not by product, vendor, Screen, or domain.

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

A product is an implementation concern inside a mechanism.

For example:

```text
API
 ├── Envoy
 ├── AlphaESS
 └── another provider
```

The source tree should describe the mechanism, not become a catalogue of devices.

---

## MQTT

`MqttDataSource` is the MQTT implementation of `IDataSource`.

It may provide observations for multiple domains through the same mechanism.

```text
MqttDataSource
    │
    ├── Weather mappings
    ├── Solar mappings
    └── future domain mappings
```

`TopicMappings` translates MQTT topic vocabulary into the framework's observation identities.

`MqttDataSource` does not know which Screen consumes those observations.

---

## API

`ApiDataSource` is the API implementation of `IDataSource`.

The mechanism is API communication. A particular provider, such as Envoy or AlphaESS, is external-system knowledge contained by the API implementation and its mappings.

The application therefore does not become architecturally dependent on one energy product merely because one API implementation currently targets it.

On a constrained ESP8266 composition, the API capability may simply not be instantiated.

On an ESP32/CYD composition, API/TLS may be instantiated where the target's resources and application requirements justify it.

---

## Domain Observation Identity

Domain identity is separate from repository storage.

The target model is:

```text
ObservationKey
      │
      ▼
Source resolution
      │
      ▼
ObservationHandle
      │
      ▼
Repository storage slot
```

### ObservationKey

A stable, human-readable identity for an observation.

Where an existing external identity already expresses the meaning well, such as a Home Assistant entity ID, the framework may use that identity directly.

Examples include:

```text
sensor.envoy_current_power_production
sensor.bar_switch_panel_energy_power
sensor.bar_switch_panel_energy_voltage
```

The key is not a storage index and must not encode repository ordering.

### ObservationHandle

An opaque runtime reference allocated by the framework.

Application code does not invent its value.

### Repository storage

Physical storage location is an implementation detail of `SensorRepository`.

The repository is free to use compact embedded storage without exposing storage position as domain identity.

The governing principle is:

> **Identity describes meaning. Storage describes implementation.**

---

## Multiple Sources on One Screen

A Screen is not tied to one source.

A Smart Wall Panel could legitimately consume:

```text
Date / Time              → local / network
Weather forecast         → API
Room temperature         → I²C
Room humidity            → I²C
Air pressure             → I²C
Solar production         → MQTT / API
Light state              → MQTT
HVAC state               → Modbus / MQTT
```

The Screen consumes the observations. The source mechanism remains behind the data boundary.

This allows the same Screen architecture to be used with different target compositions.

---

## Platform Composition

PlatformIO environments select the capabilities used by a build.

The framework remains one architecture.

```text
Telemetry
    │
    ├── ESP8266 composition
    │     └── observation-oriented
    │
    └── ESP32/CYD composition
          └── observation + control
```

The current reference profiles are:

### ESP8266

Observation-oriented and resource-constrained.

The lightweight remote data path is MQTT, with local I/O capabilities as appropriate.

### ESP32/CYD

Observation-and-control-oriented and resource-expanded.

This profile may instantiate API/TLS, richer presentation, multiple source mechanisms, and future command/control capabilities.

A capability that is unavailable or unnecessary on ESP8266 is not implemented as a framework workaround. It is simply omitted from that composition.

> **Telemetry defines the architecture. Composition determines which capabilities a target can instantiate.**

---

## Availability and Retained MQTT Data

MQTT is push-based.

Retained messages allow a newly connected subscriber to receive the latest published value without waiting for the next change.

The architectural preference is:

> **The source should provide current observations as early as reasonably possible.**

Device-side caching should not be added merely to compensate for a source protocol that already provides an appropriate current-value mechanism.

---

## Adding a New Domain

A new domain should be able to introduce its observation identities and mappings without modifying unrelated domains.

Conceptually:

```text
NewDomain
    │
    ├── domain observation definitions
    ├── optional MQTT mappings
    ├── optional API mappings
    └── Screen
```

The domain owns meaning.

The source owns acquisition.

The repository owns runtime storage.

The Screen owns presentation and contextual interpretation.

---

## Adding a New Source Mechanism

A new source mechanism belongs under:

```text
src/data/sources/<mechanism>/
```

and implements `IDataSource`.

Examples:

```text
mqtt/
api/
serial/
modbus/
```

The mechanism should not be named after the product it happens to access.

---

## Architectural Principles Applied

- **Single Responsibility** — sources obtain observations; mappings translate external vocabulary; the repository stores runtime state; Screens present and interpret it.
- **Source Independence** — Screens do not depend on MQTT, API, Envoy, AlphaESS, or another provider.
- **Domain-Owned Identity** — semantic observation identity is independent of physical storage.
- **No Positional Coupling** — domain identity never depends on array order or storage slot.
- **Multiple Sources** — one Screen or one observation may legitimately involve multiple source mechanisms.
- **Composition by Target** — PlatformIO chooses which framework capabilities a target instantiates.
