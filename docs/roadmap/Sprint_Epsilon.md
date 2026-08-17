# Sprint Epsilon — Screen Navigation

## Objective

> **Establish screen-to-screen navigation as an explicit contract between Screens and `ScreenManager`, without compromising the interaction architecture established in Sprint Delta.**

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

Sprint Epsilon answered the next question:

> **How does contextual interpretation become a request to change Screens without giving the Screen ownership of navigation?**

The answer is:

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
                              │
                              ▼
                         transition
```

Navigation is therefore **one possible interpretation** of an input event, not the universal destination of input.

---

# Phase 1 — Define the Navigation Contract

**Status: Complete**

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

The key rule is:

> **Local meaning stays local. Meaning that crosses a responsibility boundary gets an explicit contract.**

Therefore Telemetry deliberately does **not** introduce a framework-wide `LocalAction` vocabulary for every Screen.

---

# Phase 2 — Define `ScreenIntent`

**Status: Complete**

`ScreenIntent` is the explicit contract between a Screen and `ScreenManager`.

Its current vocabulary is deliberately small:

```text
ScreenIntent
├── NONE
├── NAVIGATE + target
└── BACK
```

A Screen can therefore say:

```text
NAVIGATE(ControlPanel)
```

without knowing how `ControlPanelScreen` is instantiated or activated.

Likewise:

```text
BACK
```

does not identify a destination. The navigation policy belongs to `ScreenManager`.

`ScreenIntent` does not:

- perform navigation
- reference `ScreenManager`
- describe physical input
- replace `InputAction`
- represent local Screen behaviour
- implement navigation history

### Outcome

A minimal, explicit Screen → `ScreenManager` navigation contract.

---

# Phase 3 — Implement the Navigation Contract

**Status: Complete**

## 3.1 — Independent `ScreenKind` Contract

`ScreenKind` was extracted from `Screen.h` into:

```text
src/ui/ScreenKind.h
```

This allows `Screen`, `ScreenIntent`, and `ScreenManager` to share the screen vocabulary without unnecessary dependencies.

## 3.2 — Screens Return `ScreenIntent`

The Screen contract now returns:

```cpp
ScreenIntent onInput(const InputEvent& event);
```

Local behaviour returns `ScreenIntent()` because no Screen-level navigation was requested.

Pages remain contextual children of their Screen. They do not need to participate directly in the Screen → `ScreenManager` contract.

## 3.3 — `ScreenManager` Consumes `ScreenIntent`

`ScreenManager` now:

1. dispatches the `InputEvent` to the active Screen;
2. receives its `ScreenIntent`;
3. resolves a requested `ScreenKind`;
4. activates the target Screen;
5. handles `BACK` according to the current navigation policy.

The Screen remains unaware of the transition machinery.

## 3.4 — Transition Lifecycle

`ScreenManager::activate()` is the sole Screen transition boundary:

```text
Current Screen
      │
      ▼
   leave()
      │
      ▼
currentScreen_ = target
      │
      ▼
select active input profile
      │
      ▼
   enter()
```

`ScreenManager` owns the active Screen and the transition lifecycle.

---

# Touch Profiles

Sprint Epsilon also completed the first context-dependent touch translation model.

The same physical interaction can produce different semantic input depending on the active Screen:

```text
Weather       → left edge = PREVIOUS_SCREEN
ControlPanel  → left edge = BACK
```

`TouchManager` therefore applies a profile selected by `ScreenManager`.

The profile is a **physical-to-semantic translation mechanism**. It does not know application-level meanings such as `About`, `Connectivity`, or `Toggle clock format`.

Debouncing was also introduced so that a finger held across a Screen transition does not create a second event in the newly activated context.

The stable reference is:

```text
 docs/reference/TOUCH_PROFILES.md
```

---

# Thin Slice

The first complete navigation path was:

```text
WeatherScreen
     │
     │ ScreenIntent::NAVIGATE
     ▼
ScreenManager
     │
     ▼
SolarScreen / ControlPanelScreen
```

and:

```text
ControlPanelScreen
     │
     │ ScreenIntent::BACK
     ▼
ScreenManager
     │
     ▼
previous Screen according to current policy
```

The Control Panel also demonstrated the distinction between Screen-level and Page-level navigation:

```text
ControlPanelScreen
     │
     ├── child page → another page
     │
     └── root + BACK → ScreenIntent::BACK
```

---

# Definition of Done

Sprint Epsilon is complete when:

- [x] Screen-level navigation has an explicit ownership contract.
- [x] `ScreenKind` is an independent vocabulary.
- [x] `ScreenIntent` is an independent contract.
- [x] Screens return `ScreenIntent`.
- [x] `ScreenManager` consumes `ScreenIntent`.
- [x] Screen transitions are centralised in `ScreenManager`.
- [x] Local page behaviour remains below the Screen boundary.
- [x] Screen-to-Screen navigation works through `ScreenIntent`.
- [x] BACK returns according to the current navigation policy.
- [x] Touch profiles provide context-specific physical-to-semantic mapping.
- [x] Touch debouncing prevents cross-screen ghost interactions.
- [x] No Screen directly manipulates `ScreenManager`.

---

# What Sprint Epsilon Deliberately Did Not Solve

Sprint Epsilon established navigation architecture without prematurely building a general routing framework.

The following remain future architectural questions:

- navigation history and stack policy
- transition animation
- deep links
- arbitrary routing
- more complex navigation graphs
- additional input sequence recognition
- system-level intent handling beyond the current boundary

These should only be introduced when a real architectural pressure requires them.

---

# Looking Forward — A New Data Pressure

The introduction of Solar exposed a second architectural pressure outside the navigation boundary.

A second domain brought its own observations and its own source requirements. The project consequently had to answer two new questions:

> **Who owns sensor identity when there is more than one domain?**

and:

> **Should data sources be organised around products, or around source mechanisms?**

Those questions form the subject of **Sprint Zeta — Domain Data and Source Architecture**.
