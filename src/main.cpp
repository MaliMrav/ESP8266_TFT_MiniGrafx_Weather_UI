#include <Arduino.h>

#include "config/config.h"

#include "display/DisplayManager.h"

#include "touch/TouchController.h"
#include "touch/TouchManager.h"

#include "screens/BootScreen.h"
#include "screens/CalibrationScreen.h"
#include "screens/control/ControlPanelScreen.h"
#include "screens/WeatherScreen.h"
#include "screens/EnergyStatusScreen.h"

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
EnergyStatusScreen EnergyStatusScreen(display);
CalibrationScreen calibrationScreen(
    display,
    touchController);
ControlPanelScreen controlPanelScreen(display);

OtaManager ota;
MqttDataSource mqttData;
DataSourceManager dataSources;


//=============================================================================
// Arduino setup
//=============================================================================

void setup()
{
    Serial.begin(115200);

#if defined(TELEMETRY_CAPABILITY_MQTT)
    dataSources.add(mqttData);
#endif

    SystemManager::begin(
        display,
        touchController,
        touchManager,
        screenManager,
        bootScreen,
        weatherScreen,
        EnergyStatusScreen,
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