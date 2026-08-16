# Sprint Epsilon — Screen Navigation

## Objective

> **Introduce screen-to-screen navigation without compromising the interaction architecture established in Sprint Delta.**

Sprint Delta established the interaction boundary:

```text
Physical Input
      │
      ▼
Input Source
      │
      ▼
InputManager
      │
      ▼
InputEvent
      │
      ▼
Active Screen / Context
```

Sprint Epsilon now asks a narrower question:

> **How does contextual interpretation become a request to change Screens without giving the Screen ownership of navigation?**

The answer must preserve the distinction established in Sprint Delta:

```text
                         Active Screen
                              │
                         InputEvent
                              │
                              ▼
                  contextual interpretation
                              │
             ┌────────────────┼────────────────┐
             │                │                │
             ▼                ▼                ▼
       Local behaviour   ScreenIntent     SystemIntent
             │                │                │
             ▼                ▼                ▼
        Screen state     ScreenManager    System Context
```

Navigation is therefore **one possible interpretation** of an input event, not the universal destination of input.

---

## The Architectural Question

The central question for Sprint Epsilon is:

> **How does a context express that an interaction means "leave this Screen"?**

Not:

> "How does WeatherScreen call ControlPanelScreen?"

That would create the wrong dependency direction.

The active context should interpret the interaction.

If that interpretation crosses the Screen boundary, it becomes a `ScreenIntent`.

`ScreenManager` then owns execution of the transition.

---

# The Target Architecture

The target flow is:

```text
InputEvent
      │
      ▼
Active Screen / Context
      │
      │ contextual interpretation
      ▼
ScreenIntent
      │
      ▼
ScreenManager
      │
      ▼
Screen transition
```

Local interactions remain local:

```text
InputEvent
      │
      ▼
Active Screen / Context
      │
      └── Local behaviour
```

System-level interactions remain distinct:

```text
InputEvent
      │
      ▼
Appropriate System Context
      │
      └── System-level operation
```

Sprint Epsilon implements the ScreenIntent branch without disturbing the other two.

---

# Phase 1 — Define the Navigation Contract

**Status: Complete**

Phase 1 established the ownership model before implementation.

The active Screen is the normal contextual boundary for application interaction.

A Screen provides:

```text
Presentation
Zones
Local Context
```

When an `InputEvent` arrives, the active context determines what it means.

The possible outcomes are:

```text
Local behaviour
ScreenIntent
SystemIntent
```

The key architectural rule is:

> **Local meaning stays local. Meaning that crosses a responsibility boundary gets an explicit contract.**

Therefore we deliberately do **not** introduce a framework-wide `LocalAction` vocabulary for every Screen.

For example:

```text
TAP + clock position
        │
        ▼
WeatherScreen
        │
        └── toggle clock presentation
```

is local behaviour.

It does not need to become `TOGGLE_TIME_FORMAT` in `InputAction`.

But:

```text
TAP + navigation control
        │
        ▼
WeatherScreen
        │
        └── ScreenIntent
```

crosses the Screen boundary and therefore requires an explicit navigation contract.

Similarly, a hidden key sequence requesting factory reset or AP mode is system-level meaning and belongs to a system-level owner.

### Phase 1 Outcome

The navigation architecture is now defined as:

```text
                         Active Screen
                              │
                         InputEvent
                              │
                              ▼
                  contextual interpretation
                              │
             ┌────────────────┼────────────────┐
             │                │                │
             ▼                ▼                ▼
       Local behaviour   ScreenIntent     SystemIntent
             │                │                │
             ▼                ▼                ▼
        Screen state     ScreenManager    System Context
```

No implementation should begin until this boundary is understood.

---

# Phase 2 — Define `ScreenIntent` as the Contract

Define `ScreenIntent` as the explicit contract between the active context and `ScreenManager`.

Conceptually:

```text
InputEvent
      │
      ▼
Active Screen
      │
      │ contextual interpretation
      ▼
ScreenIntent
      │
      ▼
ScreenManager
```

The Screen does not manipulate `ScreenManager`.

It expresses an intended navigation outcome.

`ScreenManager` consumes that outcome and remains responsible for the actual transition.

The contract should answer only the questions needed for the current framework.

