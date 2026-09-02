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
#include "../data/ObservationHandle.h"

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

    ObservationHandle currentProductionHandle_;
    ObservationHandle currentConsumptionHandle_;
    ObservationHandle currentExportHandle_;
    ObservationHandle currentBatteryHandle_;

    ObservationHandle todayProductionHandle_;
    ObservationHandle todayConsumptionHandle_;
    ObservationHandle todayExportHandle_;
    ObservationHandle todayBatteryHandle_;

    void drawHeader();
    void drawGrid();

    void drawQuadrant(
        int x,
        int y,
        int w,
        int h,
        const char* label,
        ObservationHandle handle);

    String formatPower(float v) const;
};