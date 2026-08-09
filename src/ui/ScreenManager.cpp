#include "ScreenManager.h"

#include "../touch/TouchManager.h"

void ScreenManager::bindTouchManager(TouchManager* touchManager)
{
    touchManager_ = touchManager;
}

void ScreenManager::activate(Screen* screen)
{
    if (currentScreen_)
    {
        currentScreen_->leave();
    }

    currentScreen_ = screen;

    if (touchManager_ && currentScreen_)
    {
        switch (currentScreen_->kind())
        {
            case ScreenKind::Weather:
                touchManager_->setProfile(TouchManager::Profile::Weather);
                break;

            case ScreenKind::ControlPanel:
                touchManager_->setProfile(TouchManager::Profile::Generic);
                break;

            case ScreenKind::Calibration:
                touchManager_->setProfile(TouchManager::Profile::Calibration);
                break;

            default:
                touchManager_->setProfile(TouchManager::Profile::Generic);
                break;
        }
    }

    if (currentScreen_)
    {
        currentScreen_->enter();
    }
}

void ScreenManager::update()
{
    if (currentScreen_)
    {
        currentScreen_->update();
    }
}

void ScreenManager::onInput(const InputEvent& event)
{
    if (!currentScreen_)
    {
        return;
    }

    ScreenIntent intent = currentScreen_->onInput(event);

    switch (intent.kind)
    {
        case ScreenIntentKind::NAVIGATE:
            // Navigation target will be resolved by ScreenManager
            // once the target Screen instance is available.
            break;

        case ScreenIntentKind::BACK:
            // Back behaviour will be defined separately.
            break;

        case ScreenIntentKind::NONE:
        default:
            break;
    }
}