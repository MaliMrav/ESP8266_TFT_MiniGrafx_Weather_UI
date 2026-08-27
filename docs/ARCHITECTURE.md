# Architecture

Telemetry began as a weather station.

It is now becoming something more deliberate.

The project is evolving into a reusable firmware framework whose first application happens to be a weather display.

That distinction explains most of the architecture.

The question is no longer:

> "How do we make this particular device work?"

It is:

> "How do we build a system in which different applications, data sources, displays and input devices can coexist without knowing unnecessary details about one another?"

---

## The Architectural Direction

The overall direction is:

```text
Application
    │
    ▼
Framework Capabilities
    │
    ▼
Hardware Abstractions
    │
    ▼
Drivers
    │
    ▼
Physical Hardware
```

The application should express policy.

Capabilities should provide reusable services.

Drivers should translate hardware.

Physical devices should remain implementation details.

The architecture deliberately moves knowledge downward.

---

# Platform Composition

Telemetry is one framework. Different targets are instantiated through different
**PlatformIO environments** rather than separate framework architectures.

The shared framework defines contracts and reusable capabilities. The PlatformIO
environment selects which capabilities are composed into a particular build.

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

The reference profiles have different purposes:

- **ESP8266** — observation-oriented, resource-constrained.
- **ESP32/CYD** — observation-and-control-oriented, resource-expanded.

The framework is not forked to accommodate those differences. PlatformIO composition
determines which capabilities a target instantiates.

This means a capability can legitimately exist in the framework while not being
instantiated by a constrained target. For example, an API/TLS data source may belong
to Telemetry's architecture while remaining outside the ESP8266 composition.

> **Telemetry defines the architecture. Composition determines which capabilities a target can instantiate.**

The source tree should therefore describe shared architecture and capabilities, while
`platformio.ini` describes concrete target compositions.

# The Major Boundaries

## The Display Boundary

Application code should not depend directly on a display driver.

The intended relationship is:

```text
Application / Screen
        │
        ▼
DisplayManager
        │
        ▼
Display Driver
        │
        ▼
Physical Display
```

A screen should be able to say:

```cpp
display.drawString(...);
display.drawLine(...);
display.commit();
```

It should not need to know whether the underlying hardware is:

- ILI9341
- ST7735
- another display controller

The screen describes what should be rendered.

The display capability decides how that rendering reaches the hardware.

This is the display boundary.

---

## The Data Boundary

The application should not care where observations came from.

The conceptual flow is:

```text
Observation
      │
      ▼
Data Source
      │
      ▼
Repository / Model
      │
      ▼
Application
      │
      ▼
Presentation
```

The source may be:

- MQTT
- a local HTTP API
- a local sensor
- a file
- another transport
- a future data source not yet imagined

The application should consume information through a stable contract.

It should not need to know how the data arrived.

This keeps observation separate from presentation.

It also preserves the distinction between:

```text
Observation
```

and:

```text
Information
```

Telemetry observes and presents information.

It deliberately does not become an intelligence layer.

---

## Multiple Sources, One Contract

A real application rarely has a single data source.

A Screen may consume observations from several independent mechanisms at the same time:

```text
                     DataSourceManager
                            │
              ┌─────────────┼─────────────┐
              ▼             ▼             ▼
        MqttDataSource   Local I/O     ApiDataSource
              │             │             │
         TopicMappings   I2C / OneWire  ApiMappings
              │             │             │
              └─────────────┼─────────────┘
                            ▼
                    SensorRepository
                            │
                  ┌─────────┴─────────┐
                  ▼                   ▼
             WeatherScreen        SolarScreen
```

The same model scales to a smart wall panel:

```text
Room temperature ──► I2C
Room humidity ──────► I2C
Weather forecast ───► API
Light state ────────► MQTT
HVAC state ─────────► Modbus
Solar production ───► MQTT / API
                  \    |    /
                   \   |   /
                    ▼  ▼  ▼
                SensorRepository
                       │
                       ▼
                SmartWallPanel
```

`IDataSource` is the source contract. `DataSourceManager` composes source implementations. Neither knows which Screen will consume the resulting observations.

The important rule is:

> **A Screen consumes observations by identity, not by source.**

---

## Domain Observation Identity

The original design used a flat integer ID as both domain identity and repository position.

That is a hidden coupling.

