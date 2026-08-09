#pragma once

#include "../../ui/Screen.h"

#include "ControlMenuPage.h"
#include "information/AboutPage.h"
#include "capabilities/ConnectivityPage.h"

class DisplayManager;

class ControlPanelScreen : public Screen
{
public:

    explicit ControlPanelScreen(DisplayManager& display);

    void update() override;

    void activate(ControlPage& page);

    ScreenIntent onInput(const InputEvent& event) override;

    ScreenKind kind() const override
    {
        return ScreenKind::ControlPanel;
    }

private:

    DisplayManager& display_;

    ControlPage* activePage_;

    ControlMenuPage menuPage_;

    AboutPage aboutPage_;

    ConnectivityPage connectivityPage_;
};
