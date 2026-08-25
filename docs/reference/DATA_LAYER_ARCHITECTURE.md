# Data Layer Architecture

## Overview

The data layer separates **what an observation means** from **how it is obtained**
and **where it is stored**.

```text
External / Local Source
        │
        ▼
   Source mechanism
        │
        ▼
 Source-specific mapping
        │
        ▼
   ObservationKey
        │
        ▼
 Runtime registration / resolution
        │
        ▼
 ObservationHandle
        │
        ▼
 SensorRepository storage
        │
        ▼
      Screen
```

A Screen consumes observations by identity, not by source.

## Shared Source Contract

`IDataSource` is the common contract for data providers. `DataSourceManager`
composes concrete sources and presents them as one data-source capability to
the application.

A single Screen may consume observations from several mechanisms at once:

```text
                     DataSourceManager
                            │
          ┌─────────────────┼─────────────────┐
          ▼                 ▼                 ▼
        MQTT              I²C               API
          │                 │                 │
     TopicMappings     Local mapping     ApiMappings
          │                 │                 │
          └─────────────────┼─────────────────┘
                            ▼
                    SensorRepository
```

## Source Mechanisms, Not Products

Data sources are organised by mechanism, not by product or vendor:

```text
src/data/sources/
├── mqtt/
└── api/
```

Future mechanisms may include `serial/`, `modbus/`, or `websocket/`. An API
source may currently communicate with Envoy, but the architectural capability
is `api`. The same mechanism may later communicate with AlphaESS or another
provider. Product-specific knowledge belongs inside source mappings, not in the
source tree's top-level architecture.

## Observation Identity

The old model used an integer as both domain identity and repository position.
That is no longer the architecture.

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

A stable, human-meaningful identity for an observation. Where an existing external
identity already provides a good semantic name, such as a Home Assistant entity
ID, Telemetry may use that identity directly.

Examples:

```text
sensor.bar_switch_panel_energy_power
sensor.envoy_current_power_production
room.temperature
solar.power.production
```

An `ObservationKey` is not a repository slot, runtime storage index, or
source-specific transport address.

### ObservationHandle

An opaque runtime reference allocated by the registration/resolution layer. The
application does not invent its value.

### Repository storage slot

An implementation detail of `SensorRepository`. The repository may use compact
fixed-capacity storage internally, but storage order must never form part of
domain identity.

> **Identity describes meaning. Storage describes implementation.**

> **Identity is stable; source resolution is dynamic.**

Adding an observation must never require renumbering another observation.

## Multiple Sources for One Observation

The same semantic observation may have several candidate sources:

```text
room.temperature
      │
      ├── local BME280
      ├── MQTT / Home Assistant
      └── Modbus controller
```

Source selection may eventually consider availability, priority, freshness,
health and explicit configuration. Round-robin is one possible policy where
several sources are genuinely equivalent; it is not part of identity.

## Multiple Sources on One Screen

A Screen may consume observations from several independent mechanisms. A Smart
Wall Panel could combine local I²C or One-Wire measurements, API weather data,
MQTT light state and MQTT/API solar data without changing the Screen's domain
model.

The Screen consumes semantic observations, not source-specific data.

## MQTT

`MqttDataSource` translates MQTT messages into observations through `TopicMappings`.
It can populate multiple domains through the same mechanism.

```text
MQTT topic
    │
    ▼
TopicMappings
    │
    ▼
ObservationKey
    │
    ▼
ObservationHandle
    │
    ▼
SensorRepository
```

## API

`ApiDataSource` provides the API source mechanism. `ApiMappings` contains the
provider-specific response mapping for the current implementation.

```text
api/
    ApiDataSource.*
    ApiMappings.h
```

The mechanism remains API even if the current provider is Envoy and a future
provider is AlphaESS.

## Platform Composition

PlatformIO environments compose the framework for a particular target. The data
architecture remains the same, but the environment decides which source
capabilities are instantiated.

```text
ESP8266 profile
    └── MQTT + local I/O

ESP32 profile
    └── MQTT + API/TLS + local I/O + future control capabilities
```

The ESP8266 is a constrained reference profile, not the architectural ceiling.

> **Telemetry defines the architecture. Composition determines which capabilities a target can instantiate.**

## Retained MQTT Data

MQTT is push-based. Retained messages allow a newly connected subscriber to receive
the latest published value immediately. This avoids device-side caching merely to
compensate for source timing.

## Future Commands

Interactive applications may eventually publish commands such as:

```text
livingroom.lamp.set
hvac.target_temperature.set
blind.position.set
```

Command identity is intentionally outside the current ObservationKey contract. If
command architecture becomes substantial enough, it should be designed explicitly
in a future sprint.

## Adding a New Source Mechanism

A new source mechanism belongs under `src/data/sources/<mechanism>/` and implements
`IDataSource`. It should map external data into existing semantic observation
identities without requiring Screen changes.

## Data-Layer Boundary

```text
Domain
    └── defines ObservationKey

Source mechanism
    └── obtains observations

Resolution / registration
    └── maps key to runtime handle

Repository
    └── stores runtime state
```

This boundary keeps Telemetry reusable across domains, sources and target platforms.
