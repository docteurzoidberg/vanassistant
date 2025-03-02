#pragma once

#include "DisplayPageManager.h"
#include "DrzGraphics.h"
#include "DrzEngine.h"
#include "DrzInputs.h"

#include <iostream>
#include <sys/types.h>
#include <vector>

#include "pages/common/widgets/Starfield.h"
#include "pages/menu/widgets/MenuItemWidget.h"
#include "pages/menu/widgets/J7Scene3d.h"

using namespace drz;

#define BACK_TIMER 5

class PageMenu : public DisplayPage {
  public:

    static void AssistantCallback() {
      std::cout << "Assistant callback" << std::endl;
      DisplayPageManager::GoToPage("assistant");
    }

    static void RoadCallback() {
      std::cout << "Road callback" << std::endl;
      DisplayPageManager::GoToPage("road");
    }
    
    static void SettingsCallback() {
      std::cout << "Settings callback" << std::endl;
      DisplayPageManager::GoToPage("settings");
    }

    static void ParkCallback() {
      std::cout << "Park callback" << std::endl;
      DisplayPageManager::GoToPage("park");
    }
    

    PageMenu() : DisplayPage("menu") {
      gfx = DrzGraphics::Get();
      engine = DrzEngine::Get();
      inputs = DrzInputs::Get();

      // Load widgets
      assistantItem = new MenuItemWidget(0, 10, 70, 40, "solidmono6", "ROAD", Key::K1, this->RoadCallback);
      roadItem = new MenuItemWidget(0, 70, 70, 40, "solidmono6", "PARK", Key::K2, this->ParkCallback);
      settingsItem = new MenuItemWidget(0, 130, 70, 40, "solidmono6", "ASSISTANT", Key::K3, this->AssistantCallback);
      parkItem = new MenuItemWidget(0, 190, 70, 40, "solidmono6", "SETTINGS", Key::K4, this->SettingsCallback);

      menuItems.push_back(assistantItem);
      menuItems.push_back(roadItem);
      menuItems.push_back(settingsItem);
      menuItems.push_back(parkItem);

      j7scene = new WidgetJ7Scene3d(0, 0, gfx->GetScreenWidth(), gfx->GetScreenHeight());
      starfield = new Starfield(0, 0, gfx->GetScreenWidth(), gfx->GetScreenHeight(), 750);
    }

    void Load() override { 
      AddWidget(starfield); 
      AddWidget(j7scene);
      for(auto item : menuItems) {
        AddWidget(item);
      }
    }

    void Activate(DisplayPage* lastPage) override {
      std::cout << "Menu page activated" << std::endl;
      if(lastPage!=nullptr) StartTimer();
    }

    void ReadInputs() override {
      for(auto item : menuItems) {
        if(inputs->GetKey(item->GetKey()).isPressed) {
          MenuItemCallback cb = item->GetCallback();
          cb();
          return;
        }
      }
    }

    void Update(float elapsedTime) override {

      gfx->Clear(BLACK);

      if(timerActive) {
        currentTimer+=elapsedTime;

        backTimer = BACK_TIMER - (int)currentTimer;

        if(backTimer==0) { 
          timerActive = false;
          DisplayPageManager::Back();
          std::cout << "Back" << std::endl;
        }
      }
    }

    void Render() override {
      if(timerActive) {
        gfx->SetFont("solidmono8");
        //TODO: align text bottom center
        gfx->DrawText("BACK IN " + std::to_string(backTimer), 0, 0, WHITE);
        
      }
    }

    void StartTimer() {
      currentTimer=0.0f;
      timerActive = true;
    }
    
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