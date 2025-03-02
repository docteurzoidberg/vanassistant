#pragma once

#include <DrzGraphics.h>

#include "../../../Widget.h"

#include <string>

using namespace drz;

class WidgetMenuItem : public Widget {
  public:

    std::string text = "";

    WidgetMenuItem(int screenX, int screenY, int width, int height, font* font) : Widget(screenX, screenY, width, height) {
      menuItemFont = font;
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
      gfx->SetFont(menuItemFont);
      textBounds = gfx->GetTextBounds(text, 0, 0);
    }
    
    // Render the widget
    void Render() override {
      //draw text centered
      gfx->SetFont(menuItemFont);
      DrawText(text,  (width - textBounds.w) / 2,  (height - textBounds.h) / 2, Color(255, 255, 255)); 

      //draw border
      DrawLine(0, 0, width, 0, Color(255, 255, 255));
      //DrawLine(0, 0, 0, height, drz::color(255, 255, 255));
      DrawLine(width, 0, width, height, Color(255, 255, 255));
      DrawLine(0, height, width, height, Color(255, 255, 255));
    }


  protected:
    drz::font* menuItemFont;

  private:
    drz::rect textBounds;

};