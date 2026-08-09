#pragma once

#include "../ControlPage.h"

class ConnectivityPage : public ControlPage
{
public:
    const char* title() const override;

    void render(DisplayManager& display) override;

    ScreenIntent onInput(const InputEvent& event) override;
};