It should not prematurely solve:

- arbitrary routing
- navigation stacks
- deep links
- animation policies
- history management
- nested navigation

### Outcome

A minimal, explicit `ScreenIntent` contract between a Screen and `ScreenManager`.

---

# Phase 3 — Implement the `ScreenIntent` Model

Introduce the actual representation of navigation intent into the framework.

The MVP should establish only the navigation concepts currently required.

The contract may eventually need to represent concepts such as:

```text
No navigation requested
Next screen
Previous screen
Back
Specific screen
```

The precise vocabulary should be determined from the existing framework and the first thin slice rather than expanded for hypothetical future requirements.

### Outcome

A minimal, compilable `ScreenIntent` model.

---

# Phase 4 — Integrate `ScreenIntent` with `Screen`

Modify the `Screen` contract so that an active Screen can express navigation intent without acquiring ownership of `ScreenManager`.

Conceptually:

```text
Screen
  │
  ├── receives InputEvent
  │
  ├── interprets it
  │
  └── produces ScreenIntent
```

The implementation mechanism—whether `onInput()` returns an intent or uses another explicit contract—should be determined from the existing framework structure.

The important requirement is the ownership boundary, not a particular implementation technique.

### Outcome

Screens can express navigation requests without directly manipulating navigation infrastructure.

---

# Phase 5 — Integrate `ScreenIntent` with `ScreenManager`

`ScreenManager` becomes the consumer and executor of navigation intent.

The resulting flow is:

```text
InputEvent
      │
      ▼
Active Screen
      │
      ▼
ScreenIntent
      │
      ▼
ScreenManager
      │
      ├── No navigation requested
      │       └── remain on current Screen
      │
      └── Navigation requested
              │
              ▼
          execute transition
```

This establishes the architectural ownership boundary:

> **The context interprets. The Screen expresses intent. `ScreenManager` executes the transition.**

### Outcome

Navigation ownership is enforced by the implementation.

---

# Phase 6 — Implement and Validate the Thin Slice

The first demonstrable navigation path should be deliberately small:

```text
WeatherScreen
      │
      │ ScreenIntent
      ▼
ScreenManager
      │
      ▼
ControlPanelScreen
```

And the reverse:

```text
ControlPanelScreen
      │
      │ BACK → ScreenIntent
      ▼
ScreenManager
      │
      ▼
WeatherScreen
```

This provides a complete end-to-end demonstration without attempting to solve every possible navigation problem.

---

# Transition Lifecycle

The navigation implementation must preserve the existing Screen lifecycle contract:

```text
Current Screen
      │
      ▼
leave()
      │
      ▼
ScreenManager changes active Screen
      │
      ▼
enter()
      │
      ▼
New Screen
```

`ScreenManager` remains responsible for ensuring that:

- the old Screen is left correctly
- the new Screen is entered correctly
- there is exactly one active Screen
- navigation state remains unambiguous
- Screens do not manipulate transition infrastructure directly

---

# Validation Against the Interaction Architecture

The completed Sprint Epsilon architecture must correctly handle all three interaction scopes.

## Local Interaction

```text
TAP
 │
 ▼
WeatherScreen
 │
 └── local interpretation
       │
       └── toggle clock presentation
```

No navigation occurs.

## Screen Navigation

```text
InputEvent
 │
 ▼
WeatherScreen
 │
 └── contextual interpretation
       │
       └── ScreenIntent
              │
              ▼
         ScreenManager
              │
              ▼
       ControlPanelScreen
```

Navigation occurs, but the Screen does not own the transition.

## System-Level Interaction

```text
InputEvent sequence
 │
 ▼
System Context
 │
 └── SystemIntent
        │
        └── system-level operation
```

The event does not become navigation merely because a Screen happens to be visible.

For example, a future keyboard or button sequence might request:

```text
RESET_TO_FACTORY_DEFAULTS
ENTER_AP_MODE
BEGIN_OTA_UPDATE
```

Those are system-level outcomes, not screen navigation.

This reinforces the architectural principle established in Sprint Delta:

> **Navigation is one possible interpretation of interaction—not the universal destination of interaction.**

---

# Framework Test

Sprint Epsilon should remain valid if a completely new application Screen is introduced.