A value such as:

```text
5
```

cannot tell a developer what it means. Worse, its meaning can become accidentally tied to array position or insertion order.

The stronger model separates three concerns:

```text
ObservationKey
      │
      ▼
Runtime registration / resolution
      │
      ▼
ObservationHandle
      │
      ▼
Repository storage slot
```

### ObservationKey

A stable, human-meaningful identity owned by the domain.

Examples:

```text
solar.power.production
solar.power.consumption
room.temperature
room.humidity
livingroom.lamp.state
```

The key describes the knowledge the application is interested in.

### ObservationHandle

An opaque runtime reference allocated by the repository or registration layer.

Application code does not invent its numeric value.

### Storage slot

The physical location used by `SensorRepository`.

It is an implementation detail.

The repository may use an array, pool, indexed record list, or another compact representation without exposing that storage scheme as domain architecture.

Therefore:

> **Identity describes meaning. Storage describes implementation.**

Adding an observation must never require renumbering an existing observation.

---

## Domain Ownership

Domains still own the vocabulary of observations they introduce, but they no longer own storage indices.

For example:

```text
Weather domain
    └── weather.* ObservationKeys

Solar domain
    └── solar.* ObservationKeys

Room domain
    └── room.* ObservationKeys
```

A Screen may use observations from several domains at once.

A Solar screen could display:

```text
solar.power.production
solar.power.battery
room.temperature
weather.forecast.condition
```

without changing the meaning of any of those identities.

This removes positional coupling while also avoiding the opposite mistake of making every domain responsible for repository storage layout.

---

## Source Mechanisms, Not Products

Data sources are organised by reusable source mechanism:

```text
src/data/sources/
├── mqtt/
├── api/
├── serial/
└── modbus/
```

A product or vendor is not itself a source category.

For example, an API-backed source may currently communicate with an Envoy, but the architectural capability is still:

```text
API
```

The same mechanism may later communicate with AlphaESS or another API provider.

This keeps product knowledge below the source-mechanism boundary.

The desired flow is:

```text
External system
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
SensorRepository
      │
      ▼
Screen / Application
```

---

## Dynamic Source Resolution

A stable observation identity does not require a single permanent source.

A runtime may have several candidates for the same ObservationKey:

```text
room.temperature
      │
      ├── local BME280
      ├── MQTT / Home Assistant
      └── Modbus controller
```

Resolution can eventually apply a policy based on:

- source availability
- source priority
- freshness
- redundancy
- platform capability

Round-robin is one possible policy where equivalent sources genuinely exist, but it is not a default architectural assumption. The important abstraction is:

> **Identity is stable; source resolution is dynamic.**

---

# The Interaction Boundary

Input is semantic, but semantic input is not the same thing as application meaning.

The conceptual flow is:

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

A physical interaction might originate from:

- touch
- buttons
- a five-way switch
- a rotary encoder
- a keyboard
- a joystick

The input source translates the physical interaction into a semantic action.

For example:

```text
Touch
  ↓
TAP + position
```

or:

```text
Rotary encoder turned
  ↓
INCREASE / DECREASE
```

The input device should not need to know what the application will do with that action.

---

## InputManager Does Not Decide Application Meaning

`InputManager` has a deliberately limited responsibility.

It:

- receives events from input sources
- normalises physical interaction into semantic events
- queues events
- delivers events to the appropriate higher-level context

It does **not** decide that:

```text
SCROLL_DOWN
```

means:

```text
OPEN_CONNECTIVITY_PAGE
```

It does **not** decide that:

```text
TAP + position
```

means:

```text
TOGGLE_TIME_FORMAT
```

And it does **not** decide that:

```text
A particular key sequence
```

means:

```text
RESET_DEVICE
```

Those are contextual decisions.

The input system describes interaction.

The appropriate context interprets it.

---

## The Active Screen Is the Contextual Boundary

The active Screen is the normal application boundary at which semantic interaction acquires local meaning.

A Screen provides:

```text
Active Screen
│
├── Presentation
├── Zones
└── Local Context
```

The Screen knows what is currently being presented, which areas are interactive, and what semantic input means within that context.

For example:

```text
TAP + position
        │
        ▼
WeatherScreen
        │
        ▼
Clock area?
        │
        ├── Yes → toggle clock presentation
        │
        └── No  → no local action
```

