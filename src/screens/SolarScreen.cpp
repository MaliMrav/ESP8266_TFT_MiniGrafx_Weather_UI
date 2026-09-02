#include "SolarScreen.h"

#include "../display/DisplayManager.h"
#include "../data/ObservationRegistry.h"
#include "../models/SensorRepository.h"
#include "../input/InputAction.h"
#include "../ui/ScreenIntent.h"
#include "../ui/ScreenKind.h"

#include <ArialRounded.h>

namespace
{
    constexpr int HEADER_H = 20;
    constexpr int MARGIN   = 4;
    constexpr int GAP      = 4;
    constexpr int COLS     = 2;
    constexpr int ROWS     = 4;
}

SolarScreen::SolarScreen(DisplayManager& display)
    : display_(display) {}

void SolarScreen::enter()
{
    currentProductionHandle_ =
        ObservationRegistry::resolve(
            SolarObservations::CURRENT_POWER_PRODUCTION);

    currentConsumptionHandle_ =
        ObservationRegistry::resolve(
            SolarObservations::CURRENT_POWER_CONSUMPTION);

    todayProductionHandle_ =
        ObservationRegistry::resolve(
            SolarObservations::ENERGY_PRODUCTION_TODAY);

    todayConsumptionHandle_ =
        ObservationRegistry::resolve(
            SolarObservations::ENERGY_CONSUMPTION_TODAY);

    // No semantic observations exist for these yet.
    currentExportHandle_ =
        ObservationHandle{};

    currentBatteryHandle_ =
        ObservationHandle{};

    todayExportHandle_ =
        ObservationHandle{};

    todayBatteryHandle_ =
        ObservationHandle{};
}

void SolarScreen::leave() {}

void SolarScreen::update()
{
    display_.clear(DisplayManager::BLACK);
    drawHeader();
    drawGrid();
    display_.commit();
}

ScreenIntent SolarScreen::onInput(const InputEvent& event)
{
    switch (event.action)
    {
        case InputAction::NEXT_SCREEN:
            return ScreenIntent::navigateTo(ScreenKind::ControlPanel);

        case InputAction::PREVIOUS_SCREEN:
            return ScreenIntent::navigateTo(ScreenKind::Weather);

        default:
            break;
    }

    return ScreenIntent();
}

void SolarScreen::drawHeader()
{
    display_.setFont(ArialRoundedMTBold_14);
    display_.setTextAlignment(DisplayManager::CENTER);
    display_.setColor(DisplayManager::WHITE);
    display_.drawString(display_.getWidth() / 2, 3, "Solar Energy");

    const int colW =
        (display_.getWidth() - (2 * MARGIN) - GAP) / COLS;

    display_.setFont(ArialMT_Plain_10);
    display_.setColor(DisplayManager::YELLOW);
    display_.drawString(
        MARGIN + colW / 2,
        HEADER_H - 2,
        "Now (W)");

    display_.drawString(
        MARGIN + colW + GAP + colW / 2,
        HEADER_H - 2,
        "Today (Wh)");
}

void SolarScreen::drawGrid()
{
    const int gridTop = HEADER_H + GAP;
    const int colW =
        (display_.getWidth() - (2 * MARGIN) - GAP) / COLS;
    const int rowH =
        (display_.getHeight() - gridTop - ((ROWS - 1) * GAP)) / ROWS;

    const ObservationHandle leftHandles[ROWS] = {
        currentProductionHandle_,
        currentConsumptionHandle_,
        currentExportHandle_,
        currentBatteryHandle_
    };

    const ObservationHandle rightHandles[ROWS] = {
        todayProductionHandle_,
        todayConsumptionHandle_,
        todayExportHandle_,
        todayBatteryHandle_
    };

    const char* rowLabels[ROWS] = {
        "Production",
        "Consumption",
        "Export",
        "Battery"
    };

    for (int row = 0; row < ROWS; row++)
    {
        const int y = gridTop + row * (rowH + GAP);

        drawQuadrant(
            MARGIN,
            y,
            colW,
            rowH,
            rowLabels[row],
            leftHandles[row]);

        drawQuadrant(
            MARGIN + colW + GAP,
            y,
            colW,
            rowH,
            rowLabels[row],
            rightHandles[row]);
    }
}

void SolarScreen::drawQuadrant(
    int x,
    int y,
    int w,
    int h,
    const char* label,
    ObservationHandle handle)
{
    const SensorTile* tile =
        SensorRepository::getTile(handle);

    display_.setColor(DisplayManager::WHITE);
    display_.drawRect(x, y, w, h);

    display_.setFont(ArialMT_Plain_10);
    display_.setTextAlignment(DisplayManager::CENTER);
    display_.setColor(DisplayManager::BLUE);
    display_.drawString(x + w / 2, y + 3, label);

    display_.setFont(ArialRoundedMTBold_14);
    display_.setColor(DisplayManager::WHITE);
    display_.setTextAlignment(DisplayManager::CENTER);

    const String val =
        tile && tile->valid
            ? formatPower(tile->value)
            : "--";

    display_.drawString(x + w / 2, y + h / 2 - 7, val);
}

String SolarScreen::formatPower(float v) const
{
    if (isnan(v)) return "--";

    // Display kW / kWh for values at or above 1000.
    if (v >= 1000.0f)
    {
        return String(v / 1000.0f, 1) + "k";
    }

    return String((int)round(v));
}
