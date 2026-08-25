# Sprint Zeta — Domain Data and Source Architecture

## Objective

> **Separate what an observation means from where it is stored, while allowing multiple independent data sources to contribute to the same application context.**

Sprint Epsilon established that a Screen is a contextual interpreter of interaction.

Solar Energy exposed the equivalent pressure in the data layer.

The project now has multiple application domains and multiple possible source mechanisms. A single Screen may consume observations from several sources at the same time, while the same observation may be supplied by different sources on different devices or under different operating conditions.

The original data model was beginning to encode storage details as domain identity.

That is the pressure this sprint resolves.

---

# The Architectural Pressure

The original Sensor model used small integer constants directly as repository indices:

```text
Weather:
    0
    1
    2
    3
    4

Solar:
    5
    6
    7
    ...
```

Even after moving the constants into per-domain headers, the integer remained both:

1. the identity of the observation; and
2. the physical location of that observation in the repository.

That creates an architectural leak.

A developer should be able to ask:

> **What observation is this?**

without also asking:

> **Which array slot did the framework assign it?**

The Smart Wall Panel thought experiment made the pressure explicit.

A single Screen might consume:

```text
room.temperature
room.humidity
room.pressure
weather.forecast
solar.power.production
livingroom.lamp.state
hvac.target_temperature
```

while obtaining those observations from:

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

---

# The Course Correction

The earlier idea of "stable integer IDs per domain" is now superseded.

The new contract is:

> **Domains own stable semantic observation keys. Runtime storage handles are allocated by the framework. Physical storage slots are implementation details.**

The conceptual model is:

```text
ObservationKey
      │
      ▼
Source resolution
      │
      ├── availability
      ├── priority
      ├── freshness
      └── policy
      │
      ▼
ObservationHandle
      │
      ▼
Repository storage slot
```

These are deliberately different concepts.

### ObservationKey

An `ObservationKey` is a human- and domain-meaningful identity.

Examples:

```text
solar.power.production
solar.power.consumption
room.temperature
room.humidity
livingroom.lamp.state
hvac.target_temperature
weather.forecast.condition
```

The key answers:

> **What observation are we talking about?**

It does not encode storage position.

It does not need to be a developer-invented integer.

---

### ObservationHandle

An `ObservationHandle` is an opaque runtime reference allocated by the framework.

The handle answers:

> **Which runtime observation record represents this key right now?**

The application should not need to invent or remember the handle.

---

### Repository storage slot

A storage slot is an internal implementation detail.

The slot answers:

> **Where is this record physically stored?**

The repository is free to allocate or reorganise slots without changing the observation's identity.

---

# Core Principles

The governing rule is:

> **Identity describes meaning. Storage describes implementation.**

And:

> **Identity is stable; source resolution is dynamic.**

Therefore:

> **Adding an observation must never require renumbering another observation.**

Likewise, changing the source of an observation must not change the observation's identity.

For example:

```text
room.temperature
    │
    ├── BME280 / I²C
    ├── MQTT / Home Assistant
    └── Modbus controller
```

The identity remains:

```text
room.temperature
```

The source may change.

The Screen does not.

---

# Phase 1 — Introduce the Solar Domain

**Status: Complete**

Solar Energy is now a first-class application domain.

The Solar Screen consumes observations through `SensorRepository`.

Its current domain includes:

```text
solar.power.production
solar.power.consumption
solar.power.export
solar.power.battery
solar.energy.production.today
solar.energy.consumption.today
solar.energy.export.today
solar.energy.battery.today
```

The Screen does not know which source provided those observations.

The Solar domain is therefore independent of transport.

---

# Phase 2 — Separate Source Mechanism from Product

**Status: Complete**

The source architecture is organised by mechanism rather than vendor or product.

The intended structure is:

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

The architectural categories answer:

> **How does the framework obtain data?**

They do not answer:

> **Which vendor or device supplied it?**

For example:

```text
API
 ├── Envoy
 ├── AlphaESS
 └── another provider
```

The provider-specific knowledge belongs inside the API implementation and its mappings.

