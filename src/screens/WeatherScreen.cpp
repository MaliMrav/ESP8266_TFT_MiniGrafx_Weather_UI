#include "WeatherScreen.h"

#include "../display/DisplayManager.h"

#include "../data/ObservationRegistry.h"

#include "../models/WeatherObservationKeys.h"
#include "../models/SensorRepository.h"

#include "../config/config.h"
#include "../system/DebugOverlay.h"
#include "ScreenConfig.h"

#include <ArialRounded.h>
#include <ESP8266WiFi.h>
#include <time.h>

WeatherScreen::WeatherScreen(DisplayManager& display)
  : display_(display) {}

void WeatherScreen::enter()
{
    kitchenTemperatureHandle_ =
        ObservationRegistry::resolve(
            WeatherObservations::KITCHEN_TEMPERATURE);

    pergolaTemperatureHandle_ =
        ObservationRegistry::resolve(
            WeatherObservations::PERGOLA_TEMPERATURE);

    kitchenHumidityHandle_ =
        ObservationRegistry::resolve(
            WeatherObservations::KITCHEN_HUMIDITY);

    pergolaHumidityHandle_ =
        ObservationRegistry::resolve(
            WeatherObservations::PERGOLA_HUMIDITY);

    pressureHandle_ =
        ObservationRegistry::resolve(
            WeatherObservations::PRESSURE);
}

void WeatherScreen::leave()
{
}

void WeatherScreen::update()
{
    display_.clear(DisplayManager::BLACK);
    drawHeader();
    drawWifiQuality();
    drawSensorGrid();
    display_.commit();
    DBG_DRAW(display_);
}

bool WeatherScreen::isClockArea(
    const InputPosition& position) const
{
    return position.x >= ScreenConfig::CLOCK_AREA_LEFT &&
           position.x < ScreenConfig::CLOCK_AREA_RIGHT &&
           position.y >= ScreenConfig::CLOCK_AREA_TOP &&
           position.y < ScreenConfig::CLOCK_AREA_BOTTOM;
}

ScreenIntent WeatherScreen::onInput(const InputEvent& event)
{
    DBG_RECORD(event);

    switch (event.action)
    {
        case InputAction::TAP:

            if (event.hasPosition &&
                isClockArea(event.position))
            {
                use12HourClock_ = !use12HourClock_;
                update();
            }

            break;

        case InputAction::NEXT_SCREEN:
            return ScreenIntent::navigateTo(ScreenKind::Solar);

        case InputAction::SELECT:
            // Contextual selection.
            break;

        default:
            break;
    }

    return ScreenIntent();
}

void WeatherScreen::drawHeader()
{
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    if (!t)
    {
        return;
    }

    display_.setTextAlignment(DisplayManager::CENTER);
    display_.setColor(DisplayManager::WHITE);

    display_.setFont(ArialRoundedMTBold_14);
    String date = TimeConfig::WDAY_NAMES[t->tm_wday] + " " +
                  TimeConfig::MONTH_NAMES[t->tm_mon] + " " +
                  String(t->tm_mday);

    display_.drawString(
        display_.getWidth() / 2,
        ScreenConfig::HEADER_DATE_Y,
        date);

    display_.setFont(ArialRoundedMTBold_36);

    char buf[10];
    if (use12HourClock_)
    {
        int hour = t->tm_hour % 12;
        if (hour == 0)
        {
            hour = 12;
        }

        snprintf(buf, sizeof(buf), "%d:%02d:%02d", hour, t->tm_min, t->tm_sec);
    }
    else
    {
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
    }

    display_.setColor(DisplayManager::WHITE);

    display_.drawString(
        display_.getWidth() / 2,
        ScreenConfig::HEADER_TIME_Y,
        buf);

    if (use12HourClock_)
    {
        const char* amPm =
            t->tm_hour < 12 ? "AM" : "PM";

        display_.setFont(ArialRoundedMTBold_14);
        display_.setTextAlignment(DisplayManager::CENTER);

        display_.setColor(DisplayManager::WHITE);
        display_.fillRect(8, 32, 34, 18);

        display_.setColor(DisplayManager::BLUE);
        display_.drawString(25, 34, amPm);
    }
}

int8_t WeatherScreen::getWifiQuality()
{
    int32_t dbm = WiFi.RSSI();
    if (dbm <= -100) return 0;
    if (dbm >= -50) return 100;
    return 2 * (dbm + 100);
}

void WeatherScreen::drawWifiQuality()
{
    int8_t q = getWifiQuality();

    display_.setTextAlignment(DisplayManager::RIGHT);
    display_.setFont(ArialMT_Plain_10);
    display_.setColor(DisplayManager::WHITE);
    display_.drawString(228, 9, String(q) + "%");

    for (int8_t i = 0; i < 4; i++)
    {
        for (int8_t j = 0; j < 2 * (i + 1); j++)
        {
            if (q > i * 25 || j == 0)
            {
                display_.setPixel(230 + 2 * i, 18 - j);
            }
        }
    }
}

