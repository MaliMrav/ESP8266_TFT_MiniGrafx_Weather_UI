# Sprint Epsilon — Screen Navigation

## Objective

> **Establish screen navigation as an explicit contract between a Screen and ScreenManager, without compromising the interaction architecture established in Sprint Delta.**

Sprint Delta established an important distinction:

> **InputManager normalises interaction.  
> The appropriate context interprets it.  
> ScreenManager owns screen transitions.**

Sprint Epsilon builds the navigation contract on top of that boundary.

The central question is therefore:

> **How does a Screen express an intention to change screens without directly owning or manipulating ScreenManager?**

Not:

> "How does WeatherScreen call ControlPanelScreen?"

That would introduce the wrong dependency direction.

---

# The Architectural Context

The interaction architecture established in Sprint Delta is:

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
ScreenManager
      │
      ├── System-level event?
      │       └── System context handles it
      │
      └── Otherwise
              │
              ▼
          Active Screen
              │
              ▼
          Contextual interpretation
```

An `InputEvent` does not necessarily mean navigation.

It may represent:

```text
TAP
SELECT
SCROLL_UP
SCROLL_DOWN
INCREASE
DECREASE
```

and the active context may interpret those events as:

```text
Toggle clock format
Scroll a menu
Select an item
Change a value
Request screen navigation
Request a system operation
```

Therefore:

> **Navigation is one possible interpretation of an input event, not the definition of an input event.**

This distinction is fundamental to the architecture.

---

# The Navigation Boundary

Sprint Epsilon introduces one additional boundary:

```text
InputEvent
      │
      ▼
Active Screen
      │
      │ interprets interaction
      ▼
ScreenIntent
      │
      ▼
ScreenManager
      │
      │ executes transition
      ▼
Target Screen
```

The responsibilities are deliberately separated.

### InputManager

Normalises physical interaction into semantic `InputEvent` objects.

It does not know about screens.

### Active Screen

Interprets the `InputEvent` within its own context.

It may determine that an interaction means:

> "The user wants to leave this screen."

It expresses that intention through the navigation contract.

### ScreenIntent

Represents the Screen's intended navigation outcome.

It is the contract between the Screen and `ScreenManager`.

### ScreenManager

Owns the actual screen transition.

It decides how the requested transition is performed and manages screen lifecycle.

---

# What We Must Not Do

A Screen must not directly manipulate `ScreenManager` in order to navigate.

We do not want:

```text
WeatherScreen
      │
      └── calls ScreenManager
                │
                └── changeScreen(...)
```

because that makes the Screen responsible for both:

1. interpreting the interaction
2. executing navigation infrastructure

Instead:

```text
WeatherScreen
      │
      │ interprets
      ▼
ScreenIntent
      │
      ▼
ScreenManager
      │
      │ executes
      ▼
ControlPanelScreen
```

This keeps the dependency direction clear.

> **A Screen may request navigation.  
> ScreenManager owns navigation.**

---

# The Most Important Distinction

We now have three distinct layers of meaning.

## 1. Physical Interaction

```text
Touch
Encoder
Joystick
Keyboard
Switch
```

These are hardware-specific mechanisms.

---

## 2. Semantic Input

```text
TAP
SELECT
SCROLL_UP
SCROLL_DOWN
BACK
INCREASE
DECREASE
```

These describe **what the user did**, independently of the device that produced it.

This is the responsibility established by Sprint Delta.

---

## 3. Contextual Interpretation

The active context determines what that interaction means **here**.

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
        └── No  → no action
```

Or:

```text
TAP + position
        │
        ▼
WeatherScreen
        │
        ▼
Navigation control?
        │
        └── Yes → ScreenIntent
```

The same semantic input may therefore produce completely different outcomes depending upon context.

That is intentional.

---

# Navigation Is Not an InputAction

This distinction is especially important.

An action such as:

```text
TAP
```

belongs to the **input vocabulary**.

An action such as:

```text
OPEN_CONTROL_PANEL
```

does not.

`OPEN_CONTROL_PANEL` is contextual application meaning.

Likewise:

```text
TOGGLE_TIME_FORMAT
TOGGLE_TEMPERATURE_UNIT
OPEN_CONNECTIVITY_PAGE
RESET_TO_FACTORY_DEFAULTS
BEGIN_OTA_UPDATE
```

do not belong in `InputAction`.

The architecture therefore remains:

```text
InputAction
    │
    │ What did the user do?
    ▼
InputEvent
    │
    │
    ▼
Context
    │
    │ What does it mean here?
    ▼
Contextual outcome
```

One possible contextual outcome is:

```text
ScreenIntent
```

Another might be:

```text
Local state change
```

And another might be:

```text
System request
```

Sprint Epsilon is concerned specifically with the first of these.

---

# Sprint Epsilon Phases

## Phase 1 — Define the Navigation Contract

Establish the architectural vocabulary for navigation.

Determine:

- what constitutes a Screen
- what constitutes a screen transition
- what `ScreenManager` owns
- what the active Screen owns
- what constitutes a navigation request
- how a Screen communicates that request

