#pragma once

#include "IDrzEngine.h"

class IWidget {
  public:
    int width=0;
    int height=0;
    int screenX=0;
    int screenY=0;

    bool isVisible = true;

    IWidget(int screenX, int screenY, int width, int height) {
      this->screenX = screenX;
      this->screenY = screenY;
      this->width = width;
      this->height = height;
      this->engine = drz::DisplayPageManager::GetEngine();
    }

    void SetPosition(int x, int y) {
      this->screenX = x;
      this->screenY = y;
    }

    void SetSize(int width, int height) {
      this->width = width;
      this->height = height;
    }

    virtual void Load() = 0;

    virtual void Update(float fElapsedTime) = 0;

    virtual void Render() = 0;

    //Draw pixel relative to the widget position
    void DrawPixel(int px, int py, drz::color p) {
      if(engine == nullptr) return;
      int x = px + screenX;
      int y = py + screenY; 
      // Check if the pixel is out of the widget bounds
      if(x < 0 || x >= width) return;
      if(y < 0 || y >= height) return;
      // Check if the pixel is out of the screen bounds
      if(x >= engine->GetScreenWidth()) return;
      if(y >= engine->GetScreenHeight()) return;

      engine->DrawPixel(x, y, p);
    }

    //Draw text relative to the widget position
    void DrawText(std::string text, int px, int py, drz::color c) {
      if(engine == nullptr) return;
      int x = px + screenX;
      int y = py + screenY; 
      engine->DrawText(text, x, y, c);
    }

    //Draw line relative to the widget position
    void DrawLine(int x1, int y1, int x2, int y2, drz::color c) {
      if(engine == nullptr) return;
      int x = x1 + screenX;
      int y = y1 + screenY;
      int x_ = x2 + screenX;
      int y_ = y2 + screenY;
      engine->DrawLine(x, y, x_, y_, c);
    }

    //Draw rectangle relative to the widget position
    void FillRect(int x, int y, int w, int h, drz::color c) {
      if(engine == nullptr) return;
      int x_ = x + screenX;
      int y_ = y + screenY;
      engine->FillRect(x_, y_, w, h, c);
    }

  protected:
    drz::IDrzEngine* engine;


};