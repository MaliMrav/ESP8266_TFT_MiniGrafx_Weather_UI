#include "TouchManager.h"

#include <Arduino.h>

#include "../config/config.h"
#include "../input/InputAction.h"
#include "../input/InputEvent.h"
#include "../input/InputManager.h"
#include "../system/DebugOverlay.h"
#include "../ui/ScreenZones.h"

TouchManager::TouchManager(
    TouchController& controller)
:
    controller_(controller)
{
}

void TouchManager::setProfile(
    Profile profile)
{
    // Only the profile changes here. wasTouched_ is deliberately not reset.
    //
    // Resetting wasTouched_ on a profile switch caused a ghost event on the
    // incoming screen: the finger that triggered navigation was still in
    // contact when the new profile became active, so the next update() call
    // saw an untouched state, cleared wasTouched_, and immediately fired a
    // second event on the new screen.
    //
    // The correct reset point is when the finger actually lifts, which
    // update() already handles via the isTouched() == false branch.
    profile_ = profile;
}

void TouchManager::emitEvent(
    InputAction action,
    const TS_Point& point)
{
    InputManager::trigger(
        InputEvent{
            action,
            millis(),
            false,
            static_cast<int16_t>(point.x),
            static_cast<int16_t>(point.y)
        });
}

void TouchManager::update()
{
    if (!controller_.isTouched())
    {
        wasTouched_ = false;
        return;
    }

    if (wasTouched_)
    {
        return;
    }

    if (millis() - lastEventMs_ < TouchConfig::DEBOUNCE_MS)
    {
        return;
    }

    const TS_Point rawPoint = controller_.getRawPoint();
    DBG_RAW(rawPoint.x, rawPoint.y);

    const TS_Point point =
        (profile_ == Profile::Calibration) ?
            rawPoint :
            controller_.getPoint();

    switch (profile_)
    {
        case Profile::Weather:
        {
            if (ScreenZones::isInHeader(point.x, point.y))
            {
                emitEvent(InputAction::TAP, point);
            }
            else if (ScreenZones::isInLeftEdge(point.x, point.y))
            {
                emitEvent(InputAction::PREVIOUS_SCREEN, point);
            }
            else if (ScreenZones::isInRightEdge(point.x, point.y))
            {
                emitEvent(InputAction::NEXT_SCREEN, point);
            }
            else
            {
                emitEvent(InputAction::SELECT, point);
            }
            break;
        }

        case Profile::Status:
        {
            if (ScreenZones::isInHeader(point.x, point.y))
            {
                emitEvent(InputAction::BACK, point);
            }
            else if (ScreenZones::isInLeftEdge(point.x, point.y))
            {
                emitEvent(InputAction::PREVIOUS_SCREEN, point);
            }
            else if (ScreenZones::isInRightEdge(point.x, point.y))
            {
                emitEvent(InputAction::NEXT_SCREEN, point);
            }
            else if (ScreenZones::isInContentUp(point.x, point.y))
            {
                emitEvent(InputAction::SCROLL_UP, point);
            }
            else if (ScreenZones::isInContentDown(point.x, point.y))
            {
                emitEvent(InputAction::SCROLL_DOWN, point);
            }
            else
            {
                emitEvent(InputAction::SELECT, point);
            }
            break;
        }

        case Profile::ControlPanel:
        {
            if (ScreenZones::isInLeftEdge(point.x, point.y))
            {
                emitEvent(InputAction::BACK, point);
            }
            else if (ScreenZones::isInRightEdge(point.x, point.y))
            {
                emitEvent(InputAction::SELECT, point);
            }
            else if (ScreenZones::isInTopStrip(point.x, point.y))
            {
                emitEvent(InputAction::SCROLL_UP, point);
            }
            else if (ScreenZones::isInBottomStrip(point.x, point.y))
            {
                emitEvent(InputAction::SCROLL_DOWN, point);
            }
            else
            {
                emitEvent(InputAction::TAP, point);
            }
            break;
        }

        case Profile::Calibration:
        {
            emitEvent(InputAction::TAP, point);
            break;
        }

        case Profile::Generic:
        default:
        {
            emitEvent(InputAction::TAP, point);
            break;
        }
    }

    wasTouched_ = true;
    lastEventMs_ = millis();
}