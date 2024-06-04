#pragma once

#include "olcPixelGameEngine.h"
#include "olcPGEX_Font.h"

#include "IDrzEngine.h"

class DrzOlcPixelGameEngine : public olc::PixelGameEngine, public IDrzEngine {

public:
  DrzOlcPixelGameEngine(olc::PixelGameEngine* p) : pge(p) {}


  uint32_t Now() override {
    //return time in milliseconds
    return static_cast <uint32_t> (GetElapsedTime() * 1000);
  }
  
  float GetRandomFloat() override {
    //return random float between 0 and 1
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
  }

  int GetScreenWidth() override {
    return ScreenWidth();
  }
  int GetScreenHeight() override {
    return ScreenHeight();
  }

  void DrawLine(float x1, float y1, float x2, float y2, color color) override {
    pge->DrawLine(x1, y1, x2, y2, olc::Pixel(color));
  }

  void DrawTriangle(const vec3d& p1, const vec3d& p2, const vec3d& p3, color color) override {
    pge->DrawTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, olc::Pixel(color));
  }

  void FillRect(int x, int y, int w, int h, color color) override {
    pge->FillRect(x, y, w, h, olc::Pixel((uint32_t)color));
  }

  void Clear(color color) override {
    pge->Clear(olc::Pixel((uint32_t)color));
  }

  void LoadFont(const std::string& fontName, const font* font) override {
    fonts[fontName] = font;
  }

  void SetFont(const std::string& fontName) override {
    if (fonts.find(fontName) != fonts.end()) {
      currentFont = fonts[fontName];
    }
  }

  void DrawText(const std::string& text, float x, float y, color color) override {
    if (currentFont) {
      float cursor_x = x;
      float cursor_y = y;
      for (char c : text) {
        if (c >= currentFont->first && c <= currentFont->last) {
          fontglyph* glyph = currentFont->glyph + (c - currentFont->first);
          uint8_t* bitmap = currentFont->bitmap + glyph->bitmapOffset;
          for (uint8_t yy = 0; yy < glyph->height; yy++) {
            for (uint8_t xx = 0; xx < glyph->width; xx++) {
              if (bitmap[yy * ((glyph->width + 7) / 8) + xx / 8] & (1 << (xx % 8))) {
                pge->Draw(cursor_x + glyph->xOffset + xx, cursor_y + glyph->yOffset + yy, olc::Pixel(color));
              }
            }
          }
          cursor_x += glyph->xAdvance;
        }
      }
    }
  }

  void ReadKeys() override {
    //TODO
  }

  hwbutton GetKey(int key) override {
    olc::HWButton olcbutton = pge->GetKey(static_cast<olc::Key>(key));
    return { 
      olcbutton.bPressed, 
      olcbutton.bReleased, 
      olcbutton.bHeld 
    };
  }

private:
  olc::PixelGameEngine* pge;
  std::map<std::string, const font*> fonts;
  const font* currentFont = nullptr;
};
