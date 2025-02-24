#pragma once

#include <DrzGraphics.h>

#include "../../../Widget.h"

using namespace drz;

class IconWidget : public Widget {
  public:

    IconWidget(int x, int y, int w, int h, bool visible) : visible(visible), Widget(x, y, w, h) {
    }

    void SetIcon(Sprite* icon) {
      this->icon = icon;
    }

    void Hide() {
      visible = false;
    }

    void Show() {
      visible = true;
    }

    void Load() override {
    }

    void Update(float fElapsedTime) override {
    }

    void Render() override { 

      if(!visible) {
        return;
      }

      if(icon!=nullptr) {
        gfx->DrawSprite(screenX, screenY, icon);
      }

      //draw rectangle around the icon
      gfx->DrawRect(screenX, screenY, width, height, WHITE);
    }

  private:
    Sprite* icon = nullptr;
    bool visible = true;
};