This prevents the source tree from becoming a catalogue of products.

---

# Phase 3 — Make Data Sources Domain-Agnostic

**Status: Complete / current architecture**

A source mechanism may provide observations belonging to several domains.

For MQTT:

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

`MqttDataSource` does not know about Screens.

`TopicMappings` binds external MQTT topics to domain-owned observation identities.

The same principle applies to API sources:

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
Domain observations
```

A source implementation provides observations.

A domain defines what those observations mean.

Those responsibilities remain separate.

---

# Phase 4 — Define Semantic Observation Identity

**Status: Architectural decision locked; implementation next**

This phase replaces direct integer identity with the semantic-key contract.

The framework will introduce:

```text
ObservationKey
```

as the stable domain-facing identity.

The key must be:

- meaningful
- stable
- source-independent
- screen-independent
- independent of repository storage order
- independent of runtime allocation

The key is not a storage index.

The developer defines meaning.

The runtime defines storage.

---

# Phase 5 — Introduce Runtime Observation Handles

**Status: Planned**

The repository will resolve an `ObservationKey` into an opaque runtime:

```text
ObservationHandle
```

Conceptually:

```text
ObservationKey
      │
      ▼
register / resolve
      │
      ▼
ObservationHandle
```

The handle is an implementation detail.

No domain contract should require values such as:

```text
17
42
0x5C8E2B13
```

merely to identify an observation.

A developer should be able to declare:

```text
solar.power.production
```

without knowing where the framework will store it.

---

# Phase 6 — Separate Handles from Repository Storage

**Status: Planned**

The repository must stop treating domain identity as a direct array index.

The target model is:

```text
SensorRepository
    │
    ├── ObservationHandle → record
    │
    └── record → SensorTile / observation state
```

The repository may continue to use compact fixed-capacity storage internally.

The important change is that storage position is no longer exposed as domain identity.

Efficient embedded storage therefore remains possible without making the storage layout part of the architecture.

---

# Phase 7 — Dynamic Source Resolution

**Status: Planned**

A semantic observation may have more than one possible source.

For example:

```text
room.temperature
    │
    ├── local BME280
    ├── MQTT / Home Assistant
    └── Modbus controller
```

The `ObservationKey` remains stable.

Source selection becomes a runtime policy.

Possible factors include:

- source availability
- source priority
- observation freshness
- source health
- explicit configuration

Round-robin selection may be appropriate when several sources are equivalent, but it is a **resolution policy**, not an identity mechanism.

The key rule is:

> **Multiple sources may resolve to the same semantic observation.**

---

# Phase 8 — Multiple Sources on One Screen

**Status: Architectural requirement**

A Screen may consume observations from several independent source mechanisms simultaneously.

For example:

```text
                     DataSourceManager
                            │
          ┌─────────────────┼─────────────────┐
          ▼                 ▼                 ▼
        MQTT              I²C               API
          │                 │                 │
     observations      observations      observations
          │                 │                 │
          └─────────────────┼─────────────────┘
                            ▼
                    SensorRepository
                            │
                            ▼
                     Smart Wall Panel
```

A Smart Wall Panel might display:

```text
Date / Time
Weather forecast
Room temperature
Room humidity
Air pressure
Solar production
Light state
HVAC state
```

while obtaining those observations from different source mechanisms.

The Screen should not need to know which source supplied each value.

This is a core framework property.

---

# Phase 9 — Future Command Identity

**Status: Future architectural pressure**

An interactive panel may also publish commands.

Examples:

```text
livingroom.lamp.set
hvac.target_temperature.set
blind.position.set
```

This is deliberately **not** being folded into the observation contract.

The current sprint establishes observation identity and source independence first.

A future sprint may introduce a corresponding command/action identity model if the pressure warrants it.

The rule remains:

> **Do not turn the observation architecture into a command bus before the requirement exists.**

---

# Phase 10 — Availability and Retained MQTT Data

**Status: Existing source behaviour / documented architecture**

MQTT is push-based.

A newly connected device may otherwise wait for the next publication.

Retained messages allow the broker to deliver the last known value immediately to a new subscriber.

This avoids introducing device-side caching merely to compensate for source timing.

The preferred architecture remains:

> **The source should provide current observations as early as reasonably possible.**

---

# Platform Constraints

The ESP8266 has a constrained runtime heap.

TLS-backed HTTPS can consume substantial memory and compete with display and MQTT requirements.

This remains a platform constraint, not a domain identity constraint.

Therefore:

```text
ESP8266
    └── MQTT composition may be appropriate

