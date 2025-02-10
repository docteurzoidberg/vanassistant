#pragma once

#include "DisplayPageManager.h"
#include "DrzGraphics.h"
#include "DrzEngine.h"
#include "DrzInputs.h"

#include <iostream>

#include "pages/menu/widgets/MenuItemWidget.h"

using namespace drz;

class PageMenu : public DisplayPage {
  public:
    PageMenu() {
      gfx = DrzGraphics::Get();
      engine = DrzEngine::Get();
      inputs = DrzInputs::Get();

      // Load widgets
      assistantItem = new MenuItemWidget(0, 0, 80, 80, "solidmono8", "Assistant", Key::F1);
      roadItem = new MenuItemWidget(80, 0, 80, 80, "solidmono8", "Road", Key::F2);
      settingsItem = new MenuItemWidget(160, 0, 80, 80, "solidmono8", "Settings", Key::F3);
    }

    
  void Load() override { 
    AddWidget(assistantItem);
    AddWidget(roadItem);
    AddWidget(settingsItem);
  }

  void ReadInputs() override {
    if(inputs->GetKey(Key::E).isPressed) {
      std::cout << "E pressed" << std::endl;
    }
  }

  void Update(float elapsedTime) override {
    //TODO
  }

  void Render() override {
    gfx->Clear(BLACK);
  }
    
  protected:
    IDrzEngine* engine;
    IDrzGraphics* gfx;
    IDrzInputs* inputs;
    MenuItemWidget* assistantItem;
    MenuItemWidget* roadItem;
    MenuItemWidget* settingsItem;
};