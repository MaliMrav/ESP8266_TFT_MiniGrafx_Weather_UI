# Touch Profiles

## Overview

`TouchManager` translates physical touch interaction into semantic `InputEvent` objects.

The translation is **context-dependent**.

The same physical interaction can produce a different semantic input depending on the active screen.

That context is represented by a **touch profile**.

A touch profile therefore defines how physical touch is translated into the input vocabulary for a particular screen context.

```text
Physical touch
      │
      ▼
TouchManager
      │
      │  active touch profile
      ▼
Semantic InputEvent
      │
      ▼
Active Screen
      │
      │  contextual interpretation
      ▼
Local behaviour / ScreenIntent / SystemIntent
```

The important distinction is:

> **TouchManager determines what physically happened. The active context determines what that interaction means.**

The screen does not interpret raw touch coordinates as hardware input. It receives semantic input.

---

## Why Profiles Exist

Consider a touch at the left edge of the display.

On the Weather screen, it may mean:

```text
PREVIOUS_SCREEN
```

On the Control Panel, it means:

```text
BACK
```

The physical interaction is identical.

The semantic input is different.

A single fixed zone-to-action mapping cannot express this distinction without introducing screen-specific interpretation into the input layer.

Touch profiles provide the necessary translation boundary:

```text
Physical interaction
        │
        ▼
TouchManager
        │
        │  "What happened?"
        ▼
InputEvent
        │
        ▼
Active context
        │
        │  "What does it mean here?"
        ▼
Contextual behaviour
```

This preserves the interaction architecture established in Sprint Delta.

---

## Who Sets the Profile

`ScreenManager` sets the touch profile when it activates a screen.

The profile is selected from the `ScreenKind` of the newly active screen:

```text
ScreenManager::activate(screen)
      │
      ├── current screen → leave()
      │
      ├── select new active screen
      │
      ├── TouchManager::setProfile(...)
      │
      └── new screen → enter()
```

The profile therefore follows the active screen.

Screens do not manage their own `TouchManager` profiles.

This keeps the ownership boundary clear:

- `TouchManager` owns physical-to-semantic translation.
- `ScreenManager` owns screen activation.
- The active screen owns contextual interpretation.
- `ScreenManager` consumes `ScreenIntent` and owns screen transitions.

---

## Zone Layout

Profiles share the common physical zone definitions provided by `ScreenZones`.

The meaning assigned to those zones may vary between profiles.

```text
┌─────────────────────────────────┐
│           HEADER                │  → profile-dependent
├────┬───────────────────────┬────┤
│    │      TOP STRIP        │    │
│    ├───────────────────────┤    │
│ L  │                       │ R  │
│ E  │    CONTENT / TAP      │ I  │
│ F  │                       │ G  │
│ T  │                       │ H  │
│    ├───────────────────────┤ T  │
│    │     BOTTOM STRIP      │    │
└────┴───────────────────────┴────┘
```

The current physical zone boundaries, as defined by `ScreenZones.h`, are:

| Zone | Boundary |
|---|---|
| Header | `y < 58` |
| Left edge | `x < 40` |
| Right edge | `x >= 200` |
| Top strip | `y = 58–118`, inside edges |
| Bottom strip | `y = 260–320`, inside edges |
| Content area | Remaining area |

These are **physical boundaries**, not application semantics.

A profile assigns semantic meaning to the interactions occurring within them.

---

## Profiles

### Weather

Used by `WeatherScreen`.

| Zone | Action |
|---|---|
| Header | `TAP + position` |
| Left edge | `PREVIOUS_SCREEN` |
| Right edge | `NEXT_SCREEN` |
| Content area | `SELECT` |

The header interaction retains its position.

This allows `WeatherScreen` to perform local hit-testing, such as determining whether a tap occurred within the clock area.

The touch layer therefore does **not** contain knowledge such as "this coordinate is the clock."

---

### ControlPanel

Used by `ControlPanelScreen` and all pages within it.

