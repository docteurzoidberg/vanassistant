#pragma once

#include <DrzGraphics.h>

#include "../../../Widget.h"
#include "DrzInputs.h"

using namespace drz;

class MenuItemWidget : public Widget {

  public:

    MenuItemWidget(int screenX, int screenY, int width, int heigth, std::string fontname, std::string text, inputs::Key key) : Widget(screenX, screenY, width, heigth) {    
      this->fontname = fontname;
      this->text = text;
      this->key = key;
    }

    void Load() override {
      //TODO
    }

    void Update(float elapsedTime) override {
      //TODO
    }

    void Render() override {
      gfx->SetFont(fontname);
      //TODO: draw text
    }

    inputs::Key GetKey() {
      return key;
    }

  private:
    //TODO
    std::string fontname;
    std::string text;
    inputs::Key key;
};