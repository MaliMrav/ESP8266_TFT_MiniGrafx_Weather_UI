#pragma once

// Screen is the base class for every screen in the framework.
//
// Screens own their presentation, zones, and local contextual behaviour.
//
// A Screen interprets InputEvents within its own context. An interaction
// may result in local behaviour, a ScreenIntent, or other contextual
// handling.
//
// Screens do not perform screen transitions directly. When a Screen
// determines that navigation is required, it expresses that request
// through ScreenIntent. ScreenManager owns the resulting transition.

#include "../input/InputEvent.h"
#include "ScreenKind.h"
#include "ScreenIntent.h"

class Screen
{
public:
    virtual ~Screen() = default;

    virtual void enter() {}
    virtual void leave() {}

    virtual void update() = 0;

    virtual ScreenIntent onInput(const InputEvent& event)
    {
        return ScreenIntent();
    }

    virtual ScreenKind kind() const
    {
        return ScreenKind::Generic;
    }
};