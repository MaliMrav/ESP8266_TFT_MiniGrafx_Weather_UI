#include "CalibrationScreen.h"

#include <ArialRounded.h>

CalibrationScreen::CalibrationScreen(
    DisplayManager& display,
    TouchController& touchController)
:
    display_(display),
    touchController_(touchController)
{
}

void CalibrationScreen::enter()
{
    calibrationData_ = {};
    calibrationSaved_ = false;
    step_ = TOP_LEFT;
    update();
}

void CalibrationScreen::leave()
{
    display_.clear(DisplayManager::BLACK);
    display_.commit();
}

void CalibrationScreen::update()
{
    display_.clear(DisplayManager::BLACK);

    switch (step_)
    {
        case TOP_LEFT:
            drawCrosshair(20, 20);
            break;

        case TOP_RIGHT:
            drawCrosshair(220, 20);
            break;

        case BOTTOM_LEFT:
            drawCrosshair(20, 300);
            break;

        case BOTTOM_RIGHT:
            drawCrosshair(220, 300);
            break;

        case COMPLETE:
            drawStatus();
            break;
    }

    display_.commit();
}

void CalibrationScreen::drawCrosshair(int16_t x, int16_t y)
{
    display_.setColor(DisplayManager::WHITE);
    display_.drawLine(x - 10, y, x + 10, y);
    display_.drawLine(x, y - 10, x, y + 10);
}

void CalibrationScreen::drawStatus()
{
    display_.setTextAlignment(DisplayManager::CENTER);
    display_.setColor(DisplayManager::WHITE);
    display_.setFont(ArialRoundedMTBold_14);

    display_.drawString(
        120,
        150,
        "Calibration Complete");

    display_.setFont(ArialMT_Plain_10);
    display_.setColor(
        calibrationSaved_ ? DisplayManager::BLUE : DisplayManager::YELLOW);

    display_.drawString(
        120,
        170,
        calibrationSaved_ ? "Saved: YES" : "Saved: NO");

    display_.setColor(DisplayManager::WHITE);

    display_.drawString(
        120,
        190,
        "TL " + String(calibrationData_.topLeft.x) + "," + String(calibrationData_.topLeft.y));

    display_.drawString(
        120,
        205,
        "TR " + String(calibrationData_.topRight.x) + "," + String(calibrationData_.topRight.y));

    display_.drawString(
        120,
        220,
        "BL " + String(calibrationData_.bottomLeft.x) + "," + String(calibrationData_.bottomLeft.y));

    display_.drawString(
        120,
        235,
        "BR " + String(calibrationData_.bottomRight.x) + "," + String(calibrationData_.bottomRight.y));
}

ScreenIntent CalibrationScreen::onInput(const InputEvent& event)
{
    if (event.action != InputAction::TAP ||
        !event.hasPosition ||
        step_ == COMPLETE)
    {
        return ScreenIntent();
    }

    capturePoint(event.position.x, event.position.y);

    if (step_ == COMPLETE)
    {
        calibrationSaved_ = touchController_.calibrate(calibrationData_);
    }

    update();
    return ScreenIntent();
}

void CalibrationScreen::capturePoint(int16_t x, int16_t y)
{
    switch (step_)
    {
        case TOP_LEFT:
            calibrationData_.topLeft = TS_Point(x, y, 0);
            step_ = TOP_RIGHT;
            break;

        case TOP_RIGHT:
            calibrationData_.topRight = TS_Point(x, y, 0);
            step_ = BOTTOM_LEFT;
            break;

        case BOTTOM_LEFT:
            calibrationData_.bottomLeft = TS_Point(x, y, 0);
            step_ = BOTTOM_RIGHT;
            break;

        case BOTTOM_RIGHT:
            calibrationData_.bottomRight = TS_Point(x, y, 0);
            step_ = COMPLETE;
            break;

        case COMPLETE:
            break;
    }
}

bool CalibrationScreen::isFinished() const
{
    return step_ == COMPLETE;
}

bool CalibrationScreen::calibrationSaved() const
{
    return calibrationSaved_;
}