The key architectural decision is:

> **Navigation execution belongs to `ScreenManager`; navigation intention belongs to the active context.**

### Outcome

A clearly defined navigation ownership model.

---

## Phase 2 — Define `ScreenIntent` as the Contract

Define `ScreenIntent` as the explicit contract between a Screen and `ScreenManager`.

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

The Screen does not manipulate the `ScreenManager`.

It expresses an intended navigation outcome.

`ScreenManager` consumes that outcome and remains responsible for the transition.

### Outcome

A minimal, explicit `ScreenIntent` contract.

---

## Phase 3 — Implement the `ScreenIntent` Model

Introduce the actual representation of navigation intent into the framework.

The MVP should establish only the navigation concepts currently required.

For example, the contract may eventually need to represent concepts such as:

```text
No navigation requested
Next screen
Previous screen
Back
Specific screen
```

The precise vocabulary should be determined during implementation rather than prematurely expanded for hypothetical future requirements.

### Outcome

A minimal, compilable `ScreenIntent` model.

---

## Phase 4 — Integrate `ScreenIntent` with `Screen`

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

The important requirement is the boundary, not a particular implementation technique.

### Outcome

Screens can express navigation requests without directly manipulating navigation infrastructure.

---

## Phase 5 — Integrate `ScreenIntent` with `ScreenManager`

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
      │       └── remain on current screen
      │
      └── Navigation requested
              │
              ▼
          execute transition
```

This establishes the architectural ownership boundary:

> **The Screen expresses intent.  
> The ScreenManager executes the transition.**

### Outcome

Navigation ownership is enforced by the implementation.

---

## Phase 6 — Implement and Validate the Thin Slice

The first demonstrable navigation path should be deliberately small:

```text
WeatherScreen
      │
      │ navigation request
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
      │ BACK
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

The `ScreenManager` remains responsible for ensuring that:

- the old Screen is left correctly
- the new Screen is entered correctly
- there is exactly one active Screen
- navigation state remains unambiguous
- Screens do not manipulate transition infrastructure directly

---

# Validation Against the Interaction Architecture

The completed Sprint Epsilon architecture should correctly handle all of the following.

### Local interaction

```text
TAP
 │
 ▼
WeatherScreen
 │
 └── local interpretation
       │
       └── toggle clock format
```

No navigation occurs.

---

### Screen navigation

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

---

### System-level interaction

```text
InputEvent
 │
 ▼
ScreenManager
 │
 └── system-level event
        │
        ▼
   System context
```

The event does not become navigation merely because it originated from an input device.

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

# The Thin Slice

The initial implementation should ultimately demonstrate:

```text
                    ┌──────────────────────┐
                    │    WeatherScreen     │
                    │                      │
                    │  Interprets semantic │
                    │  input in context    │
                    └──────────┬───────────┘
                               │
                               │ ScreenIntent
                               ▼
                    ┌──────────────────────┐
                    │    ScreenManager     │
                    │                      │
                    │  Owns transition     │
                    └──────────┬───────────┘
                               │
                               ▼
                    ┌──────────────────────┐
                    │ ControlPanelScreen   │
                    │                      │
                    │  Owns ControlPageKind│
                    └──────────────────────┘
```

Then:

```text
ControlPanelScreen
        │
        │ BACK
        ▼
ScreenManager
        │
        ▼
WeatherScreen
```

This gives us a complete, demonstrable navigation architecture while deliberately avoiding premature solutions for:

- navigation history
- arbitrary routing
- nested navigation stacks
- deep links
- animations
- gestures
- complex transition policies
- application-specific destinations

Those can be introduced later if the architecture actually requires them.

---

# Definition of Done

Sprint Epsilon is complete when:

- [ ] `ScreenIntent` exists as an explicit navigation contract.
- [ ] Screens can express navigation intent without directly manipulating `ScreenManager`.
- [ ] `ScreenManager` owns actual screen transitions.
- [ ] `InputEvent` remains independent of navigation.
- [ ] `InputAction` remains independent of application-specific destinations.
- [ ] Local contextual interactions continue to work without navigation.
- [ ] System-level interactions remain distinct from navigation.
- [ ] `WeatherScreen → ControlPanelScreen` works.
- [ ] `ControlPanelScreen → WeatherScreen` works.
- [ ] Screen lifecycle (`leave()` / `enter()`) remains correct.
- [ ] The resulting implementation matches the architectural principles established in `How-We-Think.md`.

---

# Architectural Principle

Sprint Epsilon reinforces a simple rule:

> **A context interprets interaction.  
> A Screen may express navigation intent.  
> ScreenManager owns navigation.**

This preserves the separation established in Sprint Delta while giving the framework a clean mechanism for moving between screens.

The result should feel inevitable:

```text
Input
  ↓
Interpretation
  ↓
Intent
  ↓
Ownership
  ↓
Execution
```

Each layer answers a different question.

And each layer remains responsible for only the thing it is actually qualified to decide.