# 005 — Who Owns Navigation?

## The Question

When an input event occurs, who decides what happens next?

At first, this appears to be a simple question about navigation.

A user touches the screen.

A button is pressed.

A rotary encoder is turned.

A joystick moves.

Perhaps the application should show another screen.

But screen navigation is only one possible consequence of an interaction.

The same input event might instead:

- scroll through a menu
- select an item
- change a displayed value
- toggle a presentation format
- enter a configuration mode
- reset the device
- enter Wi-Fi access-point mode
- request an OTA update
- restore factory defaults

This leads to a more fundamental question:

> **Who owns the interpretation of an interaction, and who owns the resulting operation?**

The answer is not one universal owner.

**Interpretation belongs to the appropriate context. Ownership follows scope.**

---

## Input Is Not Navigation

Telemetry deliberately separates physical interaction from the meaning assigned to it.

The input pipeline is:

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
```

At this point, the system knows what kind of interaction occurred.

For example:

```text
Touch at (120, 20)
        │
        ▼
TAP + position
```

Or:

```text
Joystick moved upward
        │
        ▼
SCROLL_UP
```

Or:

```text
Rotary encoder turned
        │
        ▼
INCREASE / DECREASE
```

The input system has normalised the physical interaction.

It has **not** yet decided what that interaction means to the application.

That distinction is deliberate.

> **Input produces interaction. Context produces meaning.**

---

## The Active Screen Is the Contextual Boundary

Once an `InputEvent` reaches the active application context, the active Screen provides the context in which the interaction acquires meaning.

A Screen therefore has three closely related responsibilities:

```text
Active Screen
│
├── Presentation
│
├── Zones
│
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

The clock-format change is local behaviour.

It does not need a framework-wide `TOGGLE_TIME_FORMAT` action.

Likewise, a future Solar Power screen might interpret:

```text
SCROLL_LEFT
        │
        ▼
SolarPowerScreen
        │
        └── move historical graph to the previous interval
```

The generic input vocabulary does not need to know anything about solar power.

This is what makes the framework extensible.

---

## The Same Input Can Mean Different Things

Consider:

```text
SCROLL_DOWN
```

On a weather screen, it might mean:

```text
SCROLL_DOWN
      │
      ▼
WeatherScreen
      │
      └── move focus to the next area
```

On the Control Panel:

```text
SCROLL_DOWN
      │
      ▼
ControlPanelScreen
      │
      └── select the next menu item
```

On an information page:

```text
SCROLL_DOWN
      │
      ▼
AboutPage
      │
      └── scroll the information
```

The input action is the same.

The interpretation is different.

That is not ambiguity.

That is contextual interpretation.

---

## Three Possible Outcomes

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

These outcomes have deliberately different scopes.

### Local behaviour

The interaction remains within the active Screen or Page.

Examples:

- toggle clock presentation
- change a displayed format
- move menu selection
- scroll information
- adjust a value
- capture a calibration point

No navigation contract is required.

### ScreenIntent

The interaction means that the user wants to move between Screens.

The active context may express that intent, but it does not execute the transition.

`ScreenManager` owns the transition.

### SystemIntent

The interaction means something that affects the device or system beyond the current Screen.

Examples include:

- reset
- factory reset
- enter AP mode
- request OTA update
- enter recovery mode

These belong to a system-level owner rather than the visible Screen.

---

## Navigation Is Only One Possible Interpretation

A semantic `InputEvent` therefore follows this conceptual path:

```text
InputEvent
      │
      ▼
Active Screen / Context
      │
      │ What does it mean here?
      ▼
Interpretation
      │
      ├── Local behaviour
      │       └── Screen state / Page state
      │
      ├── ScreenIntent
      │       └── ScreenManager
      │
      └── SystemIntent
              └── System Context
```

This is the refinement that matters most for the architecture.

> **ScreenManager owns navigation, but navigation is only one possible interpretation of an input event.**

The Screen is therefore not a miniature `ScreenManager`.

It interprets interaction in context and may express intent when that meaning crosses an architectural boundary.

---

## System-Level Interaction

Some interactions have nothing to do with the user interface currently being displayed.

For example, a special sequence from a keyboard, button matrix, or other input device might request:

```text
Key Sequence
      │
      ▼
System Context
      │
      ├── Reset device
      ├── Restore factory defaults
      ├── Enter Wi-Fi AP mode
      └── Request OTA update
```

This is conceptually similar to system-level keyboard combinations such as:

```text
CTRL + ALT + DEL
```

The physical keys are not themselves a request to reboot.

The system interprets the combination within a broader context.

The same principle applies to embedded systems.

A hidden button sequence might mean:

```text
InputEvent sequence
        │
        ▼
System Context
        │
        └── Enter recovery mode
```

These interactions should not be routed through a screen merely because a screen happens to be visible.

The active screen is not necessarily the owner of every input event.

---

## Screen-Level Navigation

If an interaction is interpreted as a request to change the active Screen, the responsibility divides cleanly:

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

The active Screen decides that the interaction means:

> "The user wants to leave this context and move to another Screen."

It does **not** manipulate `ScreenManager` directly.

`ScreenManager` owns:

- which Screen is active
- leaving the current Screen
- activating the next Screen
- entering the new Screen
- screen-level lifecycle transitions

This preserves the architectural boundary:

> **The context interprets. The Screen may express navigation intent. `ScreenManager` executes the transition.**

---

## In-Screen Interaction

If the event does not represent a system operation or screen-level navigation, it remains within the active context.

For example:

