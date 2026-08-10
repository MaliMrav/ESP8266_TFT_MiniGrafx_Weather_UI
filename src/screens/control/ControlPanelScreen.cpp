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
    // BACK has screen/page hierarchy semantics.
    //
    // A child page returns to the Control Panel menu.
    // The menu itself requests screen-level navigation.
    if (event.action == InputAction::BACK)
    {
        if (activePage_ == &menuPage_)
        {
            // Already at the Control Panel root.
            // Request screen-level navigation.
            return ScreenIntent::back();
        }

        if (activePage_)
        {
            activePage_->onLeave();
        }

        activePage_ = &menuPage_;
        activePage_->onEnter();

        return ScreenIntent();
    }

    if (activePage_)
    {
        activePage_->onInput(event);

        // Selection changes the highlighted menu item.
        // SELECT activates the currently selected page.
        if (activePage_ == &menuPage_ &&
            event.action == InputAction::SELECT)
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
        }
    }

    return ScreenIntent();
}