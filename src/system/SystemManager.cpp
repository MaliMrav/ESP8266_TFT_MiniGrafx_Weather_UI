#include "SystemManager.h"

#include "../config/config.h"

#include <ESP8266WiFi.h>
#include <LittleFS.h>

#include "../display/DisplayManager.h"
#include "../input/InputAction.h"
#include "../input/InputEvent.h"
#include "../input/InputManager.h"

#include "../data/IDataSource.h"

#include "../data/ObservationRegistry.h"
#include "../data/ObservationHandle.h"

#include "../models/SensorRepository.h"
#include "../models/SensorTile.h"

#include "../models/WeatherObservationKeys.h"
#include "../models/SolarObservationKeys.h"

#include "../ota/OtaManager.h"

#include "../screens/BootScreen.h"
#include "../screens/CalibrationScreen.h"
#include "../screens/WeatherScreen.h"
#include "../screens/SolarScreen.h"
#include "../screens/control/ControlPanelScreen.h"

#include "../system/StatusCallback.h"

#include "../touch/TouchController.h"
#include "../touch/TouchManager.h"

#include "../ui/ScreenManager.h"

#include "../wifi/WifiSetup.h"


namespace
{
    DisplayManager* s_display = nullptr;
    TouchController* s_touchController = nullptr;
    TouchManager* s_touchManager = nullptr;
    ScreenManager* s_screenManager = nullptr;

    BootScreen* s_bootScreen = nullptr;
    WeatherScreen* s_weatherScreen = nullptr;
    SolarScreen* s_solarScreen = nullptr;
    CalibrationScreen* s_calibrationScreen = nullptr;
    ControlPanelScreen* s_controlPanelScreen = nullptr;

    OtaManager* s_ota = nullptr;
    IDataSource* s_dataSource = nullptr;

    bool s_calibrationMode = false;

    uint32_t s_calibrationCompleteSince = 0;
    uint32_t s_lastRedraw = 0;


    void bootStatus(
        const String& message,
        uint8_t progress)
    {
        Serial.printf(
            "[BOOT] %u%% %s\n",
            progress,
            message.c_str());

        if (s_bootScreen)
        {
            s_bootScreen->setStatus(
                message,
                progress);
        }

        if (s_screenManager)
        {
            s_screenManager->update();
        }
    }


    void initialiseFilesystem()
    {
        bootStatus(
            "Mounting filesystem",
            BootProgress::FS_MOUNT);

        if (!SystemManager::mountFileSystem())
        {
            bootStatus(
                "Filesystem failed",
                BootProgress::FS_MOUNT);

            delay(3000);
            ESP.restart();
            return;
        }
    }


    void initialiseNetwork(
        const String& hostname)
    {
        bootStatus(
            "Starting WiFi",
            BootProgress::WIFI_CONNECT);

        WifiSetup::begin(
            hostname.c_str(),
            bootStatus);

        bootStatus(
            "Synchronising time",
            BootProgress::TIME_SYNC);

        configTime(
            TimeConfig::TIMEZONE,
            TimeConfig::NTP_SERVER);
    }


