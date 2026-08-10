#include "ScreenManager.h"

#include "../touch/TouchManager.h"

void ScreenManager::registerScreen(Screen* screen)
{
    if (screen && registryCount_ < MAX_SCREENS)
    {
        registry_[registryCount_++] = screen;
    }
}

Screen* ScreenManager::resolve(ScreenKind kind)
{
    for (uint8_t i = 0; i < registryCount_; i++)
    {
        if (registry_[i]->kind() == kind)
        {
            return registry_[i];
        }
    }
    return nullptr;
}

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
                touchManager_->setProfile(TouchManager::Profile::ControlPanel);
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
        {
            Screen* target = resolve(intent.target);
            if (target)
            {
                activate(target);
            }
            break;
        }

        case ScreenIntentKind::BACK:
        {
            Screen* weather = resolve(ScreenKind::Weather);
            if (weather)
            {
                activate(weather);
            }
            break;
        }

        case ScreenIntentKind::NONE:
        default:
            break;
    }
}