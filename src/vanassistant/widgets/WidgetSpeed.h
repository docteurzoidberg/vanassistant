#pragma once

#include <IDrzEngine.h>
#include <string>
#include "../IWidget.h"

class WidgetSpeed : public IWidget {
  public:

    unsigned int speed = 0;

    WidgetSpeed(int screenX, int screenY, int width, int height, drz::font* font) : IWidget(screenX, screenY, width, height) {
      speedFont = font;
    }

    void SetSpeed(unsigned int speed) {
      this->speed = speed;
    }
    
    // Load the widget
    void Load() override {
      //font loaded by caller
    }
    
    // Update the widget 
    void Update(float fElapsedTime) override {  
      //calc text bounds
      engine->SetFont(speedFont);
      textBounds = engine->GetTextBounds(std::to_string(speed), 0, 0);
    }
    
    // Render the widget
    void Render() override {
      //draw text centered
      engine->SetFont(speedFont);
      DrawText(std::to_string(speed),  (width - textBounds.w) / 2,  (height - textBounds.h) / 2, drz::color(255, 255, 255)); 
    }


  protected:
    drz::font* speedFont;

  private:
    drz::rect textBounds;

};