    bool initialiseObservations()
{
    ObservationRegistry::initialise();
    SensorRepository::initialise();

    // -------------------------------------------------------------------------
    // Weather
    // -------------------------------------------------------------------------

    const ObservationHandle kitchenTemperature =
        ObservationRegistry::registerObservation(
            WeatherObservations::KITCHEN_TEMPERATURE);

    const ObservationHandle pergolaTemperature =
        ObservationRegistry::registerObservation(
            WeatherObservations::PERGOLA_TEMPERATURE);

    const ObservationHandle kitchenHumidity =
        ObservationRegistry::registerObservation(
            WeatherObservations::KITCHEN_HUMIDITY);

    const ObservationHandle pergolaHumidity =
        ObservationRegistry::registerObservation(
            WeatherObservations::PERGOLA_HUMIDITY);

    const ObservationHandle pressure =
        ObservationRegistry::registerObservation(
            WeatherObservations::PRESSURE);

    // -------------------------------------------------------------------------
    // Solar
    // -------------------------------------------------------------------------

    const ObservationHandle solarCurrentProduction =
        ObservationRegistry::registerObservation(
            SolarObservations::CURRENT_POWER_PRODUCTION);

    const ObservationHandle solarTodayProduction =
        ObservationRegistry::registerObservation(
            SolarObservations::ENERGY_PRODUCTION_TODAY);

    const ObservationHandle solarCurrentConsumption =
        ObservationRegistry::registerObservation(
            SolarObservations::CURRENT_POWER_CONSUMPTION);

    const ObservationHandle solarTodayConsumption =
        ObservationRegistry::registerObservation(
            SolarObservations::ENERGY_CONSUMPTION_TODAY);

    // -------------------------------------------------------------------------
    // Validate registry allocation
    // -------------------------------------------------------------------------

    if (!kitchenTemperature.isValid() ||
        !pergolaTemperature.isValid() ||
        !kitchenHumidity.isValid() ||
        !pergolaHumidity.isValid() ||
        !pressure.isValid() ||
        !solarCurrentProduction.isValid() ||
        !solarTodayProduction.isValid() ||
        !solarCurrentConsumption.isValid() ||
        !solarTodayConsumption.isValid())
    {
        Serial.println(
            "[OBSERVATION] Registration failed: registry capacity exceeded");

        return false;
    }

    // -------------------------------------------------------------------------
    // Bind handles to repository storage
    // -------------------------------------------------------------------------

    const bool kitchenTemperatureRegistered =
        SensorRepository::registerObservation(
            kitchenTemperature,
            SensorTile{
                "Kitchen Temp",
                "°C",
                TEMP
            });

    const bool pergolaTemperatureRegistered =
        SensorRepository::registerObservation(
            pergolaTemperature,
            SensorTile{
                "Pergola Temp",
                "°C",
                TEMP
            });

    const bool kitchenHumidityRegistered =
        SensorRepository::registerObservation(
            kitchenHumidity,
            SensorTile{
                "Kitchen Hum",
                "%",
                HUMIDITY
            });

    const bool pergolaHumidityRegistered =
        SensorRepository::registerObservation(
            pergolaHumidity,
            SensorTile{
                "Pergola Hum",
                "%",
                HUMIDITY
            });

    const bool pressureRegistered =
        SensorRepository::registerObservation(
            pressure,
            SensorTile{
                "Pressure",
                "hPa",
                PRESSURE
            });

    const bool productionRegistered =
        SensorRepository::registerObservation(
            solarCurrentProduction,
            SensorTile{
                "Production",
                "W",
                ENERGY_W
            });

    const bool productionTodayRegistered =
        SensorRepository::registerObservation(
            solarTodayProduction,
            SensorTile{
                "Prod Today",
                "Wh",
                ENERGY_WH
            });

    const bool consumptionRegistered =
        SensorRepository::registerObservation(
            solarCurrentConsumption,
            SensorTile{
                "Consumption",
                "W",
                ENERGY_W
            });

    const bool consumptionTodayRegistered =
        SensorRepository::registerObservation(
            solarTodayConsumption,
            SensorTile{
                "Cons Today",
                "Wh",
                ENERGY_WH
            });

    // -------------------------------------------------------------------------
    // Validate repository bindings
    // -------------------------------------------------------------------------

    if (!kitchenTemperatureRegistered ||
        !pergolaTemperatureRegistered ||
        !kitchenHumidityRegistered ||
        !pergolaHumidityRegistered ||
        !pressureRegistered ||
        !productionRegistered ||
        !productionTodayRegistered ||
        !consumptionRegistered ||
        !consumptionTodayRegistered)
    {
        Serial.println(
            "[OBSERVATION] Registration failed: repository capacity exceeded");

        return false;
    }

    return true;
}

#if defined(TELEMETRY_OBSERVATION_PIPELINE_TEST)

