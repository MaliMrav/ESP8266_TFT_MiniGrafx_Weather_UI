# 003 — The Data Boundary

## Why should the application not care where data comes from?

A weather application needs information.

That information might come from:

- a local sensor
- MQTT
- an HTTP API
- a database
- a simulated source
- another system

The application should not need to know which one.

## The original temptation

A small application often begins like this:

```text
WeatherScreen
    │
    ▼
MQTT
    │
    ▼
Broker
```

The screen is now coupled to the transport mechanism.

It is no longer asking:

> What is the current temperature?

It is asking:

> How do I retrieve the temperature from MQTT?

Those are different questions.

## Observations and information

Telemetry separates observations from the systems that transport them:

```text
┌─────────────────────┐
│      Observation    │
│  temperature=21.4   │
└──────────┬──────────┘
           ▼
┌─────────────────────┐
│     Data Source     │
│ MQTT / Sensor / API │
└──────────┬──────────┘
           ▼
┌─────────────────────┐
│     Application     │
│   Uses information  │
└─────────────────────┘
```

The data source obtains data.

The application uses it.

## The boundary

```text
Application
    │
    ▼
IDataSource
    │
    ├── MQTT
    ├── Local Sensor
    ├── HTTP API
    └── Simulation
```

The application asks for data.

It does not need to know how that data arrived.

## Why does this matter?

If the source changes from MQTT to a local sensor, a screen coupled to MQTT must change.

A screen depending on the data contract can remain unchanged.

The implementation changes.

The application does not.

## Platform Composition

Telemetry remains one framework. PlatformIO environments instantiate different capability compositions for different targets.

The ESP8266 is the constrained observation reference profile. The ESP32/CYD is the expanded observation-and-control reference profile.

A capability that is unnecessary or unsuitable for the ESP8266 is not implemented as a workaround. It is simply omitted from that composition.

```text
Architecture
    │
    ▼
Capabilities
    │
    ▼
Composition
    │
    ▼
Target platform
```

> **Telemetry defines the architecture. Composition determines which capabilities a target can instantiate.**


## The lesson

A data source is a capability.

Transport is an implementation detail.

The application should consume information through a stable boundary.

Telemetry deliberately stops before intelligence.

It observes.

It transforms observations into useful information.

It leaves reasoning, automation and decision-making to systems above it.


## A Further Refinement: Identity Is Not Storage

The original data boundary separated the application from its sources, but the Solar expansion revealed a second coupling inside the repository itself.

A numeric sensor ID can accidentally become both:

- the identity of an observation; and
- its position in a storage array.

Those are different responsibilities.

A larger application may place observations from several domains on the same Screen:

```text
Smart Wall Panel
    ├── room.temperature        ← I2C
    ├── room.humidity           ← I2C
    ├── weather.forecast        ← API
    ├── solar.power.production  ← MQTT
    └── livingroom.lamp.state   ← MQTT
```

The Screen should not need to know which source provided each observation, and it should not need to know where the repository stores it.

The stronger boundary is therefore:

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
Repository storage
      │
      ▼
Application
```

This gives us the rule:

> **Identity describes meaning. Storage describes implementation.**

The runtime is free to allocate handles and storage slots as it sees fit.

Adding a new observation must not require renumbering an existing one.
