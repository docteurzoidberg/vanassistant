#pragma once

#include <DrzGraphics.h>

#include "../../../Widget.h"

#include <string>

using namespace drz;

class WidgetSpeed : public Widget {
  public:

    unsigned int speed = 0;

    WidgetSpeed(int screenX, int screenY, int width, int height, font* font) : Widget(screenX, screenY, width, height) {
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
      gfx->SetFont(speedFont);
      textBounds = gfx->GetTextBounds(std::to_string(speed), 0, 0);
    }
    
    // Render the widget
    void Render() override {
      //draw text centered
      gfx->SetFont(speedFont);
      DrawText(std::to_string(speed),  (width - textBounds.w) / 2,  (height - textBounds.h) / 2, Color(255, 255, 255)); 
    }


  protected:
    font* speedFont;

  private:
    drz::rect textBounds;

};