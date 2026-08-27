# Platform Profiles

## Principle

> **A platform profile is a composition of Telemetry capabilities selected for a target's resources and purpose. Platform profiles do not redefine Telemetry's architectural contracts.**

Telemetry remains one framework.

PlatformIO environments instantiate different compositions of that framework.

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

## ESP8266 Reference Profile

**Observation-oriented, resource-constrained composition.**

The ESP8266 is used deliberately as a constrained reference platform.

Its composition prioritises:

```text
MQTT
Local I/O
Touch / lightweight input
Constrained presentation
Memory discipline
Allocation discipline
```

API/TLS and other resource-intensive capabilities are not worked around on this profile. They are simply not instantiated where the composition does not require them.

## ESP32/CYD Reference Profile

**Observation-and-control-oriented, resource-expanded composition.**

The ESP32/CYD composition can instantiate capabilities such as:

```text
MQTT
API / TLS
Local I/O
Richer presentation
Multiple source mechanisms
Control / publishing
System integrations
```

The contracts remain the same as those used by the ESP8266 composition.

## PlatformIO

PlatformIO environments are the composition boundary.

A target environment selects:

- target platform and board
- libraries
- build flags
- resource-specific settings
- capabilities instantiated by the application composition

The source tree remains the shared Telemetry architecture.

## The Rule

A capability may exist in Telemetry even when a particular target does not instantiate it.

That is intentional.

> **Telemetry defines the architecture. Composition determines which capabilities a target can instantiate.**
