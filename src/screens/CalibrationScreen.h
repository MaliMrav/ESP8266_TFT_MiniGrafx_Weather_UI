#pragma once

// CalibrationScreen guides the user through a four-point touch calibration.
//
// It presents crosshairs at each corner in sequence, captures the raw
// touch coordinates via onInput(), then passes the four points to
// TouchController::calibrate() which calculates and saves the coefficients.
//
// SystemManager monitors isFinished() and calibrationSaved() to know
// when to transition to the main WeatherScreen.

#include "../ui/Screen.h"
#include "../display/DisplayManager.h"
#include "../touch/TouchController.h"
#include "../touch/CalibrationData.h"

class CalibrationScreen : public Screen
{
public:
    CalibrationScreen(
        DisplayManager& display,
        TouchController& touchController);

    void enter() override;
    void leave() override;
    void update() override;
    ScreenIntent onInput(const InputEvent& event) override;

    ScreenKind kind() const override
    {
        return ScreenKind::Calibration;
    }

    bool isFinished() const;
    bool calibrationSaved() const;

private:
    enum CalibrationStep
    {
        TOP_LEFT,
        TOP_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_RIGHT,
        COMPLETE
    };

    DisplayManager& display_;
    TouchController& touchController_;

    CalibrationStep step_ = TOP_LEFT;
    CalibrationData calibrationData_{};
    bool calibrationSaved_ = false;

    void drawCrosshair(int16_t x, int16_t y);
    void drawStatus();
    void capturePoint(int16_t x, int16_t y);
};