The local behaviour belongs to the Screen.

It does not need to become a framework-wide `InputAction`.

A different Screen may interpret the same input differently.

For example:

```text
SCROLL_LEFT
        │
        ▼
SolarPowerScreen
        │
        └── move historical graph backwards
```

No change to the generic input vocabulary is required merely because a new Screen has been introduced.

This is one of the tests of whether the framework boundary is healthy.

---

## Contextual Outcomes

Once the active context interprets an `InputEvent`, there are three important architectural outcomes:

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

### Local behaviour

The interaction remains inside the current Screen or Page.

Examples include:

- toggling clock presentation
- changing a display format
- moving menu selection
- scrolling information
- adjusting a value
- capturing a calibration point

No navigation contract is required.

### ScreenIntent

The interaction means that the user wants to move between Screens.

The active context may express that intent, but does not execute the transition.

`ScreenManager` owns the transition.

### SystemIntent

The interaction means something that affects the device or system beyond the current Screen.

Examples include reset, factory reset, AP mode, recovery mode and OTA requests.

The active Screen is not necessarily the owner of such operations.

---

## Navigation Is Only One Possible Meaning

This distinction is central to the architecture.

An input event may result in:

```text
InputEvent
      │
      ▼
Contextual interpretation
      │
      ├── Local behaviour
      │
      ├── ScreenIntent
      │
      └── SystemIntent
```

Therefore:

> **ScreenManager owns screen-level navigation, but navigation is only one possible interpretation of an input event.**

The active context determines the meaning.

The owner of the resulting scope executes the operation.

---

## Navigation Ownership

When an interaction is interpreted as a request to change the active Screen, the responsibility divides cleanly:

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

`ScreenManager` owns:

- which Screen is active
- leaving the current Screen
- activating the next Screen
- entering the new Screen
- screen-level lifecycle transitions
- navigation history

The Screen does not directly manipulate `ScreenManager`.

It expresses intent.

The navigation owner executes the transition.

This preserves the ownership boundary:

> **The context interprets. The Screen may express navigation intent. `ScreenManager` owns navigation.**

---

## Navigation History

Navigation in Telemetry is hierarchical, not a flat carousel.

Screens may have internal depth (pages within a screen). A ring model breaks down the moment any screen has internal hierarchy, because lateral navigation and back navigation would share the same gesture with two different meanings.

`ScreenManager` therefore maintains a shallow history stack.

- `ScreenIntent::navigateTo()` pushes the current screen onto the stack before activating the destination.
- `ScreenIntent::back()` pops the stack, returning to wherever the user came from.
- `activate()` — used for boot and calibration — does not push to the stack. Those transitions are not part of user-navigable history.

Screens have no knowledge of the stack. The history is owned entirely by `ScreenManager`.

That is the architecture working correctly: the Screen expresses intent, the owner of navigation decides what that means.

---

## System-Level Input

Not every input belongs to the visible Screen.

A keyboard sequence, button sequence, or other interaction may request an operation affecting the entire device.

Examples include:

```text
Hidden key sequence
        │
        ▼
System Context
        │
        ├── Factory reset
        ├── Enter AP mode
        ├── Request OTA update
        └── Reboot device
```

These are not screen navigation.

The active Screen should not need to know that a particular key sequence means reset.

Likewise, the input source should not know that the application contains a `ConnectivityPage`.

The architecture therefore separates:

```text
Physical interaction
        ↓
Semantic input
        ↓
Contextual interpretation
        ↓
Scope-specific ownership
```

---

## Why Not Create a Global LocalAction Vocabulary?

It may be tempting to define:

```text
WeatherAction
SolarPowerAction
ControlPanelAction
CalibrationAction
ConnectivityAction
```

But doing so would create a second application-specific input language.

The framework already provides a generic semantic vocabulary:

```text
TAP
SELECT
SCROLL_UP
SCROLL_DOWN
INCREASE
DECREASE
...
```

The active context interprets those actions directly.

Only outcomes that cross an architectural boundary require an explicit contract such as `ScreenIntent` or `SystemIntent`.

> **Local meaning stays local. Meaning that crosses a responsibility boundary gets an explicit contract.**

This keeps the framework vocabulary small and reusable.

---

## The Input Ownership Model

The complete model is:

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
      │
      │ contextual interpretation
      ▼