    void testObservationPipeline()
    {
        Serial.println(
            "[ZETA] Observation pipeline test");

        constexpr ObservationKey key{
            "sensor.bar_switch_panel_energy_power"
        };


        const ObservationHandle handle =
            ObservationRegistry::registerObservation(key);

        if (!handle.isValid())
        {
            Serial.println(
                "[ZETA] FAIL: ObservationHandle allocation");

            return;
        }


        const bool registered =
            SensorRepository::registerObservation(
                handle,
                SensorTile{
                    "Pipeline Test",
                    "W",
                    ENERGY_W
                });

        if (!registered)
        {
            Serial.println(
                "[ZETA] FAIL: SensorRepository registration");

            return;
        }


        constexpr float TEST_VALUE = 42.0f;

        if (!SensorRepository::setValue(
                handle,
                TEST_VALUE))
        {
            Serial.println(
                "[ZETA] FAIL: SensorRepository value update");

            return;
        }


        SensorTile* stored =
            SensorRepository::getTile(handle);

        if (!stored ||
            !stored->valid ||
            stored->value != TEST_VALUE)
        {
            Serial.println(
                "[ZETA] FAIL: SensorRepository readback");

            return;
        }


        const ObservationHandle resolved =
            ObservationRegistry::resolve(key);

        if (resolved != handle)
        {
            Serial.println(
                "[ZETA] FAIL: ObservationKey resolution");

            return;
        }


        Serial.println(
            "[ZETA] PASS");
    }

#endif


    void initialiseServices(
        const String& hostname)
    {
        bootStatus(
            "Loading sensors",
            BootProgress::SENSORS_INIT);

        if (!initialiseObservations())
        {
            Serial.println(
                "[OBSERVATION] Application composition failed");

            delay(3000);
            ESP.restart();
            return;
        }

#if defined(TELEMETRY_OBSERVATION_PIPELINE_TEST)
        testObservationPipeline();
#endif


        bootStatus(
            "Starting OTA",
            BootProgress::OTA_INIT);

        if (s_ota)
        {
            s_ota->begin(
                hostname.c_str());
        }


        bootStatus(
            "Starting data sources",
            BootProgress::MQTT_INIT);

        if (s_dataSource)
        {
            s_dataSource->begin();
        }
    }


    void initialiseUI()
    {
        // Only handle calibration if touch hardware exists.
        if (HardwareConfig::HAS_TOUCH &&
            HardwareConfig::HAS_RESISTIVE_TOUCH)
        {
            const bool calibrationLoaded =
                s_touchController &&
                s_touchController->loadCalibration();

            // Enter calibration mode when:
            // - forced via configuration, or
            // - no valid calibration data exists.
            s_calibrationMode =
                CalibrationConfig::FORCE_CALIBRATION ||
                !calibrationLoaded;


            if (s_touchManager)
            {
                s_touchManager->setProfile(
                    s_calibrationMode
                        ? TouchManager::Profile::Calibration
                        : TouchManager::Profile::Generic);
            }


            if (s_calibrationMode &&
                s_calibrationScreen &&
                s_screenManager)
            {
                Serial.println(
                    "[CALIBRATION] Entering calibration mode");

                s_screenManager->activate(
                    s_calibrationScreen);
            }
            else if (s_weatherScreen &&
                     s_screenManager)
            {
                s_screenManager->activate(
                    s_weatherScreen);
            }
        }
        else
        {
            s_calibrationMode = false;

            if (s_weatherScreen &&
                s_screenManager)
            {
                Serial.println(
                    "[SYSTEM] Touch disabled or not requiring calibration - showing WeatherScreen");

                s_screenManager->activate(
                    s_weatherScreen);
            }
        }


        if (s_screenManager)
        {
            s_screenManager->update();
        }
    }
}


namespace SystemManager
{
    String getHostname()
    {
        return String(System::HOSTNAME_PREFIX) +
               String(ESP.getChipId(), HEX);
    }


