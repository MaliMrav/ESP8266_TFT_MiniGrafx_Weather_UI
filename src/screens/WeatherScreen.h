#pragma once

// WeatherScreen is the main dashboard screen.
//
// It displays the current time and date, WiFi signal strength, and a
// grid of sensor tiles sourced from SensorRepository. Tiles show the
// current value, daily min/max, and trend direction for each sensor.
//
// Input handling:
//   - TAP within the clock interaction area toggles 12/24-hour clock
//   - other events may be interpreted contextually by the screen
//
// Screen-level navigation is owned by ScreenManager.
//
// WeatherScreen reads from SensorRepository only. It has no knowledge
// of where the data came from (MQTT, HTTP, local sensor, etc.).

#include <Arduino.h>
#include "../models/SensorTile.h"
#include "../display/DisplayManager.h"
#include "../ui/Screen.h"
#include "../input/InputEvent.h"

class WeatherScreen : public Screen
{
public:
    explicit WeatherScreen(DisplayManager& display);

    void enter() override;
    void leave() override;
    void update() override;

    ScreenIntent onInput(const InputEvent& event) override;

    ScreenKind kind() const override
    {
        return ScreenKind::Weather;
    }

private:
    DisplayManager& display_;

    bool use12HourClock_ = false;

    bool isClockArea(
        const InputPosition& position) const;

    void drawHeader();
    void drawWifiQuality();
    void drawSensorGrid();
    void drawTrendArrow(int x, int y, TrendDirection t);

    int8_t getWifiQuality();
    String formatValue(SensorType type, float v);
    DisplayManager::Color getColor(SensorType t);
};