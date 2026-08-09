#include "ConnectivityPage.h"

#include "../../../display/DisplayManager.h"

const char* ConnectivityPage::title() const
{
    return "Connectivity";
}

void ConnectivityPage::render(DisplayManager& display)
{
    display.clear(DisplayManager::BLACK);

    display.setColor(DisplayManager::WHITE);
    display.setTextAlignment(DisplayManager::CENTER);

    display.drawString(display.getWidth() / 2, 4, title());

    display.commit();
}

ScreenIntent ConnectivityPage::onInput(const InputEvent&)
{
    // MVP
    return ScreenIntent();
}