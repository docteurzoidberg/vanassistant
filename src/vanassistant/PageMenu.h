#pragma once

#include "DisplayPageManager.h"
#include "DrzGraphics.h"
#include "DrzInputs.h"

#include <sys/types.h>
#include <vector>
#include <iostream>

#include "pages/common/widgets/Starfield.h"
#include "pages/menu/widgets/MenuItemWidget.h"
#include "pages/menu/widgets/J7Scene3d.h"

using namespace drz;

#define BACK_TIMER 5

class PageMenu : public DisplayPage {
  public:

    static void AssistantCallback();
    static void RoadCallback();
    static void SettingsCallback();
    static void ParkCallback();
    PageMenu();
    void Load();
    void Activate(DisplayPage* lastPage);
    void ReadInputs();
    void Update(float elapsedTime);
    void Render();
    void StartTimer();
    
  protected:
    IDrzEngine* engine;
    IDrzGraphics* gfx;
    IDrzInputs* inputs;
    MenuItemWidget* assistantItem;
    MenuItemWidget* roadItem;
    MenuItemWidget* settingsItem;
    MenuItemWidget* parkItem;
    std::vector<MenuItemWidget*> menuItems;
    WidgetJ7Scene3d* j7scene;
    Starfield* starfield;
    float currentTimer = 0.0f;
    uint8_t backTimer = BACK_TIMER;
    bool timerActive = false;
};