#pragma once

// SystemManager orchestrates the full application lifecycle.
//
// Responsibilities:
//   - receive all top-level objects from main.cpp (no globals of its own)
//   - run the boot sequence in the correct order
//   - delegate update() calls to OTA, data source, input, and screen each loop
//   - manage the calibration workflow transition to normal operation
//
// SystemManager does not own any objects — it holds references to objects
// created in main.cpp. This keeps construction order explicit and testable.
// It depends on IDataSource, not any concrete data source implementation.

#include <Arduino.h>

class DisplayManager;
class TouchController;
class TouchManager;
class ScreenManager;
class BootScreen;
class WeatherScreen;
class SolarScreen;
class CalibrationScreen;
class ControlPanelScreen;
class OtaManager;
class IDataSource;

namespace SystemManager
{
    bool   mountFileSystem();
    String getHostname();

    void begin(
        DisplayManager&     display,
        TouchController&    touchController,
        TouchManager&       touchManager,
        ScreenManager&      screenManager,
        BootScreen&         bootScreen,
        WeatherScreen&      weatherScreen,
        SolarScreen&        solarScreen,
        CalibrationScreen&  calibrationScreen,
        ControlPanelScreen& controlPanelScreen,
        OtaManager&         ota,
        IDataSource&        dataSource);

    void update();
}