    bool mountFileSystem()
    {
        if (LittleFS.begin())
        {
            Serial.println(
                "LittleFS mounted");

            return true;
        }


        Serial.println(
            "LittleFS mount failed");

        Serial.println(
            "Formatting LittleFS...");


        if (!LittleFS.format())
        {
            Serial.println(
                "LittleFS format failed");

            return false;
        }


        return LittleFS.begin();
    }


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
        IDataSource&        dataSource)
    {
        s_display = &display;
        s_touchController = &touchController;
        s_touchManager = &touchManager;
        s_screenManager = &screenManager;

        s_bootScreen = &bootScreen;
        s_weatherScreen = &weatherScreen;
        s_solarScreen = &solarScreen;
        s_calibrationScreen = &calibrationScreen;
        s_controlPanelScreen = &controlPanelScreen;

        s_ota = &ota;
        s_dataSource = &dataSource;


        s_screenManager->bindTouchManager(
            s_touchManager);


        s_screenManager->registerScreen(
            s_weatherScreen);

        s_screenManager->registerScreen(
            s_solarScreen);

        s_screenManager->registerScreen(
            s_calibrationScreen);

        s_screenManager->registerScreen(
            s_controlPanelScreen);


        if (s_display)
        {
            s_display->begin();
        }


        if (s_screenManager &&
            s_bootScreen)
        {
            s_screenManager->activate(
                s_bootScreen);
        }


        bootStatus(
            "Starting display",
            BootProgress::DISPLAY_INIT);


        if (HardwareConfig::HAS_TOUCH &&
            s_touchController)
        {
            s_touchController->begin();

            bootStatus(
                "Starting touch",
                BootProgress::TOUCH_INIT);

            if (s_touchManager)
            {
                InputManager::registerSource(
                    s_touchManager);
            }
        }
        else
        {
            bootStatus(
                "Touch disabled",
                BootProgress::TOUCH_INIT);
        }


        initialiseFilesystem();


        const String hostname =
            getHostname();


        initialiseNetwork(
            hostname);

        initialiseServices(
            hostname);


        bootStatus(
            "Ready",
            BootProgress::READY);


        initialiseUI();


        s_lastRedraw = millis();
    }


    void update()
    {
        if (s_ota)
        {
            s_ota->loop();
        }


        if (s_dataSource)
        {
            s_dataSource->loop();
        }


        InputManager::update();


        const bool inputHandled =
            s_screenManager &&
            InputManager::dispatchTo(
                *s_screenManager);


        if (inputHandled &&
            s_screenManager)
        {
            s_screenManager->update();
            s_lastRedraw = millis();
        }


        // Handle calibration completion transition.
        if (HardwareConfig::HAS_TOUCH &&
            HardwareConfig::HAS_RESISTIVE_TOUCH &&
            s_calibrationMode &&
            s_calibrationScreen &&
            s_calibrationScreen->isFinished() &&
            s_calibrationScreen->calibrationSaved())
        {
            if (s_calibrationCompleteSince == 0)
            {
                s_calibrationCompleteSince =
                    millis();
            }
            else if (
                millis() -
                s_calibrationCompleteSince > 2000)
            {
                Serial.println(
                    "[CALIBRATION] Complete - transitioning to WeatherScreen");

                s_calibrationMode = false;


                if (s_touchManager)
                {
                    s_touchManager->setProfile(
                        TouchManager::Profile::Generic);
                }


                if (s_screenManager &&
                    s_weatherScreen)
                {
                    s_screenManager->activate(
                        s_weatherScreen);

                    s_screenManager->update();

                    s_lastRedraw =
                        millis();
                }
            }
        }
        else
        {
            s_calibrationCompleteSince = 0;
        }


        // Periodic screen refresh.
        if (s_screenManager &&
            millis() -
            s_lastRedraw >= 1000)
        {
            s_screenManager->update();
            s_lastRedraw = millis();
        }
    }
}