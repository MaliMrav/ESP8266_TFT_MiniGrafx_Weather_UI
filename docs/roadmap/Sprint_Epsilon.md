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

# Definition of Done

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
