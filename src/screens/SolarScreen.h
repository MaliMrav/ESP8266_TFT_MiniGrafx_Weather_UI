#pragma once

// SolarScreen displays current and today's solar-energy measurements.
//
// Left column  — instantaneous power (W):
//   Production, Consumption, Export, Battery
//
// Right column — today's accumulated energy (Wh):
//   Production, Consumption, Export, Battery
//
// Data is sourced from SensorRepository only. SolarScreen has no knowledge
// of MQTT, Home Assistant, Envoy, or any other data transport.
//
// Input handling:
//   - NEXT_SCREEN     → ControlPanelScreen
//   - PREVIOUS_SCREEN → WeatherScreen

#include <Arduino.h>
#include "../display/DisplayManager.h"
#include "../ui/Screen.h"
#include "../input/InputEvent.h"
#include "../models/SolarObservationKeys.h"

class SolarScreen : public Screen
{
public:
    explicit SolarScreen(DisplayManager& display);

    void enter()  override;
    void leave()  override;
    void update() override;

    ScreenIntent onInput(const InputEvent& event) override;

    ScreenKind kind() const override
    {
        return ScreenKind::Solar;
    }

private:
    DisplayManager& display_;

    void drawHeader();
    void drawGrid();
    void drawQuadrant(int x, int y, int w, int h,
                      const char* label, uint8_t id);

    String formatPower(float v) const;
};
