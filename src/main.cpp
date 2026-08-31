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

#include "data/ObservationKey.h"
#include "data/ObservationHandle.h"
#include "data/ObservationRegistry.h"
#include "models/SensorRepository.h"
#include "models/SolarObservationKeys.h"

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

#if defined(TELEMETRY_OBSERVATION_PIPELINE_TEST)

namespace
{
    constexpr ObservationKey BAR_SWITCH_PANEL_POWER{
        "sensor.bar_switch_panel_energy_power"
    };

    void testObservationPipeline()
    {
        Serial.println("[ZETA] Observation pipeline test");

        // 1. Semantic identity
        const ObservationKey key = BAR_SWITCH_PANEL_POWER;

        // 2. Runtime identity
        const ObservationHandle handle =
            ObservationRegistry::registerObservation(key);

        if (!handle.isValid())
        {
            Serial.println("[ZETA] FAIL: ObservationHandle allocation");
            return;
        }

        // 3. Repository registration
        const SensorTile tile{
            "Bar Switch Panel Power",
            "W",
            ENERGY_W
        };

        if (!SensorRepository::registerObservation(handle, tile))
        {
            Serial.println("[ZETA] FAIL: SensorRepository registration");
            return;
        }

        // 4. Runtime value update
        if (!SensorRepository::setValue(handle, 42.0f))
        {
            Serial.println("[ZETA] FAIL: SensorRepository value update");
            return;
        }

        // 5. Read back through the handle
        SensorTile* stored =
            SensorRepository::getTile(handle);

        if (!stored || !stored->valid || stored->value != 42.0f)
        {
            Serial.println("[ZETA] FAIL: SensorRepository readback");
            return;
        }

        // 6. Resolution must return the same runtime handle
        const ObservationHandle resolved =
            ObservationRegistry::resolve(key);

        if (resolved != handle)
        {
            Serial.println("[ZETA] FAIL: ObservationKey resolution");
            return;
        }

        Serial.println("[ZETA] PASS");
    }
}

#endif

//=============================================================================
// Arduino setup
//=============================================================================

void setup()
{
    Serial.begin(115200);

    #if defined(TELEMETRY_OBSERVATION_PIPELINE_TEST)
        ObservationRegistry::initialise();
        SensorRepository::initialise();
        ObservationHandle solarCurrentProductionHandle;
        ObservationHandle solarTodayProductionHandle;
        ObservationHandle solarCurrentConsumptionHandle;
        ObservationHandle solarTodayConsumptionHandle;

        solarCurrentProductionHandle =
            ObservationRegistry::registerObservation(
                SolarObservations::CURRENT_POWER_PRODUCTION);

        solarTodayProductionHandle =
            ObservationRegistry::registerObservation(
                SolarObservations::ENERGY_PRODUCTION_TODAY);

        solarCurrentConsumptionHandle =
            ObservationRegistry::registerObservation(
                SolarObservations::CURRENT_POWER_CONSUMPTION);

        solarTodayConsumptionHandle =
            ObservationRegistry::registerObservation(
                SolarObservations::ENERGY_CONSUMPTION_TODAY);

        testObservationPipeline();
    #endif

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