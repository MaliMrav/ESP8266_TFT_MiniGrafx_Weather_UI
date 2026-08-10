#include "AboutPage.h"

#include "../../../display/DisplayManager.h"
#include "../../../system/BuildInfo.h"

const char* AboutPage::title() const
{
    return "About";
}

void AboutPage::render(DisplayManager& display)
{
    constexpr int LEFT         = 10;
    constexpr int VALUE        = 95;
    constexpr int LINE_HEIGHT = 18;

    display.clear(DisplayManager::BLACK);

    display.setColor(DisplayManager::WHITE);
    display.setTextAlignment(DisplayManager::CENTER);

    display.drawString(
        display.getWidth() / 2,
        4,
        title());

    display.setTextAlignment(DisplayManager::LEFT);

    int y = 30;

    display.drawString(LEFT, y, "Framework");
    display.drawString(VALUE, y, BuildInfo::frameworkName());

    y += LINE_HEIGHT;

    display.drawString(LEFT, y, "Version");
    display.drawString(VALUE, y, BuildInfo::version());

    y += LINE_HEIGHT;

    display.drawString(LEFT, y, "Board");
    display.drawString(VALUE, y, BuildInfo::boardName());

    y += LINE_HEIGHT;

    display.drawString(LEFT, y, "Display");
    display.drawString(VALUE, y, BuildInfo::displayName());

    // Reserved for future expansion:
    //
    // y += LINE_HEIGHT;
    // display.drawString(LEFT,  y, "Commit");
    // display.drawString(VALUE, y, BuildInfo::gitCommit());
    //
    // y += LINE_HEIGHT;
    // display.drawString(LEFT,  y, "Built");
    // display.drawString(VALUE, y, BuildInfo::buildDate());
    //
    // y += LINE_HEIGHT;
    // display.drawString(LEFT,  y, "Compiler");
    // display.drawString(VALUE, y, BuildInfo::compiler());

    display.commit();
}