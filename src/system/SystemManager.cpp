#include "SystemManager.h"

#include "../config/config.h"

#include <ESP8266WiFi.h>
#include <LittleFS.h>

#include "../display/DisplayManager.h"
#include "../input/InputAction.h"
#include "../input/InputEvent.h"
#include "../input/InputManager.h"
#include "../data/IDataSource.h"
#include "../models/SensorRepository.h"
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
            s_bootScreen->setStatus(message, progress);
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

    void initialiseServices(
        const String& hostname)
    {
        bootStatus(
            "Loading sensors",
            BootProgress::SENSORS_INIT);

        SensorRepository::initialise();

        bootStatus(
            "Starting OTA",
            BootProgress::OTA_INIT);

        if (s_ota)
        {
            s_ota->begin(hostname.c_str());
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
        // Only handle calibration if touch hardware exists
        if (HardwareConfig::HAS_TOUCH && HardwareConfig::HAS_RESISTIVE_TOUCH)
        {
            // Check if calibration data exists
            const bool calibrationLoaded =
                s_touchController &&
                s_touchController->loadCalibration();

            // Enter calibration mode if:
            // - Forced via config, OR
            // - No valid calibration exists
            s_calibrationMode =
                CalibrationConfig::FORCE_CALIBRATION ||
                !calibrationLoaded;

            if (s_touchManager)
            {
                s_touchManager->setProfile(
                    s_calibrationMode ?
                        TouchManager::Profile::Calibration :
                        TouchManager::Profile::Generic);
            }

            if (s_calibrationMode && s_calibrationScreen && s_screenManager) // Enter calibration mode if no calibration exists or forced via config or if the user has requested it via the ControlPanelScreen
            {
                Serial.println("[CALIBRATION] Entering calibration mode");
                s_screenManager->activate(s_calibrationScreen); // CalibrationScreen will handle the transition to WeatherScreen when calibration is complete
            }
            else if (s_weatherScreen && s_screenManager) // If calibration is not required, go straight to the WeatherScreen
            {
                s_screenManager->activate(s_weatherScreen);
            }
        }
        else
        {
            // No touch hardware or capacitive touch (no calibration needed)
            s_calibrationMode = false;

            if (s_weatherScreen && s_screenManager)
            {
                Serial.println("[SYSTEM] Touch disabled or not requiring calibration - showing WeatherScreen");
                s_screenManager->activate(s_weatherScreen);
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
            Serial.println("LittleFS mounted");
            return true;
        }

        Serial.println("LittleFS mount failed");
        Serial.println("Formatting LittleFS...");

        if (!LittleFS.format())
        {
            Serial.println("LittleFS format failed");
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
        s_ota        = &ota;
        s_dataSource = &dataSource;

        s_screenManager->bindTouchManager(s_touchManager);

        s_screenManager->registerScreen(s_weatherScreen);
        s_screenManager->registerScreen(s_solarScreen);
        s_screenManager->registerScreen(s_calibrationScreen);
        s_screenManager->registerScreen(s_controlPanelScreen);

        if (s_display)
        {
            s_display->begin();
        }

        if (s_screenManager && s_bootScreen)
        {
            s_screenManager->activate(s_bootScreen);
        }

        bootStatus(
            "Starting display",
            BootProgress::DISPLAY_INIT);

        if (HardwareConfig::HAS_TOUCH && s_touchController)
        {
            s_touchController->begin();

            bootStatus(
                "Starting touch",
                BootProgress::TOUCH_INIT);

            if (s_touchManager)
            {
                InputManager::registerSource(s_touchManager);
            }
        }
        else
        {
            bootStatus(
                "Touch disabled",
                BootProgress::TOUCH_INIT);
        }

        initialiseFilesystem();

        const String hostname = getHostname();

        initialiseNetwork(hostname);
        initialiseServices(hostname);

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
            InputManager::dispatchTo(*s_screenManager);

        if (inputHandled && s_screenManager)
        {
            s_screenManager->update();
            s_lastRedraw = millis();
        }

        // Handle calibration completion transition
        // Only relevant if we're in calibration mode (resistive touch only)
        if (HardwareConfig::HAS_TOUCH &&
            HardwareConfig::HAS_RESISTIVE_TOUCH &&
            s_calibrationMode &&
            s_calibrationScreen &&
            s_calibrationScreen->isFinished() &&
            s_calibrationScreen->calibrationSaved())
        {
            if (s_calibrationCompleteSince == 0)
            {
                s_calibrationCompleteSince = millis();
            }
            else if (millis() - s_calibrationCompleteSince > 2000)
            {
                Serial.println("[CALIBRATION] Complete - transitioning to WeatherScreen");
                s_calibrationMode = false;

                if (s_touchManager)
                {
                    s_touchManager->setProfile(TouchManager::Profile::Generic);
                }

                if (s_screenManager && s_weatherScreen)
                {
                    s_screenManager->activate(s_weatherScreen);
                    s_screenManager->update();
                    s_lastRedraw = millis();
                }
            }
        }
        else
        {
            s_calibrationCompleteSince = 0;
        }

        // Periodic screen refresh
        if (s_screenManager && millis() - s_lastRedraw >= 1000)
        {
            s_screenManager->update();
            s_lastRedraw = millis();
        }
    }
}