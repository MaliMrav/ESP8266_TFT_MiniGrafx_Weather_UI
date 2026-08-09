#include <Arduino.h>

#include "config/config.h"

#include "display/DisplayManager.h"

#include "touch/TouchController.h"
#include "touch/TouchManager.h"

#include "screens/BootScreen.h"
#include "screens/CalibrationScreen.h"
#include "screens/control/ControlPanelScreen.h"
#include "screens/WeatherScreen.h"

#include "ui/ScreenManager.h"

#include "system/SystemManager.h"

#include "ota/OtaManager.h"
#include "mqtt/MqttDataSource.h"

//=============================================================================
// Global objects
//=============================================================================

DisplayManager display;

TouchController touchController(
    TouchConfig::TOUCH_CS,
    TouchConfig::TOUCH_IRQ);

TouchManager touchManager(touchController);

ScreenManager screenManager;

BootScreen bootScreen(display);
WeatherScreen weatherScreen(display);
CalibrationScreen calibrationScreen(display,touchController);
ControlPanelScreen controlPanelScreen(display);

OtaManager ota;
MqttDataSource mqttData;

//=============================================================================
// Arduino setup
//=============================================================================

void setup()
{
    Serial.begin(115200);

    SystemManager::begin(
        display,
        touchController,
        touchManager,
        screenManager,
        bootScreen,
        weatherScreen,
        calibrationScreen,
        controlPanelScreen,
        ota,
        mqttData);
}

//=============================================================================
// Arduino loop
//=============================================================================

void loop()
{
    SystemManager::update();
}