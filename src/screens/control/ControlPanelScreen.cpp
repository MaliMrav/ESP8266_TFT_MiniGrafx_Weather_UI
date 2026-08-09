#include "ControlPanelScreen.h"

ControlPanelScreen::ControlPanelScreen(DisplayManager& display)
    : display_(display)
    , activePage_(nullptr)
{
    activate(menuPage_);
}

void ControlPanelScreen::activate(ControlPage& page)
{
    if (activePage_)
    {
        activePage_->onLeave();
    }

    activePage_ = &page;

    activePage_->onEnter();
}

void ControlPanelScreen::update()
{
    if (activePage_)
    {
        activePage_->render(display_);
    }
}

ScreenIntent ControlPanelScreen::onInput(const InputEvent& event)
{
    if (activePage_)
    {
        activePage_->onInput(event);

        // If the active page is the menu, check whether the user has
        // selected a different page.
        if (activePage_ == &menuPage_)
        {
            switch (menuPage_.selectedPage())
            {
                case ControlPageKind::About:
                    activate(aboutPage_);
                    break;

                case ControlPageKind::Connectivity:
                    activate(connectivityPage_);
                    break;
            }

            return ScreenIntent();
        }
    }

    if (event.action == InputAction::BACK)
    {
        if (activePage_ == &menuPage_)
        {
            // Already on the menu page, so leave Control Panel.
            // Screen-level navigation will be handled by ScreenManager.

            return ScreenIntent();
        }

        activePage_->onLeave();
        activePage_ = &menuPage_;
        activePage_->onEnter();

        return ScreenIntent();
    }

    return ScreenIntent();
}