# Touch Calibration Rules and Behavior

## Overview

The calibration system is **only active for resistive touchscreens**. It is completely bypassed for:
- Non-touch displays
- Capacitive touchscreens (they don't need calibration)
- Any other input method

---

## When Calibration Occurs

Calibration will ONLY run if **ALL** of these conditions are true:

1. `CFG_HAS_TOUCH` is `true`
2. `CFG_HAS_RESISTIVE_TOUCH` is `true`
3. Either:
   - `CFG_FORCE_CALIBRATION` is `true`, OR
   - No valid calibration file exists at `CFG_CALIBRATION_FILE`

---

## When Calibration is Skipped

Calibration is **completely bypassed** and the system goes straight to WeatherScreen if:

- `CFG_HAS_TOUCH` is `false` (no touch hardware)
- `CFG_HAS_RESISTIVE_TOUCH` is `false` (capacitive touch doesn't need calibration)
- Valid calibration data exists AND `CFG_FORCE_CALIBRATION` is `false`

---

## Configuration Examples

### Example 1: 2.4" ILI9341 TFT with XPT2046 Resistive Touch (Default)

```cpp
// config_override.h
#undef CFG_HAS_TOUCH
#define CFG_HAS_TOUCH true

#undef CFG_HAS_RESISTIVE_TOUCH
#define CFG_HAS_RESISTIVE_TOUCH true

#undef CFG_HAS_CAPACITIVE_TOUCH
#define CFG_HAS_CAPACITIVE_TOUCH false
```
```cpp
bool enabled = true;

if (enabled)
{
    Serial.println("Hello");
}
```

**Behavior:**
- First boot: Enters calibration mode (no saved calibration)
- Subsequent boots: Loads calibration, goes straight to WeatherScreen
- With `CFG_FORCE_CALIBRATION true`: Always calibrates on every boot

---

### Example 2: 2.2" ILI9341 TFT Without Touch

```cpp
// config_override.h
#undef CFG_HAS_TOUCH
#define CFG_HAS_TOUCH false
```

**Behavior:**
- Never enters calibration mode
- Goes straight to WeatherScreen on every boot
- Touch controller is not initialized
- Touch pins (CFG_TOUCH_CS, CFG_TOUCH_IRQ) are ignored
- CalibrationScreen is never shown

---

### Example 3: Display with Capacitive Touch (Future)

```cpp
// config_override.h
#undef CFG_HAS_TOUCH
#define CFG_HAS_TOUCH true

#undef CFG_HAS_RESISTIVE_TOUCH
#define CFG_HAS_RESISTIVE_TOUCH false

#undef CFG_HAS_CAPACITIVE_TOUCH
#define CFG_HAS_CAPACITIVE_TOUCH true
```

**Behavior:**
- Never enters calibration mode (capacitive doesn't need it)
- Touch input works immediately
- Goes straight to WeatherScreen on every boot

---

## Forcing Calibration

If you need to recalibrate (e.g., after replacing display or touch controller):

**Temporary Method:**
```cpp
// config_override.h
#undef CFG_FORCE_CALIBRATION
#define CFG_FORCE_CALIBRATION true
```

Flash firmware, perform calibration, then set back to `false` and reflash.

**Permanent Method:**
Delete the calibration file via serial:
```cpp
LittleFS.remove("/calibration.txt");
```

On next boot, calibration will run automatically.

---

## Boot Sequence Decision Tree

```
Boot Start
    │
    ├─ Display initialized
    ├─ Touch hardware checked
    │
    └─ Is CFG_HAS_TOUCH true?
        │
        ├─ NO  → Skip touch init
        │      → Skip calibration
        │      → Show WeatherScreen
        │      → Done
        │
        └─ YES → Initialize touch controller
               │
               └─ Is CFG_HAS_RESISTIVE_TOUCH true?
                   │
                   ├─ NO  → Skip calibration (capacitive)
                   │      → Show WeatherScreen
                   │      → Done
                   │
                   └─ YES → Is CFG_FORCE_CALIBRATION true?
                           │
                           ├─ YES → Show CalibrationScreen
                           │      → Collect 4 points
                           │      → Save calibration
                           │      → Transition to WeatherScreen
                           │      → Done
                           │
                           └─ NO  → Does calibration file exist?
                                   │
                                   ├─ YES → Load calibration
                                   │      → Show WeatherScreen
                                   │      → Done
                                   │
                                   └─ NO  → Show CalibrationScreen
                                          → Collect 4 points
                                          → Save calibration
                                          → Transition to WeatherScreen
                                          → Done
```

---

## Calibration File Format

Location: `/calibration.txt` on LittleFS

Format:
```
alphaX betaX deltaX
alphaY betaY deltaY
```

Example:
```
0.0612 -0.0003 -11.2450
-0.0001 0.0817 -15.8903
```

These coefficients transform raw touch coordinates (0-4095) to screen pixels (0-239, 0-319).

---

## Debug Logging

When troubleshooting, check serial output:

**Touch disabled:**
```
[SYSTEM] Touch disabled or not requiring calibration - showing WeatherScreen
```

**Touch enabled, entering calibration:**
```
[CALIBRATION] Entering calibration mode
```

**Calibration complete:**
```
[CALIBRATION] Complete - transitioning to WeatherScreen
```

---

## Common Issues and Solutions

### Issue: System stuck on crosshair (non-touch display)

**Cause:** `CFG_HAS_TOUCH` is `true` but no touch hardware is connected.

**Solution:**
```cpp
// config_override.h
#undef CFG_HAS_TOUCH
#define CFG_HAS_TOUCH false
```

Rebuild and flash.

---

### Issue: Calibration runs every boot

**Cause:** Calibration file not being saved or `CFG_FORCE_CALIBRATION` is `true`.

**Solution 1:** Check `CFG_FORCE_CALIBRATION`:
```cpp
#undef CFG_FORCE_CALIBRATION
#define CFG_FORCE_CALIBRATION false
```

**Solution 2:** Verify LittleFS is mounted:
Check serial output for `LittleFS mounted` message.

**Solution 3:** Check filesystem space:
```cpp
FSInfo fs_info;
LittleFS.info(fs_info);
Serial.printf("FS: %u / %u bytes\n", fs_info.usedBytes, fs_info.totalBytes);
```

---

### Issue: Touch works but is inaccurate

**Cause:** Calibration coefficients are incorrect.

**Solution:** Force recalibration:
```cpp
#undef CFG_FORCE_CALIBRATION
#define CFG_FORCE_CALIBRATION true
```

Reflash, calibrate carefully, then set back to `false`.

---

### Issue: Capacitive touch doesn't work

**Cause:** Capacitive touch support not yet implemented.

**Solution:** Capacitive touch controllers (FT6236, etc.) require different drivers. This is a future enhancement. For now, only resistive touch (XPT2046) is supported.

---

## Future Enhancements

### Calibration Timeout

Add a timeout to CalibrationScreen: if no touch input for 30 seconds, skip calibration and continue to WeatherScreen with default/uncalibrated touch.

### User-Triggered Calibration

Add a "Recalibrate Touch" option in SettingsScreen (future) so users can recalibrate without reflashing firmware.

### Calibration Quality Check

After collecting 4 points, validate that they form a reasonable rectangle. Reject and retry if points are too close together or in wrong order.

### Multi-Point Calibration

Collect more than 4 points (9-point or 12-point) for higher accuracy across the entire screen, especially on larger displays.

---

## Summary

**Key Rule:** Calibration only happens for resistive touchscreens when no valid calibration exists.

**For Non-Touch Displays:** Set `CFG_HAS_TOUCH = false` and calibration is completely bypassed.

**For Capacitive Touch:** Set `CFG_HAS_RESISTIVE_TOUCH = false` and calibration is skipped.

**For Resistive Touch:** Calibration runs once, saves to filesystem, then loads automatically on subsequent boots.
