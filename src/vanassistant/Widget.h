#pragma once

#include <DrzGraphics.h>

namespace drz {
  
  using namespace graphics;

  class Widget {
    public:
      int width=0;
      int height=0;
      int screenX=0;
      int screenY=0;

      bool isVisible = true;

      Widget(int screenX, int screenY, int width, int height);

      void SetPosition(int x, int y);

      void SetSize(int width, int height);

      virtual void Load() = 0;
      virtual void Update(float fElapsedTime) = 0;
      virtual void Render() = 0;

      //Draw pixel relative to the widget position
      void DrawPixel(int px, int py, Color p);

      //Draw text relative to the widget position
      void DrawText(std::string text, int px, int py, Color c);
      //Draw line relative to the widget position
      void DrawLine(int x1, int y1, int x2, int y2, Color c);

      //Draw rectangle relative to the widget position
      void FillRect(int x, int y, int w, int h, Color c);

      //Clear the widget
      void Clear(Color c);
      void Clear();

    protected:
      IDrzGraphics* gfx;
  };
} //namespace drz