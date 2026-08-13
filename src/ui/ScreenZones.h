#pragma once

// ScreenZones divides the display into named hit regions.
//
// TouchManager uses these zones to translate a touch coordinate into
// a semantic InputAction without any screen needing to know about
// pixel boundaries. Zones are defined once here and reused across
// all touch profiles.
//
// Adjusting zone boundaries (e.g. a wider edge swipe area) requires
// a change in this file only.

#include <Arduino.h>

namespace ScreenZones
{
    constexpr int SCREEN_WIDTH  = 240;
    constexpr int SCREEN_HEIGHT = 320;

    constexpr int HEADER_HEIGHT = 58;

    constexpr int EDGE_WIDTH = 40;

    constexpr int CONTENT_TOP    = HEADER_HEIGHT;
    constexpr int CONTENT_BOTTOM = SCREEN_HEIGHT;

    constexpr int CONTENT_MID_Y =
        CONTENT_TOP + ((CONTENT_BOTTOM - CONTENT_TOP) / 2);

    constexpr int STRIP_HEIGHT = 60;

    inline bool isInTopStrip(
        int16_t x,
        int16_t y)
    {
        return y >= CONTENT_TOP &&
               y < (CONTENT_TOP + STRIP_HEIGHT) &&
               x >= EDGE_WIDTH &&
               x < (SCREEN_WIDTH - EDGE_WIDTH);
    }

    inline bool isInBottomStrip(
        int16_t x,
        int16_t y)
    {
        return y >= (CONTENT_BOTTOM - STRIP_HEIGHT) &&
               y < CONTENT_BOTTOM &&
               x >= EDGE_WIDTH &&
               x < (SCREEN_WIDTH - EDGE_WIDTH);
    }

    inline bool isInHeader(
        int16_t x,
        int16_t y)
    {
        (void)x;
        return y < HEADER_HEIGHT;
    }

    inline bool isInLeftEdge(
        int16_t x,
        int16_t y)
    {
        return y >= HEADER_HEIGHT &&
               x < EDGE_WIDTH;
    }

    inline bool isInRightEdge(
        int16_t x,
        int16_t y)
    {
        return y >= HEADER_HEIGHT &&
               x >= (SCREEN_WIDTH - EDGE_WIDTH);
    }

    inline bool isInContentUp(
        int16_t x,
        int16_t y)
    {
        return y >= CONTENT_TOP &&
               y < CONTENT_MID_Y &&
               x >= EDGE_WIDTH &&
               x < (SCREEN_WIDTH - EDGE_WIDTH);
    }

    inline bool isInContentDown(
        int16_t x,
        int16_t y)
    {
        return y >= CONTENT_MID_Y &&
               y < CONTENT_BOTTOM &&
               x >= EDGE_WIDTH &&
               x < (SCREEN_WIDTH - EDGE_WIDTH);
    }

    inline bool isInContentArea(
        int16_t x,
        int16_t y)
    {
        return y >= CONTENT_TOP &&
               y < CONTENT_BOTTOM &&
               x >= EDGE_WIDTH &&
               x < (SCREEN_WIDTH - EDGE_WIDTH);
    }
}