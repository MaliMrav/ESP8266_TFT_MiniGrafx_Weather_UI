#include "ControlMenuPage.h"

#include "../../display/DisplayManager.h"

const char* ControlMenuPage::title() const
{
    return "Control Panel";
}

ScreenIntent ControlMenuPage::onInput(const InputEvent& event)
{
    switch (event.action)
    {
        case InputAction::SCROLL_UP:

            selectedPage_ = ControlPageKind::About;
            break;

        case InputAction::SCROLL_DOWN:

            selectedPage_ = ControlPageKind::Connectivity;
            break;

        default:
            break;
    }

    return ScreenIntent();
}

void ControlMenuPage::render(DisplayManager& display)
{
    constexpr int LEFT = 10;
    constexpr int LINE_HEIGHT = 18;

    display.clear(DisplayManager::BLACK);

    display.setColor(DisplayManager::WHITE);
    display.setTextAlignment(DisplayManager::CENTER);

    display.drawString(display.getWidth() / 2, 4, title());

    // display.drawHorizontalLine(0, 18, display.getWidth());

    display.setTextAlignment(DisplayManager::LEFT);

    int y = 30;

    display.drawString(
        LEFT,
        y,
        selectedPage_ == ControlPageKind::About
            ? "> About"
            : "  About");

    y += LINE_HEIGHT;

    display.drawString(
        LEFT,
        y,
        selectedPage_ == ControlPageKind::Connectivity
            ? "> Connectivity"
            : "  Connectivity");

    display.commit();
}