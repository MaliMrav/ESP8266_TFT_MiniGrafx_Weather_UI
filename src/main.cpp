#include <Arduino.h>

#include "config/config.h"

#include "display/DisplayManager.h"

#include "touch/TouchController.h"
#include "touch/TouchManager.h"

#include "screens/BootScreen.h"
#include "screens/CalibrationScreen.h"
#include "screens/control/ControlPanelScreen.h"
#include "screens/WeatherScreen.h"
#include "screens/SolarScreen.h"

#include "ui/ScreenManager.h"

#include "system/SystemManager.h"

#include "ota/OtaManager.h"
#include "data/sources/mqtt/MqttDataSource.h"
#include "data/DataSourceManager.h"

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
SolarScreen solarScreen(display);
CalibrationScreen calibrationScreen(display,touchController);
ControlPanelScreen controlPanelScreen(display);

OtaManager ota;
MqttDataSource  mqttData;
DataSourceManager dataSources;

//=============================================================================
// Arduino setup
//=============================================================================

void setup()
{
    Serial.begin(115200);

    dataSources.add(mqttData);

    SystemManager::begin(
        display,
        touchController,
        touchManager,
        screenManager,
        bootScreen,
        weatherScreen,
        solarScreen,
        calibrationScreen,
        controlPanelScreen,
        ota,
        dataSources);
}

//=============================================================================
// Arduino loop
//=============================================================================

void loop()
{
    SystemManager::update();
}