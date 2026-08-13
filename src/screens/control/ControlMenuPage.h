#pragma once

#include "ControlPage.h"
#include "ControlPageKind.h"
#include "../../ui/ScreenIntent.h"

class DisplayManager;

class ControlMenuPage : public ControlPage
{
public:

    const char* title() const override;

    void render(DisplayManager& display) override;

    ScreenIntent onInput(const InputEvent& event) override;

    ControlPageKind selectedPage() const
    {
        return selectedPage_;
    }

private:
    static constexpr int ITEMS_Y      = 30;
    static constexpr int LINE_HEIGHT  = 18;

    ControlPageKind pageForY(int16_t y) const;

    ControlPageKind selectedPage_ = ControlPageKind::About;
};