#pragma once

#include <IDrzEngine.h>
#include <string>

#include "../../Widget.h"

class WidgetMenuItem : public Widget {
  public:

    std::string text = "";

    WidgetMenuItem(int screenX, int screenY, int width, int height, drz::font* font) : Widget(screenX, screenY, width, height) {
      speedFont = font;
    }

    void SetText(std::string text) {
      this->text = text;
    } 
    
    // Load the widget
    void Load() override {
      //font loaded by caller
    }
    
    // Update the widget 
    void Update(float fElapsedTime) override {  
      //calc text bounds
      engine->SetFont(speedFont);
      textBounds = engine->GetTextBounds(text, 0, 0);
    }
    
    // Render the widget
    void Render() override {
      //draw text centered
      engine->SetFont(speedFont);
      DrawText(text,  (width - textBounds.w) / 2,  (height - textBounds.h) / 2, drz::color(255, 255, 255)); 

      //draw border
      DrawLine(0, 0, width, 0, drz::color(255, 255, 255));
      //DrawLine(0, 0, 0, height, drz::color(255, 255, 255));
      DrawLine(width, 0, width, height, drz::color(255, 255, 255));
      DrawLine(0, height, width, height, drz::color(255, 255, 255));
    }


  protected:
    drz::font* speedFont;

  private:
    drz::rect textBounds;

};