ESP32-class hardware
    └── API/TLS composition may be appropriate
```

The domain model remains identical.

Only source composition changes.

---

# Platform Composition

**Status: Architectural decision locked**

Telemetry is a framework, not an ESP8266 architecture. PlatformIO environments are
the build-time composition mechanism. The shared source tree defines contracts and
reusable capabilities. A PlatformIO environment selects the target platform, board,
resource envelope, libraries and capabilities instantiated for that build.

```text
                         TELEMETRY
                             │
             ┌───────────────┴───────────────┐
             │                               │
       Shared Framework                 PlatformIO Composition
             │                               │
             │                    ┌──────────┴──────────┐
             │                    ▼                     ▼
             │              ESP8266 env           ESP32/CYD env
             │                    │                     │
             │                    ├── MQTT              ├── MQTT
             │                    ├── Local I/O         ├── API/TLS
             │                    ├── Touch             ├── Local I/O
             │                    └── constrained UI    ├── Touch
             │                                          ├── Control
             │                                          └── richer UI
             │
             └──── identical contracts and domain model ─────
```

The project deliberately does not create separate framework architectures under
`src/platform/esp8266` and `src/platform/esp32`.

The governing principle is:

> **Telemetry defines the architecture. Composition determines which capabilities a target can instantiate.**

The ESP8266 remains a constrained reference profile. An ESP32/CYD profile may
instantiate capabilities that are impractical on the ESP8266, including API/TLS,
richer UI, control and broader multi-source composition.

PlatformIO expresses this through build environments and build flags. Capability
selection should remain at the composition root rather than spreading platform
conditionals throughout the framework.

# Definition of Done

Sprint Zeta is complete when:

- [x] Solar exists as an independent domain.
- [x] Source mechanisms are organised independently of products.
- [x] MQTT can provide observations for multiple domains.
- [x] The course correction from integer identity to semantic `ObservationKey` is documented and frozen.
- [ ] `ObservationKey` replaces domain-facing integer identity.
- [ ] `ObservationHandle` is introduced as an opaque runtime reference.
- [ ] Repository storage slots become implementation details.
- [ ] Domain identity is independent of storage ordering.
- [ ] Multiple sources can resolve to the same observation key.
- [ ] Source selection policy is defined without becoming part of observation identity.
- [ ] A single Screen can consume observations from multiple independent sources.
- [ ] The observation model remains reusable for future interactive/source-publishing applications.
- [ ] Command identity, if required, is designed separately rather than mixed into the observation contract.

---

# The Architectural Result

The data architecture should ultimately read as:

```text
                    Semantic Knowledge
                           │
                           ▼
                   ObservationKey
                           │
                           ▼
                    Source resolution
                           │
                           ▼
                  ObservationHandle
                           │
                           ▼
                 Repository storage
                           ▲
                           │
              ┌────────────┼────────────┐
              │            │            │
             MQTT         I²C          API
              │            │            │
              └────────────┴────────────┘
```

Screens consume observations by identity:

```text
ObservationKey
      │
      ▼
ObservationHandle
      │
      ▼
Screen
```

not:

```text
Sensor number 17
      │
      ▼
array slot 17
```

---

# Architectural Lesson

The original problem appeared to be:

> "How do we stop Weather sensors and Solar sensors from occupying accidental ranges in one array?"

The deeper problem was:

> **We had allowed storage identity to become domain identity.**

The solution is not merely to choose better numbers.

The solution is to remove storage identity from the domain model entirely.

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

This is the course correction that makes the data architecture suitable for Telemetry as a framework rather than a single weather application.
