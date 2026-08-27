# Sprint Zeta — Domain Data and Source Architecture

## Objective

> **Separate what an observation means from where it is stored, while allowing multiple independent data sources to contribute to the same application context and allowing each target to instantiate an appropriate capability composition.**

Sprint Epsilon established that a Screen is a contextual interpreter of interaction.

Solar Energy exposed the equivalent pressure in the data layer.

The project now has multiple application domains, multiple source mechanisms, and multiple possible target platforms.

The architectural pressure is therefore no longer simply:

> "How do we stop Weather and Solar from sharing accidental sensor ranges?"

It is:

> **How do we build one framework in which domain identity, source mechanism, runtime storage, and target capability composition remain independent?**

---

# Architectural Decisions Frozen by This Sprint

Sprint Zeta establishes these principles:

> **Identity describes meaning. Storage describes implementation.**

> **Identity is stable; source resolution is dynamic.**

> **Sources are mechanisms, not products.**

> **A Screen consumes observations by identity, not by source.**

> **Multiple source mechanisms may contribute observations to the same Screen or observation.**

> **Telemetry defines the architecture. Composition determines which capabilities a target can instantiate.**

> **PlatformIO environments instantiate platform-specific capability compositions.**

> **ESP8266 is the constrained observation reference profile.**

> **ESP32/CYD is the expanded observation-and-control reference profile.**

Capabilities that are unnecessary or unsuitable for the ESP8266 are not worked around. They are simply not instantiated by the ESP8266 composition.

---

# The Architectural Pressure

The original Sensor model used small integer constants directly as repository indices.

Even after moving constants into per-domain headers, the integer remained both the identity of an observation and its physical storage location.

That creates a hidden coupling.

The Smart Wall Panel thought experiment exposed the deeper problem.

A single Screen may consume observations from several domains and several source mechanisms:

```text
room.temperature
room.humidity
room.pressure
weather.forecast
solar.power.production
livingroom.lamp.state
hvac.target_temperature
```

while obtaining them from:

```text
MQTT
I²C
One-Wire
API
Modbus
```

The architecture must therefore stop assuming:

```text
one Screen
    =
one domain
    =
one source
```

It must also stop treating repository storage location as semantic identity.

---

# The Course Correction

The earlier idea of stable integer IDs per domain is superseded.

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

These are deliberately different concepts.

## ObservationKey

A stable, human-readable semantic identity.

Examples may include existing external identities such as Home Assistant entity IDs:

```text
sensor.bar_switch_panel_energy_power
sensor.envoy_current_power_production
sensor.envoy_energy_production_today
```

The framework does not require a developer to invent a meaningless numeric identifier.

The key describes what the observation means.

## ObservationHandle

An opaque runtime reference allocated by the framework.

The developer does not invent or remember its numeric value.

## Repository Storage Slot

A private implementation detail.

The repository may use compact fixed-capacity embedded storage while keeping storage order completely independent of domain identity.

---

# Phase 1 — Introduce the Solar Domain

**Status: Complete**

Solar Energy became a first-class application domain.

Its Screen consumes Solar observations through `SensorRepository` and does not depend on the source that provides them.

---

# Phase 2 — Separate Source Mechanism from Product

**Status: Complete**

Sources are organised by mechanism rather than vendor or product.

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

An API implementation may target Envoy today and AlphaESS tomorrow without making either product the architecture's source category.

---

# Phase 3 — Make Data Sources Domain-Agnostic

**Status: Complete**

A source mechanism may provide observations belonging to several domains.

For example:

```text
                     MqttDataSource
                            │
              ┌─────────────┴─────────────┐
              ▼                           ▼
        Weather mappings            Solar mappings
              │                           │
              └─────────────┬─────────────┘
                            ▼
                    SensorRepository
```

`MqttDataSource` does not know which Screen consumes the data.

---

# Phase 4 — Define Semantic Observation Identity

**Status: Architectural decision locked; implementation next**

The framework introduces `ObservationKey` as the stable domain-facing identity of an observation.

The key is:

- meaningful
- stable
- human-readable
- source-independent
- Screen-independent
- independent of storage order
- independent of runtime allocation

Where an existing external identity already expresses the meaning well, such as a Home Assistant entity ID, the framework may use it directly.

The key is not a repository index.

---

# Phase 5 — Introduce Runtime Observation Handles

**Status: Planned**

`ObservationHandle` becomes the compact runtime reference used after registration/resolution.

The runtime allocates it.

No domain contract contains a manually invented storage number.

---

# Phase 6 — Separate Handles from Repository Storage

**Status: Planned**

`SensorRepository` will keep storage private.

The desired relationship is:

```text
ObservationHandle
      │
      ▼
repository record
      │
      ▼
storage slot
```

The storage representation remains an implementation concern.

---

# Phase 7 — Dynamic Source Resolution

**Status: Planned**

An observation may have several candidate source mechanisms:

```text
room.temperature
    │
    ├── local BME280 / I²C
    ├── MQTT / Home Assistant
    └── Modbus
```

The key remains stable.

Source selection may use availability, priority, freshness, health, or other policy.

Round-robin may be one possible policy where several sources are equivalent, but it is not an identity mechanism.

---

# Phase 8 — Multiple Sources on One Screen

**Status: Architectural requirement**

