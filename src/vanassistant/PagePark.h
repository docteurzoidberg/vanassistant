#pragma once

#include "DisplayPageManager.h"
#include "DrzGraphics.h"
#include "DrzEngine.h"
#include "DrzInputs.h"

using namespace drz;


class PagePark : public DisplayPage {
  public:

    PagePark() : DisplayPage("park") {
      gfx = DrzGraphics::Get();
      engine = DrzEngine::Get();
      inputs = DrzInputs::Get();
    }

    void Load() override { 

    }

    void Activate(DisplayPage* lastPage) override {
      
    }

    void ReadInputs() override {
      if(
        inputs->GetKey(Key::K1).isPressed || 
        inputs->GetKey(Key::K2).isPressed || 
        inputs->GetKey(Key::K3).isPressed || 
        inputs->GetKey(Key::K4).isPressed
      ) {
        DisplayPageManager::GoToPage("menu");
      }
    }

    void Update(float elapsedTime) override {
      gfx->Clear(BLACK);
    }

    void Render() override {
      
    }
    
  protected:
    IDrzEngine* engine;
    IDrzGraphics* gfx;
    IDrzInputs* inputs;
};