For example:

```text
SolarPowerScreen
```

might contain zones for:

- solar production
- household consumption
- battery storage
- grid export

It may interpret:

```text
SCROLL_LEFT
```

as moving a historical graph backwards.

That local behaviour does not require a new framework-wide `InputAction`.

If the Screen interprets another interaction as a request to open a battery configuration Screen, that meaning becomes a `ScreenIntent`.

If it interprets an interaction as a request to perform a device-wide operation, that meaning becomes a `SystemIntent`.

The framework remains unchanged.

That is an important test of the architecture.

---

# Phase 7 — Navigation History

**Status: Complete**

## The Pressure

Sprint Epsilon deliberately deferred navigation history. The thin slice only needed `Weather → ControlPanel → Weather`.

Once `SolarScreen` was introduced, the navigation tree became genuinely hierarchical:

```text
Weather
  └── NEXT → Solar
                └── NEXT → ControlPanel (menu)
                                ├── SELECT → About
                                └── SELECT → Connectivity
```

The original `BACK` implementation hardcoded a return to `WeatherScreen`. This meant that `BACK` from `ControlPanel` skipped `SolarScreen` entirely — the user lost their place in the navigation tree.

The pressure was real. The fix was necessary.

## Why Not a Carousel?

A ring model (`Weather ↔ Solar ↔ ControlPanel`) was considered and rejected.

`ControlPanel` has internal depth — pages within it. When a user is inside `About`, `PREVIOUS_SCREEN` would mean either "go to Solar" or "go back to the menu". Those are two different things sharing one gesture. The model breaks down the moment any screen has internal hierarchy.

A history stack is the honest model for a hierarchical navigation tree.

## The Decision

`ScreenManager` maintains a shallow history stack.

- `NAVIGATE` pushes the current screen onto the stack before activating the destination.
- `BACK` pops the stack and returns to wherever the user came from.
- `activate()` — used for boot and calibration — does **not** push to the stack. Those transitions are not part of user-navigable history.

The stack has a fixed maximum depth (`MAX_HISTORY = 8`), which is protective against unbounded growth on a memory-constrained device.

## What This Means for Screens

Nothing changed in any Screen.

Screens still express `ScreenIntent::navigateTo(kind)` or `ScreenIntent::back()`. They have no knowledge of the stack. `ScreenManager` owns the history entirely.

That is the architecture working correctly.

```text
InputEvent
      │
      ▼
Active Screen
      │
      └── ScreenIntent::back()
                │
                ▼
          ScreenManager
                │
                └── pop history stack
                          │
                          ▼
                    previous Screen
```

## Navigation Tree (current)

```text
Weather
  └── NEXT_SCREEN → Solar
                      └── NEXT_SCREEN → ControlPanel (menu)
                                            ├── SELECT → About
                                            │     └── BACK → ControlPanel (menu)
                                            └── SELECT → Connectivity
                                                  └── BACK → ControlPanel (menu)
                                        BACK → Solar
                      BACK → Weather
```

---

# Phase 8 — Per-Domain Sensor IDs

**Status: Complete**

## The Pressure

The original `SensorIds.h` was a single flat enum containing every sensor in the system. When `SolarScreen` was added alongside `WeatherScreen`, the weather screen had to call `getTiles(SENSOR_KITCHEN_TEMP, 5)` — a range slice that relied on weather sensors occupying the first five positions in the array. The count was implicit. The boundary was positional.

The pressure became clear when thinking one step further: a future room panel screen would add its own sensors to the same enum. Every domain would need to know the total count of every other domain's sensors just to avoid reading the wrong tiles.

The smell was not in `Topics.h` — that file is a string lookup table and should grow. The smell was in the flat enum forcing global awareness of local concerns.

## Why Not Keep the Flat Enum?

A flat enum works when there is one domain. It breaks when there are two, because the second domain's starting index depends on the first domain's count. That is positional coupling disguised as named coupling.

The `static_assert` in `SensorRepository.cpp` was enforcing exact count equality — meaning adding any sensor anywhere required touching a global file that belonged to no domain in particular.

## The Decision

Sensor IDs are declared per-domain as `constexpr uint8_t` constants in domain-owned header files:

