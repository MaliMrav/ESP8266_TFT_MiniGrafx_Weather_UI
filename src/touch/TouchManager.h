#pragma once

// TouchManager translates raw touch hardware events into abstract InputEvents.
//
// Responsibilities:
//   - poll TouchController each loop
//   - apply debounce to prevent event flooding
//   - interpret touch coordinates using the active Profile
//   - emit InputActions via InputManager::trigger()
//
// Profile determines how screen zones map to actions. Each screen
// context that requires specialised touch semantics gets its own profile
// so the same physical touch can mean different things in different
// contexts.
//
// It implements IInputSource so InputManager treats it identically
// to any other input device (button, encoder, etc.).

#include "TouchController.h"
#include "../input/InputSource.h"
#include "../input/InputAction.h"

class TouchManager : public IInputSource
{
public:
    enum class Profile
    {
        Generic,
        Weather,
        Status,
        ControlPanel,
        Calibration
    };

    explicit TouchManager(
        TouchController& controller);

    void setProfile(Profile profile);
    void update() override;

private:
    TouchController& controller_;
    Profile profile_ = Profile::Generic;

    bool wasTouched_ = false;
    unsigned long lastEventMs_ = 0;

    void emitEvent(
        InputAction action,
        const TS_Point& point);
};