A Screen may consume observations from several independent mechanisms:

```text
                     DataSourceManager
                            │
          ┌─────────────────┼─────────────────┐
          ▼                 ▼                 ▼
        MQTT              I²C               API
          │                 │                 │
          └─────────────────┼─────────────────┘
                            ▼
                    SensorRepository
                            │
                            ▼
                      Active Screen
```

A Smart Wall Panel may combine local sensors, MQTT devices, API information, and future Modbus observations without making the Screen source-aware.

---

# Phase 9 — Platform Profiles and Composition

**Status: Architectural decision locked; implementation underway**

Telemetry remains one framework.

PlatformIO environments instantiate target-specific capability compositions.

```text
                         TELEMETRY
                             │
              ┌──────────────┴──────────────┐
              │                             │
       Shared Framework              PlatformIO Composition
              │                             │
              │                    ┌────────┴────────┐
              │                    ▼                 ▼
              │              ESP8266 env        ESP32/CYD env
              │                    │                 │
              │                    ├── MQTT          ├── MQTT
              │                    ├── Local I/O     ├── API/TLS
              │                    ├── Touch         ├── Local I/O
              │                    └── constrained   ├── Touch
              │                        observation   ├── Control
              │                        UI            └── richer UI
              │
              └──── shared contracts and domain model ────
```

### Platform Profile Principle

> **A platform profile is a composition of Telemetry capabilities selected for a target's resources and purpose. Platform profiles do not redefine Telemetry's architectural contracts.**

The reference profiles are:

### ESP8266 Reference Profile

> **Observation-oriented, resource-constrained composition.**

The ESP8266 remains a deliberate constrained reference platform. It validates memory discipline, allocation discipline, compact runtime design, and lightweight observation architecture.

MQTT and appropriate local I/O capabilities are its primary data mechanisms.

### ESP32/CYD Reference Profile

> **Observation-and-control-oriented, resource-expanded composition.**

The ESP32/CYD provides the resource envelope for richer observation, API/TLS integration, multi-source composition, system integration, and control-oriented applications.

Capabilities unsuitable or unnecessary for the ESP8266 are not implemented as workarounds. They are omitted from that composition.

The governing rule is:

> **Telemetry defines the architecture. Composition determines which capabilities a target can instantiate.**

PlatformIO provides this composition boundary through build environments rather than a second platform-specific framework architecture.

---

# Phase 10 — Future Command Identity

**Status: Future architectural pressure**

Interactive target applications may eventually publish commands such as:

```text
livingroom.lamp.set
hvac.target_temperature.set
blind.position.set
```

Command identity is deliberately not mixed into the observation contract in this sprint.

A future sprint may introduce an analogous command identity model if the requirement warrants it.

---

# Phase 11 — Availability and Retained MQTT Data

**Status: Existing architecture / documentation**

MQTT retained messages allow a newly connected subscriber to receive the last published value immediately.

The preferred architecture remains:

> **The source should provide current observations as early as reasonably possible.**

---

# Definition of Done

Sprint Zeta is complete when:

- [x] Solar exists as an independent domain.
- [x] Source mechanisms are organised independently of products.
- [x] MQTT can provide observations for multiple domains.
- [x] Semantic `ObservationKey` identity is defined and frozen.
- [ ] `ObservationKey` is implemented as the stable domain-facing identity.
- [ ] `ObservationHandle` is introduced as an opaque runtime reference.
- [ ] Repository storage slots become implementation details.
- [ ] Source resolution is independent of observation identity.
- [ ] Multiple sources can resolve to the same observation key.
- [ ] A single Screen can consume observations from multiple independent sources.
- [x] PlatformIO is established as the target composition mechanism.
- [x] ESP8266 is defined as the constrained observation reference profile.
- [x] ESP32/CYD is defined as the expanded observation-and-control reference profile.
- [ ] Platform-specific capability composition is fully wired into application construction.
- [ ] Command identity, if required, is designed separately rather than mixed into the observation contract.

---

# Architectural Result

The data architecture now has two independent axes.

### Identity axis

```text
Meaning
  ↓
ObservationKey
  ↓
ObservationHandle
  ↓
Storage
```

### Composition axis

```text
Architecture
    ↓
Capabilities
    ↓
PlatformIO Composition
    ↓
Target platform
```

Together:

```text
                         TELEMETRY
                             │
                 ┌───────────┴───────────┐
                 │                       │
             Identity                Composition
                 │                       │
           ObservationKey          PlatformIO env
                 │                       │
        ObservationHandle          ESP8266 / ESP32
                 │                       │
              Storage                Capabilities
```

The framework remains one architecture.

The target determines which parts of that architecture are instantiated.

---

# Architectural Lesson

The original problem appeared to be:

> "How do we stop Weather and Solar from occupying accidental ranges in one array?"

The deeper problem was:

> **We had allowed storage identity to become domain identity.**

The solution is not to choose better numbers.

The solution is to separate meaning from storage:

```text
Meaning
  ↓
ObservationKey
  ↓
Resolution
  ↓
ObservationHandle
  ↓
Storage
```

And the platform question is separate:

```text
Architecture
  ↓
Capabilities
  ↓
Composition
  ↓
Target platform
```

This is what allows Telemetry to remain one framework while being deliberately different in composition on constrained observation devices and richer observation-and-control devices.
