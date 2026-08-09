#pragma once

// ScreenIntent represents a request from a Screen to the ScreenManager.
//
// A Screen may interpret an InputEvent as requiring screen-level
// navigation. It expresses that interpretation through ScreenIntent.
//
// ScreenIntent does not perform navigation and does not reference
// ScreenManager. ScreenManager consumes the intent and owns the
// resulting transition.
//
// Local behaviour remains inside the Screen.
// System-level behaviour remains outside this contract.

#include "ScreenKind.h"

enum class ScreenIntentKind
{
    NONE,
    NAVIGATE,
    BACK
};

struct ScreenIntent
{
    // What the Screen is requesting.
    ScreenIntentKind kind = ScreenIntentKind::NONE;

    // Destination requested by NAVIGATE.
    //
    // Not meaningful for NONE or BACK.
    ScreenKind target = ScreenKind::Generic;

    ScreenIntent() {}

    explicit ScreenIntent(ScreenIntentKind kind)
        : kind(kind)
    {
    }

    ScreenIntent(ScreenIntentKind kind, ScreenKind target)
        : kind(kind)
        , target(target)
    {
    }

    static ScreenIntent navigateTo(ScreenKind target)
    {
        return ScreenIntent(ScreenIntentKind::NAVIGATE, target);
    }

    static ScreenIntent back()
    {
        return ScreenIntent(ScreenIntentKind::BACK);
    }
};