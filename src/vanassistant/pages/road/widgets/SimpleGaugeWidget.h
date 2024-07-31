#pragma once

#include <IDrzEngine.h>

using namespace drz;

class SimpleVerticalGaugeWidget : public Widget {
  public:
  
    int min=0;
    int max=100;
    int value=0;

    SimpleVerticalGaugeWidget(int x, int y, int w, int h, int min, int max, int value) : min(min), max(max), value(value), Widget(x, y, w, h) {
      SetBarRect({0, 0, w, h});
    }

    void SetMask(Sprite* mask) {
      this->mask = mask;
    }

    void SetBarRect(rect r) {
      startValueDrawX = r.x;
      startValueDrawY = r.y;
      endValueDrawX = r.x + r.w;
      endValueDrawY = r.y + r.h;
      valueBarHeight = endValueDrawY - startValueDrawY;
      valueBarWidth = endValueDrawX - startValueDrawX;
    }

    void Load() override {
    }

    void Update(float fElapsedTime) override {
    }

    void Render() override {
    
      //Clear the value area 
      engine->FillRect(screenX+ startValueDrawX, screenY + startValueDrawY, valueBarWidth, valueBarHeight, BLACK);

      //Draw value bar
      auto valueHeight = (value - min) * (endValueDrawY - startValueDrawY) / (max - min);
      engine->FillRect(screenX + startValueDrawX, screenY + endValueDrawY - valueHeight, valueBarWidth, valueHeight, WHITE);

      //Draw front mask
      if(mask!=nullptr) {
        engine->DrawMaskSprite(mask, screenX, screenY);
      }
    }

  private:
    int startValueDrawY;
    int startValueDrawX;

    int endValueDrawY;
    int endValueDrawX;

    int valueBarWidth;
    int valueBarHeight;

    Sprite* mask = nullptr;

};