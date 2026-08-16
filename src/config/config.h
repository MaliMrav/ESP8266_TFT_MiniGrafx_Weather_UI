#pragma once

// =============================================================================
// config.h — Single configuration include
// =============================================================================
//
// This is the only config header that framework code should include.
//
// Include order:
//   1. secrets.h       — credentials, excluded from source control
//   2. config_defaults.h — all CFG_ defaults (do not edit)
//   3. config_override.h — your overrides (edit this one)
//
// After the includes, CFG_ defines are bridged into constexpr namespaces
// so framework code uses typed, scoped identifiers rather than bare macros.
//
// =============================================================================

#include <Arduino.h>

#include "secrets.h"
#include "config_defaults.h"
#include "config_override.h"

// Include the appropriate display driver configuration
#define ILI9341 1
#define SSD1306 2
#define ST7735  3

#if CFG_DISPLAY_DRIVER == ILI9341
  #include "display/ILI9341_config.h"
#elif CFG_DISPLAY_DRIVER == SSD1306
  #error "SSD1306 driver configuration not yet implemented. See config/display/ILI9341_config.h as a template."
#elif CFG_DISPLAY_DRIVER == ST7735
  #error "ST7735 driver configuration not yet implemented. See config/display/ILI9341_config.h as a template."
#else
  #error "CFG_DISPLAY_DRIVER not set or invalid. Set it in config_override.h to ILI9341, SSD1306, or ST7735."
#endif


// -----------------------------------------------------------------------------
// System
// -----------------------------------------------------------------------------
namespace System
{
    constexpr const char* HOSTNAME_PREFIX = CFG_HOSTNAME_PREFIX;
}


// -----------------------------------------------------------------------------
// Connectivity — WiFi
// -----------------------------------------------------------------------------
namespace WiFiConfig
{
    constexpr const char* SSID     = WiFiSecrets::SSID;
    constexpr const char* PASSWORD = WiFiSecrets::PASSWORD;
}


// -----------------------------------------------------------------------------
// Connectivity — MQTT
// -----------------------------------------------------------------------------
namespace MQTT
{
    constexpr const char* SERVER   = MqttSecrets::SERVER;
    constexpr uint16_t    PORT     = CFG_MQTT_PORT;
    constexpr const char* CLIENT   = MqttSecrets::CLIENT;
    constexpr const char* USERNAME = MqttSecrets::USERNAME;
    constexpr const char* PASSWORD = MqttSecrets::PASSWORD;
    constexpr uint16_t    KEEPALIVE     = CFG_MQTT_KEEPALIVE;
    constexpr uint32_t    RECONNECT_MS  = CFG_MQTT_RECONNECT_MS;
}


// -----------------------------------------------------------------------------
// Connectivity — API
// -----------------------------------------------------------------------------
namespace ApiConfig
{
    // The currently configured API credentials are stored in secrets.h.
    constexpr const char* HOST    = ApiSecrets::HOST;
    constexpr const char* TOKEN   = ApiSecrets::TOKEN;
    constexpr uint32_t    POLL_MS = CFG_API_POLL_MS;
}


// -----------------------------------------------------------------------------
// Connectivity — OTA
// -----------------------------------------------------------------------------
namespace OtaConfig
{
    constexpr const char* PASSWORD      = OtaSecrets::PASSWORD;
    constexpr const char* PASSWORD_HASH = OtaSecrets::PASSWORD_HASH;
}


// -----------------------------------------------------------------------------
// Time and Locale
// -----------------------------------------------------------------------------
namespace TimeConfig
{
    constexpr const char* TIMEZONE        = CFG_TIMEZONE;
    constexpr const char* NTP_SERVER      = CFG_NTP_SERVER;
    constexpr bool        USE_12HOUR_CLOCK = CFG_USE_12HOUR_CLOCK;

    extern const String WDAY_NAMES[7];
    extern const String MONTH_NAMES[12];
}


// -----------------------------------------------------------------------------
// Hardware — Display
// -----------------------------------------------------------------------------
namespace HardwareConfig
{
    constexpr bool HAS_DISPLAY = CFG_HAS_DISPLAY;

    constexpr bool HAS_TOUCH            = CFG_HAS_TOUCH;
    constexpr bool HAS_RESISTIVE_TOUCH  = CFG_HAS_RESISTIVE_TOUCH;
    constexpr bool HAS_CAPACITIVE_TOUCH = CFG_HAS_CAPACITIVE_TOUCH;
    constexpr bool HAS_BUTTONS          = CFG_HAS_BUTTONS;
    constexpr bool HAS_ENCODER          = CFG_HAS_ENCODER;
}

namespace Pins
{
    constexpr int TFT_DC  = CFG_TFT_DC;
    constexpr int TFT_CS  = CFG_TFT_CS;
    constexpr int TFT_LED = CFG_TFT_LED;
}


// -----------------------------------------------------------------------------
// Hardware — Touch
// -----------------------------------------------------------------------------
namespace TouchConfig
{
    constexpr int      TOUCH_CS  = CFG_TOUCH_CS;
    constexpr int      TOUCH_IRQ = CFG_TOUCH_IRQ;

    constexpr int      SCREEN_WIDTH  = CFG_SCREEN_WIDTH;
    constexpr int      SCREEN_HEIGHT = CFG_SCREEN_HEIGHT;

    constexpr uint16_t DEBOUNCE_MS          = CFG_TOUCH_DEBOUNCE_MS;
    constexpr uint32_t CALIBRATION_PROMPT_MS = 3000;
    constexpr uint16_t MAX_TOUCH_TIME_MS     = 2000;
    constexpr uint16_t CALIBRATION_RETRY_MS  = 500;

    constexpr const char* CALIBRATION_FILE = CFG_CALIBRATION_FILE;
    constexpr const char* UI_STATE_FILE    = "/ui_state.txt";
}


// -----------------------------------------------------------------------------
// Touch Calibration
// -----------------------------------------------------------------------------
namespace CalibrationConfig
{
    constexpr bool FORCE_CALIBRATION = CFG_FORCE_CALIBRATION;
}


// -----------------------------------------------------------------------------
// Boot progress milestones (internal — not user configurable)
// -----------------------------------------------------------------------------
namespace BootProgress
{
    constexpr uint8_t DISPLAY_INIT = 5;
    constexpr uint8_t TOUCH_INIT   = 15;
    constexpr uint8_t FS_MOUNT     = 30;
    constexpr uint8_t WIFI_CONNECT = 45;
    constexpr uint8_t TIME_SYNC    = 60;
    constexpr uint8_t SENSORS_INIT = 75;
    constexpr uint8_t OTA_INIT     = 85;
    constexpr uint8_t MQTT_INIT    = 95;
    constexpr uint8_t READY        = 100;
}
