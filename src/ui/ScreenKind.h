#pragma once

// ScreenKind identifies a screen within the framework.
//
// ScreenKind is a shared vocabulary used by the screen architecture.
// It does not define how screens are implemented or how navigation
// occurs.
//
// ScreenManager uses ScreenKind to identify screens.
// ScreenIntent uses ScreenKind to express navigation destinations.
// Individual Screen implementations report their own kind.

enum class ScreenKind
{
    Generic,

    Boot,
    Weather,
    Status,
    Calibration,
    ControlPanel,
    Solar,
    ConnectivityPage
};