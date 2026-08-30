#include "BootScreen.h"

#include <ArialRounded.h>

BootScreen::BootScreen(
    DisplayManager& display)
:
    display_(display)
{
}

void BootScreen::setStatus(
    const String& message,
    uint8_t progress)
{
    status_   = message;
    progress_ = progress;

    update();
}

void BootScreen::update()
{
    display_.clear(
        DisplayManager::BLACK);

    display_.setFont(
        ArialRoundedMTBold_14);

    display_.setTextAlignment(
        DisplayManager::CENTER);

    display_.setColor(
        DisplayManager::WHITE);

    display_.drawString(
        120,
        50,
        "Vlad's Telemetry UI");

    display_.setColor(
        DisplayManager::YELLOW);

    display_.drawString(
        120,
        120,
        status_);

    display_.setColor(
        DisplayManager::WHITE);

    display_.drawRect(
        10,
        168,
        220,
        15);

    display_.setColor(
        DisplayManager::BLUE);

    display_.fillRect(
        12,
        170,
        (216 * progress_) / 100,
        11);

    display_.setColor(
        DisplayManager::WHITE);

    display_.drawString(
        120,
        200,
        String(progress_) + "%");

    display_.commit();
}