```text
WeatherSensorIds.h    — owned by the weather domain
SolarSensorIds.h      — owned by the solar domain
```

`SensorRepository` becomes a flat indexed store with a fixed capacity (`MAX_SENSORS = 32`). It has no knowledge of domains. The `static_assert` validates capacity, not exact count.

`SensorRepository.cpp` uses C99 designated initialisers to make slot assignment explicit:

```cpp
[SENSOR_KITCHEN_TEMP] = { "Kitchen Temp", "°C", TEMP },
[SENSOR_SOLAR_POWER_NOW] = { "Production", "W", ENERGY_W },
```

This makes the mapping between ID and tile auditable at a glance, and eliminates any dependency on array position.

## What Changed in Screens

`WeatherScreen` now declares an explicit ID array:

```cpp
static const uint8_t ids[WEATHER_SENSOR_COUNT] = {
    SENSOR_KITCHEN_TEMP,
    SENSOR_PERGOLA_TEMP,
    SENSOR_KITCHEN_HUM,
    SENSOR_PERGOLA_HUM,
    SENSOR_PRESSURE
};
```

This is the same pattern `SolarScreen` already used. Both screens now call `getTile(id)` by explicit ID. Neither knows about the other's sensors.

## What a Future Domain Looks Like

Adding a room panel screen requires:

1. Create `RoomSensorIds.h` with `constexpr uint8_t` IDs starting after the last used slot.
2. Add tiles to `SensorRepository.cpp` using designated initialisers.
3. Add topic bindings to `TopicMappings.cpp` including the new domain header.
4. The new screen includes only `RoomSensorIds.h`.

No weather file changes. No solar file changes. No global enum to update.

## Files Changed

- `src/models/SensorIds.h` — deleted
- `src/models/SensorCapacity.h` — new; defines `MAX_SENSORS`
- `src/models/WeatherSensorIds.h` — new; weather domain IDs
- `src/models/SolarSensorIds.h` — new; solar domain IDs
- `src/models/SensorRepository.h` — `SensorId` → `uint8_t`; removed range API
- `src/models/SensorRepository.cpp` — designated initialisers; capacity assert
- `src/data/sources/mqtt/TopicMappings.h` — includes domain headers instead of global enum
- `src/screens/WeatherScreen.cpp` — explicit ID array replaces range slice
- `src/screens/SolarScreen.h/.cpp` — `SensorId` → `uint8_t`

---


Sprint Epsilon Phase 1 is complete when:

- [x] The active Screen is established as the normal contextual interpretation boundary.
- [x] Local behaviour is explicitly distinguished from navigation.
- [x] `ScreenIntent` is identified as the contract required when meaning crosses the Screen boundary.
- [x] `SystemIntent` remains distinct from Screen navigation.
- [x] `ScreenManager` is established as the owner of screen-level transitions.
- [x] A framework-wide `LocalAction` vocabulary is deliberately rejected unless future architectural pressure demonstrates a need for one.

The Sprint as a whole will be complete when:

- [x] `ScreenIntent` exists as an explicit navigation contract.
- [x] Screens can express navigation intent without directly manipulating `ScreenManager`.
- [x] `ScreenManager` owns actual Screen transitions.
- [x] `InputEvent` remains independent of navigation.
- [x] `InputAction` remains independent of application-specific destinations.
- [x] Local contextual interactions continue to work without navigation.
- [x] System-level interactions remain distinct from navigation.
- [x] `WeatherScreen → ControlPanelScreen` works.
- [x] `ControlPanelScreen → WeatherScreen` works.
- [x] Screen lifecycle (`leave()` / `enter()`) remains correct.
- [x] The resulting implementation matches the architectural principles established in `How-We-Think.md`.

---

# Architectural Principle

Sprint Epsilon reinforces a simple rule:

> **A context interprets interaction. A Screen may express navigation intent. ScreenManager owns screen-level navigation.**

The resulting flow is:

```text
Input
  ↓
Interpretation
  ↓
Outcome
  ↓
Scope-specific ownership
  ↓
Execution
```

For local behaviour, execution remains within the Screen or Page.

For Screen navigation, the outcome is a `ScreenIntent` and execution belongs to `ScreenManager`.