┌─────────────────────────────────────────┐
│                                         │
│  Local behaviour   ScreenIntent   SystemIntent
│       │                 │               │
│       ▼                 ▼               ▼
│  Screen state      ScreenManager   System Context
│                         │
│                         ▼
│                  Screen transition
│                                         │
└─────────────────────────────────────────┘
```

This separates:

```text
InputManager
    owns normalisation and delivery

Active Context
    owns interpretation

ScreenManager
    owns screen-level transitions

System Context
    owns system-level operations
```

No layer becomes the universal owner of interaction.

---

# The System Lifecycle

The system is orchestrated through the application lifecycle.

Conceptually:

```text
Boot
  │
  ▼
Initialise Hardware
  │
  ▼
Initialise Capabilities
  │
  ▼
Initialise Services
  │
  ▼
Select Initial Screen
  │
  ▼
Run
```

During normal operation:

```text
SystemManager
      │
      ├── OTA
      ├── Data Source
      ├── InputManager
      └── ScreenManager
```

The system manager coordinates the lifecycle.

It should not become the owner of every responsibility.

Coordination is not the same as implementation.

---

# Dependency Direction

Dependencies should flow downward.

```text
Application
    │
    ▼
Capabilities
    │
    ▼
Drivers
    │
    ▼
Hardware
```

The opposite direction should be avoided.

A display driver should not know about a weather screen.

A touch controller should not know about the Control Panel.

An MQTT data source should not know how a temperature is rendered.

The higher layer expresses policy.

The lower layer provides capability.

---

# The Repository Should Reflect the Architecture

The directory structure is part of the architecture.

A developer should be able to understand the broad design by browsing the repository.

For example:

```text
src/
├── data/
├── display/
├── hardware/
├── input/
├── models/
├── mqtt/
├── ota/
├── screens/
├── system/
├── touch/
├── ui/
└── wifi/
```

The exact structure may evolve.

The architectural intent should remain visible.

A repository should not require a developer to read every file before discovering where responsibilities live.

---

# Architecture Before Implementation

A recurring principle in Telemetry is:

> Architecture precedes implementation.

Before adding a feature, ask:

- What responsibility does this introduce?
- Which layer owns that responsibility?
- What should this code know?
- What should this code not know?
- Does the repository structure reflect the design?
- Does the abstraction reduce complexity?

Only then should implementation begin.

This is why architectural work may appear slower than simply writing code.

The time is not lost.

It is being spent making later code more inevitable.

---

# The Cost of Abstraction

Every boundary introduces a cost.

There may be:

- more files
- more interfaces
- more indirection
- more concepts to explain

That cost is justified only when the abstraction reduces complexity somewhere else.

The question is not:

> "Can we abstract this?"

It is:

> "Does this abstraction make the system easier to understand and evolve?"

This is especially important for interaction architecture.

The framework does not need a global action vocabulary for every possible Screen behaviour.

It needs only the abstractions that cross meaningful responsibility boundaries.

---

# Why These Boundaries Exist

Each architectural boundary answers a question.

### Why is there a DisplayManager?

Because application code should not depend directly on display hardware.

### Why is there a data boundary?

Because the application should not care where observations originate, how many sources are running, or what transport each source uses.

### Why are sensor IDs declared per-domain?

Because a global sensor enum forces every domain to know about every other domain. When a new screen is added, no existing domain file should need to change.

### Why is there an InputManager?

Because physical input devices should be interchangeable and Screens should consume semantic actions.

### Why does the active Screen interpret input?

Because meaning depends on the current presentation, zones and local context.

### Why does ScreenManager own transitions?

Because screen lifecycle and screen-level navigation should have one owner.

### Why is navigation only one possible interpretation of input?

Because an input event may instead produce local behaviour or a system-level operation.

### Why can system-level input bypass screen navigation?

Because not every input event belongs to the visible Screen.

These are not arbitrary layers.

They are answers to architectural pressures.

---

# The Guiding Principle

Telemetry is designed around a simple direction:

```text
Physical World
      │
      ▼
Capabilities
      │
      ▼
Semantic Information
      │
      ▼
Application
      │
      ▼
Human Understanding
```

The framework exists to make those transformations explicit.

The architecture is not there to make the project look sophisticated.

It is there to make the project understandable.

> **Good architecture should feel inevitable.**
