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

        case InputAction::TAP:
            if (event.hasPosition)
            {
                selectedPage_ = pageForY(event.position.y);
            }
            break;

        default:
            break;
    }

    return ScreenIntent();
}

ControlPageKind ControlMenuPage::pageForY(int16_t y) const
{
    int index = (y - ITEMS_Y) / LINE_HEIGHT;

    switch (index)
    {
        case 0:  return ControlPageKind::About;
        case 1:  return ControlPageKind::Connectivity;
        default: return selectedPage_;
    }
}

void ControlMenuPage::render(DisplayManager& display)
{
    constexpr int LEFT = 10;

    display.clear(DisplayManager::BLACK);

    display.setColor(DisplayManager::WHITE);
    display.setTextAlignment(DisplayManager::CENTER);

    display.drawString(display.getWidth() / 2, 4, title());

    display.setTextAlignment(DisplayManager::LEFT);

    int y = ITEMS_Y;

    display.drawString(
        LEFT, y,
        selectedPage_ == ControlPageKind::About
            ? "> About"
            : "  About");

    y += LINE_HEIGHT;

    display.drawString(
        LEFT, y,
        selectedPage_ == ControlPageKind::Connectivity
            ? "> Connectivity"
            : "  Connectivity");

    display.commit();
}