For system-level operations, the outcome is a `SystemIntent` and execution belongs to the system owner.

Each layer answers a different question.

Each layer remains responsible for only the thing it is actually qualified to decide.

The architecture should produce the reaction we are aiming for throughout Telemetry:

> **"Of course... why would it be designed any other way?"**

---

# Phase 9 — EnvoyDataSource and DataSourceManager

**Status: Complete**

## The Pressure

The solar sensor IDs existed in `SolarSensorIds.h` and `SolarScreen` was wired and rendering. But the data pipeline behind it was placeholder MQTT topics in `Topics.h` and `TopicMappings.cpp` — entries pointing at broker topics that didn't exist and would have required manually republishing Envoy data through HA and then through MQTT.

Two problems with that approach:

- It introduces HA as a hard dependency. If HA is down, the solar screen shows nothing.
- It requires maintaining MQTT topics that are purely an artefact of the transport, not the data.

The Enphase Envoy exposes a local HTTP API on the LAN with no authentication. The data is available directly from the device. There is no reason to route it through a broker.

## The Architectural Question

The framework already had `IDataSource` as the boundary. The question was whether to replace `MqttDataSource` with an Envoy source, or run both simultaneously.

Weather sensors (Kitchen, Pergola) publish to MQTT. The Envoy uses HTTP. These are different transports serving different domains. Neither should know about the other.

`SystemManager` holds a single `IDataSource&`. The clean solution is a compositor that satisfies that contract while delegating to multiple sources — `DataSourceManager`.

## The Decision

- `DataSourceManager` implements `IDataSource` and owns a fixed list of sources. `SystemManager` never changes.
- `MqttDataSource` continues to serve weather sensors unchanged.
- `EnvoyDataSource` polls two Envoy local endpoints and writes solar sensor values directly to `SensorRepository`.
- Dead MQTT entries for solar data (`Topics::Envoy`, solar rows in `TopicMappings.cpp`) are removed.

## API Endpoints

| Endpoint | Provides |
|---|---|
| `GET /ivp/meters/readings` | Real-time power (W) — production `[0].activePower`, net import `[1].activePower` |
| `GET /api/v1/production` | Today's energy totals — `production.wattHoursToday`, `consumption.wattHoursToday` |

Total household consumption (production + net import) is the one value not directly available as a single field in `/ivp/meters/readings`. It is derived at the data source boundary — the screen receives a ready-to-display observation. The UI performs no arithmetic.

## The "No Calculations in the UI" Principle

`How-We-Think.md` is explicit: Telemetry observes and transforms observations into information. It does not become an intelligence layer. The data source is the correct place to resolve the consumption figure — it is a data normalisation step at the transport boundary, not a business rule. The screen receives a value, not a formula.

## What Changed

- `src/data/DataSourceManager.h/.cpp` — new; composes sources, implements `IDataSource`
- `src/data/sources/api/ApiDataSource.h/.cpp` — new; polls Envoy local API, writes to `SensorRepository`
- `src/data/sources/api/ApiMappings.h` — new; documents endpoint structure and sensor ID bindings
- `src/data/sources/mqtt/Topics.h` — `Envoy` namespace removed; MQTT covers weather sensors only
- `src/data/sources/mqtt/TopicMappings.cpp` — solar rows removed
- `src/data/sources/mqtt/TopicMappings.h` — `SolarSensorIds.h` include removed
- `src/config/secrets.h` — `EnvoySecrets::HOST` added
- `src/config/config_defaults.h` — `CFG_ENVOY_POLL_MS` added (default 10 000 ms)
- `src/config/config.h` — `EnvoyConfig` namespace added
- `src/config/config_override.h` — Envoy section added
- `src/config/secrets.example.h` — `EnvoySecrets` added
- `src/main.cpp` — `DataSourceManager` wired with both sources

## Data Flow (current)

```text
MqttDataSource          ApiDataSource
(weather — MQTT push)   (solar — HTTP poll, 10 s)
        │                       │
        └───────────┬───────────┘
                    ▼
          DataSourceManager
          (implements IDataSource)
                    │
                    ▼
          SensorRepository
                    │
          ┌─────────┴──────────┐
          ▼                    ▼
   WeatherScreen          SolarScreen
```