| Zone         | Action          |
|--------------|-----------------|
| Left edge    | `BACK`          |
| Right edge   | `SELECT`        |
| Top strip    | `SCROLL_UP`     |
| Bottom strip | `SCROLL_DOWN`   |
| Content area | `TAP + position`|

The content-area tap retains its position so that the active page can perform local hit-testing where appropriate.

This allows a touch interaction and an encoder interaction to converge on the same semantic vocabulary:

```text
Touch ────────┐
              │
              ▼
          InputEvent
              ▲
              │
Encoder ──────┘
```

The pages therefore remain independent of the physical input device.

---

### Status

Used by `StatusScreen`.

| Zone | Action |
|---|---|
| Header | `BACK` |
| Left edge | `PREVIOUS_SCREEN` |
| Right edge | `NEXT_SCREEN` |
| Upper content | `SCROLL_UP` |
| Lower content | `SCROLL_DOWN` |
| Content area | `SELECT` |

The exact interpretation of these semantic actions remains the responsibility of the active screen.

---

### Calibration

Used by `CalibrationScreen`.

Calibration deliberately operates differently from normal application screens.

Touches produce:

```text
TAP + raw position
```

The raw position is required because calibration is responsible for establishing the coordinate correction that normal touch interaction will subsequently use.

Applying the calibration correction while determining that correction would introduce a circular dependency.

Calibration is therefore an explicit exception to the normal calibrated-touch path.

---

### Generic

The fallback profile.

Touches produce:

```text
TAP + position
```

No screen-specific semantic interpretation is performed by the profile.

The receiving context decides what the interaction means.

---

## Profile Switching and Debounce

When `ScreenManager` activates a new screen, it changes the active `TouchManager` profile.

Changing the profile does **not** reset the touch-state flag tracking whether a finger is currently being held.

This is deliberate.

Consider a navigation gesture:

```text
WeatherScreen
     │
     │ finger touches right edge
     ▼
NEXT_SCREEN
     │
     ▼
ScreenManager
     │
     ▼
ControlPanelScreen
```

The finger may still be physically touching the display when the new screen becomes active.

If the profile switch artificially reset the touch state, the still-active touch could be interpreted as a new interaction by the newly activated screen.

That could produce a **ghost interaction**.

The correct state boundary is the physical release of the finger.

`TouchManager::update()` therefore retains its touch state across a profile change and clears it when the physical touch is actually released.

This is an important distinction:

> **Changing context must not manufacture a new physical interaction.**

---

## Adding a New Profile

A new touch profile should be introduced when a screen requires a different mapping from **physical touch to semantic input**.

The architectural sequence is:

1. Define the physical interaction vocabulary required by the screen.
2. Determine which existing zones provide those interactions.
3. Introduce a new profile only where the existing mappings are insufficient.
4. Associate the profile with the corresponding `ScreenKind`.
5. Keep application-specific interpretation out of `TouchManager`.

The test for whether a new profile is necessary is therefore:

> **Does this context require different physical-to-semantic translation?**

Not:

> **Does this screen behave differently?**

Different screen behaviour normally belongs in the screen's contextual interpretation, not in another touch profile.

---

## Architectural Boundary

Touch profiles are deliberately a **translation mechanism**, not a behaviour mechanism.

They must not contain knowledge such as:

```text
"tap here means change the clock"
"tap here means open Control Panel"
"tap here means reset the device"
```

Instead they produce semantic inputs such as:

```text
TAP
SELECT
SCROLL_UP
SCROLL_DOWN
BACK
NEXT_SCREEN
```

The receiving context then interprets those inputs:

```text
InputEvent
      │
      ▼
Active Screen
      │
      │ contextual interpretation
      ├───────────────┬────────────────┐
      ▼               ▼                ▼
Local behaviour   ScreenIntent     SystemIntent
      │               │                │
      ▼               ▼                ▼
Screen state     ScreenManager    System Context
```

This preserves the fundamental interaction architecture:

> **Input describes what happened. Context determines what it means.**