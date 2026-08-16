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
// Navigation history is maintained as a shallow stack. NAVIGATE pushes
// the destination onto the stack. BACK pops it, returning to wherever
// the user came from. The stack has a fixed maximum depth — sufficient
// for the current navigation tree and protective against unbounded growth
// on a memory-constrained device.

#include "Screen.h"
#include "ScreenKind.h"
#include "../input/InputEvent.h"

class TouchManager;

class ScreenManager
{
public:
    void bindTouchManager(TouchManager* touchManager);

    void registerScreen(Screen* screen);

    // Activates a screen without pushing to the history stack.
    // Used for boot and calibration transitions that should not
    // be part of the user-navigable history.
    void activate(Screen* screen);

    void update();

    void onInput(const InputEvent& event);

private:
    void applyProfile(Screen* screen);

    // Pushes destination onto the history stack and activates it.
    void navigateTo(Screen* screen);

    // Pops the history stack and returns to the previous screen.
    void navigateBack();

    Screen* resolve(ScreenKind kind);

    Screen* currentScreen_ = nullptr;
    TouchManager* touchManager_ = nullptr;

    static constexpr uint8_t MAX_SCREENS = 8;
    Screen* registry_[MAX_SCREENS] = {};
    uint8_t registryCount_ = 0;

    static constexpr uint8_t MAX_HISTORY = 8;
    Screen* history_[MAX_HISTORY] = {};
    uint8_t historyDepth_ = 0;
};