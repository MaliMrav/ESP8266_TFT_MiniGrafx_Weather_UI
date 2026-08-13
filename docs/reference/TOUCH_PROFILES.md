# Touch Profiles

## Overview

`TouchManager` translates raw touch coordinates into semantic `InputEvent` objects.

The translation is not fixed. It depends on context.

The same physical touch in the same screen location can mean different things depending on which screen is active.

That context is captured in a **touch profile**.

---

## Why Profiles Exist

Consider a touch at the left edge of the screen.

On the Weather screen, that means:

```
PREVIOUS_SCREEN
```

On the Control Panel, that means:

```
BACK
```

The physical interaction is identical.

The semantic meaning is different.

A single fixed zone-to-action mapping cannot express this.

Profiles solve this by allowing `TouchManager` to apply a different translation table depending on the active screen context.

This preserves the semantic input boundary:

```text
Physical touch
      │
      ▼
TouchManager (applies active profile)
      │
      ▼
Semantic InputEvent
      │
      ▼
Active Screen
```

The screen never sees coordinates. It sees meaning.

---

## Who Sets the Profile

`ScreenManager` sets the profile when it activates a screen:

```text
ScreenManager::activate(screen)
      │
      ├── calls screen->enter()
      │
      └── calls touchManager->setProfile(...)
```

The profile therefore always matches the active screen.

No screen needs to manage its own touch profile.

---

## Zone Layout

All profiles share the same underlying zone definitions from `ScreenZones`.

```text
┌─────────────────────────────────┐
│           HEADER                │  → varies by profile
├────┬───────────────────────┬────┤
│    │      TOP STRIP        │    │
│    ├───────────────────────┤    │
│ L  │                       │ R  │
│ E  │    CONTENT AREA       │ I  │
│ F  │                       │ G  │
│ T  │                       │ H  │
│    ├───────────────────────┤ T  │
│    │     BOTTOM STRIP      │    │
└────┴───────────────────────┴────┘
```

Zone dimensions (defined in `ScreenZones.h`):

| Zone         | Boundary                          |
|--------------|-----------------------------------|
| Header       | y < 58                            |
| Left edge    | x < 40                            |
| Right edge   | x >= 200                          |
| Top strip    | y 58–118, inside edges            |
| Bottom strip | y 260–320, inside edges           |
| Content area | remainder                         |

---

## Profiles

### Weather

Used by `WeatherScreen`.

| Zone         | Action            |
|--------------|-------------------|
| Header       | `TAP + position`  |
| Left edge    | `PREVIOUS_SCREEN` |
| Right edge   | `NEXT_SCREEN`     |
| Content area | `SELECT`          |

The header tap carries position so `WeatherScreen` can hit-test the clock area locally.

### ControlPanel

Used by `ControlPanelScreen` and all pages within it.

| Zone         | Action          |
|--------------|-----------------|
| Left edge    | `BACK`          |
| Right edge   | `SELECT`        |
| Top strip    | `SCROLL_UP`     |
| Bottom strip | `SCROLL_DOWN`   |
| Content area | `TAP + position`|

The content area tap carries position so individual pages can perform direct item selection by hit-testing locally.

This means a rotary encoder producing `SCROLL_UP`, `SCROLL_DOWN`, and `SELECT` works identically to touch — the pages never see anything hardware-specific.

### Status

Used by status and information screens.

| Zone         | Action            |
|--------------|-------------------|
| Header       | `BACK`            |
| Left edge    | `PREVIOUS_SCREEN` |
| Right edge   | `NEXT_SCREEN`     |
| Upper half   | `SCROLL_UP`       |
| Lower half   | `SCROLL_DOWN`     |
| Content area | `SELECT`          |

### Calibration

Used by `CalibrationScreen`.

All touches produce `TAP + raw position`.

Raw coordinates are used here because calibration is the process that produces the correction coefficients — applying those coefficients during calibration would be circular.

### Generic

Fallback profile. All touches produce `TAP + position`.

---

## Profile Switching and Debounce

When `ScreenManager` activates a new screen, it calls `setProfile()` on `TouchManager`.

`setProfile()` changes the active profile but deliberately does **not** reset the `wasTouched_` flag.

This is intentional.

If `wasTouched_` were reset on a profile switch, the finger that triggered navigation would still be in contact when the new profile became active. The next `update()` call would see the finger lift, clear `wasTouched_`, and immediately fire a second event on the new screen — producing a ghost interaction.

The correct reset point is when the finger actually lifts, which `update()` handles naturally via the `isTouched() == false` branch.

---

## Adding a New Profile

To add a profile for a new screen:

1. Add the value to the `Profile` enum in `TouchManager.h`
2. Add a `case` in `TouchManager::update()` mapping zones to actions
3. Add a `case` in `ScreenManager::activate()` mapping the `ScreenKind` to the new profile

The screen itself requires no changes.
