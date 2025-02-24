#pragma once

#include <DrzGraphics.h>
#include <string>

#include "../../../Widget.h"


using namespace drz;

class SpeedometerWidget : public Widget {
  public:

  SpeedometerWidget(int screenX, int screenY, int width, int heigth) : Widget(screenX, screenY, width, heigth) {  
    gfx = DrzGraphics::Get();
  }

  void Load() override {
    //TODO
  }

  void Update(float elapsedTime) override {
    speedInt = (int) speed;
    if(speedInt<0) speedInt = 0;
    speedStr = std::to_string(speedInt);
    if(speedInt<10) speedStr = "0" + speedStr;
  }

  void Render() override {
    //debug
    //gfx->FillRect(screenX, screenY, width, height, Color(0,255,255));

    //Draw speed;
    gfx->SetFont("sevenseg30");

    auto textBounds = gfx->GetTextBounds(speedStr, 0, 0);

    if(speedStr.length()>2) {
      gfx->DrawText(speedStr,screenX -19, screenY+60,WHITE);
    }
    else {
      gfx->DrawText(speedStr,screenX + 22,screenY+60,WHITE);
    }
  }

  void SetSpeed(float speed) {
    this->speed = speed;
  }

  private:
    IDrzGraphics* gfx;
    float speed = 0.0f;
    std::string speedStr = "";
    int speedInt = 0;

};