void WeatherScreen::drawSensorGrid()
{
    const ObservationHandle handles[] = {
        kitchenTemperatureHandle_,
        pergolaTemperatureHandle_,
        kitchenHumidityHandle_,
        pergolaHumidityHandle_,
        pressureHandle_
    };

    constexpr uint8_t sensorCount =
        sizeof(handles) / sizeof(handles[0]);

    const int margin = ScreenConfig::SIDE_MARGIN;
    const int gap    = ScreenConfig::TILE_GAP;
    const int topY   = ScreenConfig::TOP_MARGIN;

    const int cols = ScreenConfig::COLUMNS;
    const int rows =
        (sensorCount + cols - 1) / cols;

    const int tileW =
        (display_.getWidth() -
         (2 * margin) -
         ((cols - 1) * gap)) / cols;

    const int tileH =
        (display_.getHeight() -
         topY -
         ((rows - 1) * gap)) / rows;

    for (uint8_t i = 0; i < sensorCount; i++)
    {
        uint8_t row = i / cols;
        uint8_t col = i % cols;

        bool full =
            ScreenConfig::LAST_TILE_FULL_WIDTH &&
            (i == sensorCount - 1) &&
            (sensorCount % cols == 1);

        int x =
            full
                ? margin
                : margin + col * (tileW + gap);

        int w =
            full
                ? display_.getWidth() - (2 * margin)
                : tileW;

        int y =
            topY + row * (tileH + gap);

        SensorTile* s =
            SensorRepository::getTile(handles[i]);

        if (!s)
        {
            continue;
        }

        display_.setColor(DisplayManager::WHITE);
        display_.drawRect(
            x,
            y,
            w,
            tileH);

        display_.setFont(ArialRoundedMTBold_14);
        display_.setColor(getColor(s->type));
        display_.setTextAlignment(DisplayManager::CENTER);

        display_.drawString(
            x + w / 2,
            y + 10,
            s->label);

        display_.setFont(ArialMT_Plain_24);

        String valueStr =
            formatValue(s->type, s->value) + s->unit;

        int valueWidth =
            display_.getStringWidth(valueStr);

        const int arrowOffset =
            ScreenConfig::ARROW_OFFSET;

        const int arrowWidth =
            ScreenConfig::ARROW_WIDTH;

        const int spacing =
            ScreenConfig::SPACING;

        const int rightPad =
            ScreenConfig::RIGHT_PAD;

        int arrowCenterX =
            x + arrowOffset;

        int contentLeft =
            x +
            arrowOffset +
            arrowWidth +
            spacing;

        int contentRight =
            x + w - rightPad;

        int contentWidth =
            contentRight - contentLeft;

        int cy =
            y + tileH / 2 - 8;

        int valueStartX =
            contentLeft +
            (contentWidth - valueWidth) / 2;

        if (s->trend != TREND_NONE)
        {
            display_.setColor(
                s->trend == TREND_UP   ? DisplayManager::YELLOW :
                s->trend == TREND_DOWN ? DisplayManager::BLUE   :
                                         DisplayManager::WHITE);

            drawTrendArrow(
                arrowCenterX,
                cy + 10,
                s->trend);
        }

        display_.setColor(DisplayManager::WHITE);
        display_.setTextAlignment(DisplayManager::LEFT);

        display_.drawString(
            valueStartX,
            cy,
            valueStr);

        display_.setTextAlignment(DisplayManager::CENTER);
        display_.setFont(ArialMT_Plain_10);
        display_.setColor(DisplayManager::BLUE);

        if (!isnan(s->minVal) &&
            !isnan(s->maxVal))
        {
            String mm =
                formatValue(s->type, s->minVal) +
                " / " +
                formatValue(s->type, s->maxVal);

            display_.drawString(
                x + w / 2,
                y + tileH - 18,
                mm);
        }
        else
        {
            display_.drawString(
                x + w / 2,
                y + tileH - 18,
                "-- / --");
        }
    }
}

void WeatherScreen::drawTrendArrow(int x, int y, TrendDirection t)
{
    auto thick = [&](int x1, int y1, int x2, int y2)
    {
        display_.drawLine(x1, y1, x2, y2);
        display_.drawLine(x1 + 1, y1, x2 + 1, y2);
    };

    switch (t)
    {
        case TREND_UP:
            thick(x, y + 6, x, y - 6);
            thick(x, y - 6, x - 3, y - 2);
            thick(x, y - 6, x + 3, y - 2);
            break;

        case TREND_DOWN:
            thick(x, y - 6, x, y + 6);
            thick(x, y + 6, x - 3, y + 2);
            thick(x, y + 6, x + 3, y + 2);
            break;

        case TREND_FLAT:
            thick(x - 3, y, x + 3, y);
            break;

        default:
            break;
    }
}

String WeatherScreen::formatValue(
    SensorType type,
    float v)
{
    if (isnan(v))
    {
        return "--";
    }

    switch (type)
    {
        case TEMP:
            return String(v, 1);

        case HUMIDITY:
        case PRESSURE:
            return String((int)round(v));

        default:
            return "--";
    }
}

DisplayManager::Color WeatherScreen::getColor(SensorType t)
{
    if (t == HUMIDITY) return DisplayManager::BLUE;
    if (t == PRESSURE) return DisplayManager::YELLOW;
    return DisplayManager::WHITE;
}