```text
InputEvent
      │
      ▼
WeatherScreen
      │
      ├── TAP + clock position
      │       └── toggle clock presentation
      │
      └── SCROLL_DOWN
              └── local interpretation
```

Or:

```text
InputEvent
      │
      ▼
ControlPanelScreen
      │
      ▼
ControlMenuPage
      │
      └── SCROLL_DOWN
              └── select the next item
```

The input event remains generic.

The Screen and its local context give it meaning.

---

## Why Not Create a Global LocalAction Vocabulary?

A tempting design would be to create a separate action enum for every Screen:

```text
WeatherAction
SolarPowerAction
ControlPanelAction
CalibrationAction
ConnectivityAction
...
```

That would create a second application-specific input language and force the framework to understand details belonging to individual applications.

It is unnecessary.

The framework already provides the semantic input vocabulary:

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

Only when the resulting meaning crosses an architectural boundary do we introduce an explicit contract such as `ScreenIntent` or `SystemIntent`.

> **Local meaning stays local. Meaning that crosses a responsibility boundary gets an explicit contract.**

This keeps the framework vocabulary small and reusable.

---

## The Ownership Contract

The resulting architecture is:

```text
Physical Input
      │
      ▼
Input Source
      │
      └── Detect physical interaction
              │
              ▼
        InputManager
              │
              ├── Normalise interaction
              ├── Queue event
              └── Deliver event
                      │
                      ▼
                 InputEvent
                      │
                      ▼
                Active Context
                      │
          ┌───────────┼───────────┐
          ▼           ▼           ▼
       Local       Screen       System
      behaviour    Intent       Intent
          │           │           │
          ▼           ▼           ▼
      Screen /     Screen       System
        Page       Manager       Context
```

Each layer has a distinct responsibility.

### Input Source

Answers:

> What physical interaction occurred?

Examples:

- touch
- button press
- encoder movement
- joystick movement
- keyboard input

### InputManager

Answers:

> How do we represent and transport that interaction independently of the physical device?

It produces semantic `InputEvent` objects.

### Active Context

Answers:

> What does this interaction mean here?

It may produce local behaviour, a `ScreenIntent`, or a `SystemIntent` depending on scope and ownership.

### ScreenManager

Answers:

> How is a requested Screen transition executed?

It owns Screen lifecycle and transitions.

### System Context

Answers:

> What should happen when an interaction represents a system-level operation?

Examples include reset, recovery, AP mode, factory reset and OTA operations.

### Active Page or Control

Answers:

> What does this interaction mean within this local area of the current Screen?

A Page or Control may participate in the contextual interpretation without becoming a framework-level input owner.

---

## Why Not Let Screens Execute Navigation?

A simpler design might allow:

```text
WeatherScreen
      │
      └── ScreenManager.show(ControlPanel)
```

This appears straightforward.

But it gives the Screen direct knowledge of navigation infrastructure and the collection of available Screens.

The dependency becomes:

```text
WeatherScreen
      │
      ▼
ScreenManager
```

That is the wrong ownership direction.

Instead:

```text
WeatherScreen
      │
      ▼
ScreenIntent
      │
      ▼
ScreenManager
```

The Screen expresses intent.

The owner of navigation executes it.

---

## Why Not Let InputManager Decide?

The opposite design is equally problematic.

If `InputManager` decides:

```text
TAP at (120, 20)
      │
      └── Open About Page
```

then the input system must understand:

- the current Screen
- the current Page
- the Screen layout
- application-specific controls
- system modes

The input system is no longer an input abstraction.

It has become an application controller.

That violates the separation established by Sprint Delta.

---

## The Deeper Principle

The architecture separates three questions:

```text
1. Interaction
      │
      ▼
What did the user do?

2. Interpretation
      │
      ▼
What does that interaction mean here?

3. Ownership
      │
      ▼
Which scope is responsible for the resulting operation?
```

For example:

```text
TAP + position
      │
      ▼
WeatherScreen
      │
      ├── clock area
      │      └── local behaviour
      │
      └── navigation control
             └── ScreenIntent
```

Or:

```text
Special key sequence
      │
      ▼
System Context
      │
      └── SystemIntent
```

The resulting operation is then handled by the owner of that scope.

This produces a simple architectural rule:

> **Interpretation is contextual. Ownership follows scope. Execution belongs to the owner.**

---

## The Framework Test

The architecture should remain valid if a completely new Screen is introduced.

Imagine adding:

```text
SolarPowerScreen
```

with zones for:

- solar production
- household consumption
- battery storage
- grid export

It may interpret:

```text
SCROLL_LEFT
```

as:

```text
Move historical graph backwards
```

while another Screen interprets the same input as something completely different.

No change to `InputAction` is required.

No global `SolarPowerAction` enum is required.

The new Screen simply provides its own contextual interpretation.

If adding a new application Screen requires modifying the generic input vocabulary, the abstraction boundary should be questioned.

That is one of the tests that tells us whether Telemetry is behaving like a framework rather than a single-purpose weather application.

---

## The Final Model

The complete interaction architecture is:

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

This is the architectural answer to:

> **Who owns navigation?**

**ScreenManager owns screen-level navigation.**

But navigation is only one possible interpretation of an input event.

The active context interprets the event.

Local meaning remains local.

Screen-level meaning becomes `ScreenIntent`.

System-level meaning becomes `SystemIntent`.

And each resulting operation is handled by the owner of its scope.

That separation is what allows the framework to grow without turning the input system into an application controller or every Screen into a navigation controller.
