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

void ScreenManager::applyProfile(Screen* screen)
{
    if (!touchManager_ || !screen) return;

    switch (screen->kind())
    {
        case ScreenKind::Weather:
        case ScreenKind::Solar:
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

void ScreenManager::activate(Screen* screen)
{
    // activate() is used for boot and calibration transitions.
    // It does not push to the history stack — these transitions
    // are not part of the user-navigable history.
    if (currentScreen_)
    {
        currentScreen_->leave();
    }

    currentScreen_ = screen;
    applyProfile(currentScreen_);

    if (currentScreen_)
    {
        currentScreen_->enter();
    }
}

void ScreenManager::navigateTo(Screen* screen)
{
    if (!screen) return;

    if (currentScreen_)
    {
        currentScreen_->leave();

        // Push current screen onto the history stack before leaving.
        if (historyDepth_ < MAX_HISTORY)
        {
            history_[historyDepth_++] = currentScreen_;
        }
    }

    currentScreen_ = screen;
    applyProfile(currentScreen_);
    currentScreen_->enter();
}

void ScreenManager::navigateBack()
{
    if (historyDepth_ == 0) return;

    if (currentScreen_)
    {
        currentScreen_->leave();
    }

    currentScreen_ = history_[--historyDepth_];
    applyProfile(currentScreen_);
    currentScreen_->enter();
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
                navigateTo(target);
            }
            break;
        }

        case ScreenIntentKind::BACK:
        {
            navigateBack();
            break;
        }

        case ScreenIntentKind::NONE:
        default:
            break;
    }
}