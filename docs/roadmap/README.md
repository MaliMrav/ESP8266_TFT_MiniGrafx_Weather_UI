# Roadmap

The roadmap records the architectural journey of Telemetry.

These are not simply lists of completed features. Each sprint represents a stage in the evolution of the system.

## The Sprints

- [Sprint Alpha](Sprint_Alpha.md) — The initial foundation.
- [Sprint Beta — Establish the Application Boundaries](Sprint_Beta.md)
- [Sprint Gamma — Establish the Control Panel](Sprint_Gamma.md)
- [Sprint Delta — Interaction Architecture](Sprint_Delta.md)

## The Roadmap as a Breadcrumb Trail

```text
Foundation
    ↓
Application boundaries
    ↓
Control Panel
    ↓
Interaction architecture
    ↓
Navigation
```

The roadmap is a record of architectural discovery.

Sprint Delta established the interaction architecture. The next implementation task is to return to the deferred:

> Weather → Control Panel navigation

The architecture comes first.

The implementation follows.


## Current Architectural Pressure

The Solar and data-source work has exposed a further refinement that will be recorded as a new sprint after the architecture is frozen:

```text
ObservationKey
      │
      ▼
ObservationHandle
      │
      ▼
Repository storage
```

The principle is:

> **Identity is semantic; runtime storage is implementation detail.**

The same observation may also be resolved from multiple source mechanisms.
