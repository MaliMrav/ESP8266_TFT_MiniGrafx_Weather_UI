#pragma once

// ScreenManager owns screen-level navigation and screen transitions.
//
// Screens do not manipulate ScreenManager directly. They express
// navigation requests through ScreenIntent, which ScreenManager
// interprets and executes.
//
// ScreenManager does not interpret the contextual meaning of InputEvents.
// That responsibility belongs to the active Screen (or the appropriate
// higher-level context for system-level behaviour).
//
// Navigation history and more advanced transition policies are
// intentionally outside the current thin-slice implementation.

#include "Screen.h"
#include "ScreenKind.h"
#include "../input/InputEvent.h"

class TouchManager;

class ScreenManager
{
public:
    void bindTouchManager(TouchManager* touchManager);

    void registerScreen(Screen* screen);

    void activate(Screen* screen);

    void update();

    void onInput(const InputEvent& event);

private:
    Screen* resolve(ScreenKind kind);

    Screen* currentScreen_ = nullptr;
    TouchManager* touchManager_ = nullptr;

    static constexpr uint8_t MAX_SCREENS = 8;
    Screen* registry_[MAX_SCREENS] = {};
    uint8_t registryCount_ = 0;
};