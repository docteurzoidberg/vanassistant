#pragma once

#include <DrzGraphics.h>

#include "../../../Widget.h"
#include "DrzInputs.h"

using namespace drz;

//callback type for menu items. return void and take no arguments
typedef void (*MenuItemCallback)(void);

class MenuItemWidget : public Widget {

  public:

    MenuItemWidget(int screenX, int screenY, int width, int heigth, std::string fontname, std::string text, inputs::Key key, MenuItemCallback callback) : Widget(screenX, screenY, width, heigth) {    
      this->fontname = fontname;
      this->text = text;
      this->key = key;
      this->callback = callback;
    }

    void Load() override {
      //TODO
      
    }

    void Update(float elapsedTime) override {
      //TODO
      gfx->SetFont(fontname);
      textBounds = gfx->GetTextBounds(text, 0, 0);
    }

    void Render() override {
      gfx->SetFont(fontname);

      //std::cout << "width: " << width << " textBounds.w: " << textBounds.w << " textBounds.h: " << textBounds.h << std::endl;
      DrawText(text,  (width-textBounds.w)/2,  height / 2 + textBounds.h/2, Color(255, 255, 255)); 

      //draw border
      DrawLine(0, 0, width, 0, Color(255, 255, 255));
      //DrawLine(0, 0, 0, height, drz::color(255, 255, 255));
      DrawLine(width, 0, width, height, Color(255, 255, 255));
      DrawLine(0, height, width, height, Color(255, 255, 255));
    }

    inputs::Key GetKey() {
      return key;
    }

    MenuItemCallback GetCallback() {
      return callback;
    }

  private:
    std::string fontname;
    std::string text;
    inputs::Key key;
    font* menuItemFont;
    rect textBounds;
    MenuItemCallback callback;
};