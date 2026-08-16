#pragma once

// =============================================================================
// config_override.h — Your configuration
// =============================================================================
//
// This is the only file you need to edit to adapt this firmware to your
// hardware and environment.
//
// Uncomment and change any value below. Any value not defined here will
// fall back to the default in config_defaults.h.
//
// Credentials (WiFi, MQTT, OTA passwords) belong in secrets.h, not here.
// Copy secrets.example.h to secrets.h and fill in your values.
//
// =============================================================================


// -----------------------------------------------------------------------------
// System
// -----------------------------------------------------------------------------

// #define CFG_HOSTNAME_PREFIX "weather-display-"


// -----------------------------------------------------------------------------
// Connectivity — MQTT
// -----------------------------------------------------------------------------

// #define CFG_MQTT_PORT         1883
// #define CFG_MQTT_KEEPALIVE    30
// #define CFG_MQTT_RECONNECT_MS 5000


// -----------------------------------------------------------------------------
// Connectivity — API
// -----------------------------------------------------------------------------
// API host/credentials are defined in secrets.h.
// Override the poll interval here if needed.
//
// #undef  CFG_API_POLL_MS
// #define CFG_API_POLL_MS 10000


// -----------------------------------------------------------------------------
// Time and Locale
// -----------------------------------------------------------------------------
//
// Set your timezone. Full list of valid POSIX strings:
//   https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

#undef CFG_TIMEZONE
#define CFG_TIMEZONE "AEST-10AEDT,M10.1.0,M4.1.0/3"  // Australia/Sydney

// #undef CFG_NTP_SERVER
// #define CFG_NTP_SERVER "pool.ntp.org"

// #define CFG_USE_12HOUR_CLOCK false


// =============================================================================
// HARDWARE CONFIGURATION
// =============================================================================
//
// Configure your hardware in hierarchical order:
//   1. Choose display type
//   2. Set display pins
//   3. Enable input devices (touch/buttons/encoder)
//   4. Configure enabled devices
//
// =============================================================================


// -----------------------------------------------------------------------------
// Display — Type and Driver
// -----------------------------------------------------------------------------
//
// This firmware currently uses a 2.4" ILI9341 TFT display (240x320, SPI).
// If you're using the same hardware, you likely don't need to change anything.
//
// If you're using different hardware:
//   1. Uncomment and set CFG_DISPLAY_DRIVER to your chip (ILI9341, SSD1306, etc.)
//   2. Adjust CFG_SCREEN_WIDTH and CFG_SCREEN_HEIGHT to match your display
//   3. Verify pin assignments below
//   4. Check config/display/YourDriver_config.h for driver-specific settings
//
// Available drivers:
//   ILI9341 — 2.4" 240x320 TFT (default, fully configured)
//   SSD1306 — 128x64 OLED (future, not yet implemented)
//   ST7735  — 1.8" 128x160 TFT (future, not yet implemented)

// #undef CFG_DISPLAY_DRIVER
// #define CFG_DISPLAY_DRIVER ILI9341

// #undef CFG_HAS_DISPLAY
// #define CFG_HAS_DISPLAY true


// -----------------------------------------------------------------------------
// Display — Resolution
// -----------------------------------------------------------------------------

// #undef CFG_SCREEN_WIDTH
// #define CFG_SCREEN_WIDTH 240

// #undef CFG_SCREEN_HEIGHT
// #define CFG_SCREEN_HEIGHT 320


// -----------------------------------------------------------------------------
// Display — Pin Assignments (SPI)
// -----------------------------------------------------------------------------

// #undef CFG_TFT_DC
// #define CFG_TFT_DC D4

// #undef CFG_TFT_CS
// #define CFG_TFT_CS D2

// #undef CFG_TFT_LED
// #define CFG_TFT_LED D3


// =============================================================================
// INPUT DEVICES
// =============================================================================
//
// Enable input devices your hardware has, then configure their settings below.
//
// =============================================================================


// -----------------------------------------------------------------------------
// Touch Input — Enable/Disable
// -----------------------------------------------------------------------------
//
// Set to false if your display does NOT have touch capability.
// Example: 2.2" ILI9341 TFT without touch controller
//
// When false, all touch-related settings below are ignored and the system
// will skip touch initialization and calibration entirely.

// #undef CFG_HAS_TOUCH
// #define CFG_HAS_TOUCH false    // <-- Set to false for non-touch displays


// -----------------------------------------------------------------------------
// Touch Input — Type (only relevant if CFG_HAS_TOUCH = true)
// -----------------------------------------------------------------------------
// These settings are IGNORED if CFG_HAS_TOUCH is false.

// #undef CFG_HAS_RESISTIVE_TOUCH
// #define CFG_HAS_RESISTIVE_TOUCH true

// #undef CFG_HAS_CAPACITIVE_TOUCH
// #define CFG_HAS_CAPACITIVE_TOUCH false


// -----------------------------------------------------------------------------
// Touch Input — Pin Assignments (only relevant if CFG_HAS_TOUCH = true)
// -----------------------------------------------------------------------------
// These pins are IGNORED if CFG_HAS_TOUCH is false.

// #undef CFG_TOUCH_CS
// #define CFG_TOUCH_CS D0

// #undef CFG_TOUCH_IRQ
// #define CFG_TOUCH_IRQ D1


// -----------------------------------------------------------------------------
// Touch Input — Behavior (only relevant if CFG_HAS_TOUCH = true)
// -----------------------------------------------------------------------------

// #undef CFG_TOUCH_DEBOUNCE_MS
// #define CFG_TOUCH_DEBOUNCE_MS 50


// -----------------------------------------------------------------------------
// Touch Calibration (only relevant if CFG_HAS_TOUCH = true)
// -----------------------------------------------------------------------------
// Calibration is IGNORED if CFG_HAS_TOUCH is false.
// Calibration is typically only needed for resistive touchscreens.

// #undef CFG_FORCE_CALIBRATION
// #define CFG_FORCE_CALIBRATION false

// #undef CFG_CALIBRATION_FILE
// #define CFG_CALIBRATION_FILE "/calibration.txt"


// -----------------------------------------------------------------------------
// Other Input Devices (future)
// -----------------------------------------------------------------------------

// #undef CFG_HAS_BUTTONS
// #define CFG_HAS_BUTTONS false

// #undef CFG_HAS_ENCODER
// #define CFG_HAS_ENCODER false


// =============================================================================
// DEVELOPER TOOLS
// =============================================================================


// -----------------------------------------------------------------------------
// Debug Overlay
// -----------------------------------------------------------------------------
//
// Renders a two-line diagnostic bar at the bottom of the display on every
// touch event:
//   Line 1 (yellow) — InputAction and calibrated screen coordinates
//   Line 2 (blue)   — raw ADC values from the XPT2046
//
// Useful for diagnosing touch calibration and input pipeline issues without
// needing a serial connection. Compiles away completely when not defined.
//
// Uncomment to enable:
// #define DEBUG_OVERLAY

// Uncomment to enable Envoy HTTP diagnostic overlay on SolarScreen:
#define ENVOY_DEBUG
