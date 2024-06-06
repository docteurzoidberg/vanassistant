#pragma once

#include "olcPixelGameEngine.h"
#include "olcPGEX_Font.h"

#include "IDrzEngine.h"
#include <chrono>
#include <cstdint>

class DrzOlcPixelGameEngine : public olc::PixelGameEngine, public IDrzEngine {

public:
  DrzOlcPixelGameEngine(olc::PixelGameEngine* p) : pge(p) { 
    startedAt = std::chrono::system_clock::now();
  }

  long Now() override {
    auto elapsed = std::chrono::system_clock::now() - startedAt;
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
    return millis.count();
  }
   
  /* Get */

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

  uint32_t GetFPS() override {
    return pge->GetFPS();
  }

  hwbutton GetKey(uint8_t key) override {
    olc::HWButton olcbutton = pge->GetKey(static_cast<olc::Key>(key));
    return { 
      olcbutton.bPressed, 
      olcbutton.bReleased, 
      olcbutton.bHeld 
    };
  }

  /* Set */

  void SetFont(const std::string& fontName) override {
    if (fonts.find(fontName) != fonts.end()) {
      currentFont = fonts[fontName];
    }
  }

  void SetFont(const font* f) override {
    currentFont = f;
  }

  void SetCursorPos(uint16_t x, uint16_t y) override {
    cursorX = x;
    cursorY = y;
  }

  void SetWrap(bool w) override {
    wrap = w;
  }

  /* Drawing methods */

  void Clear(color color) override {
    pge->Clear(ColorToPixel(color));
  }

  void DrawPixel(int x, int y, color color) override {
    pge->Draw(x, y, ColorToPixel(color));
  }

  void DrawLine(int x1, int y1, int x2, int y2, color color) override {
    pge->DrawLine(x1, y1, x2, y2, ColorToPixel(color));
  }

  void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, color color) override {
    pge->DrawTriangle(x1, y1, x2, y2, x3, y3, ColorToPixel(color));
    //pge->DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, ColorToPixel(color));
    //.pge->DrawLine((int)p2.x, (int)p2.y, (int)p3.x, (int)p3.y, ColorToPixel(color));
    //pge->DrawLine((int)p3.x, (int)p3.y, (int)p1.x, (int)p1.y, ColorToPixel(color));
  }

  void FillRect(int x, int y, int w, int h, color color) override {
    pge->FillRect(x, y, w, h, ColorToPixel(color));
  }

  void FillTriangle(vec2d p1, vec2d p2, vec2d p3, color color) override {
    pge->FillTriangle((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, (int)p3.x, (int)p3.y, ColorToPixel(color));
  }

  void FillCircle(int x, int y, int radius, color color) override {
    pge->FillCircle(x, y, radius, ColorToPixel(color));
  }

  /* Text */

  const font* LoadFont(const std::string& fontName, const font* font) override {
    fonts[fontName] = font;
    return font;
  }

  void DrawText(const std::string& text, float x, float y, color color) override {
    if(!currentFont) return;
    write(text);
  }

private:
  olc::PixelGameEngine* pge;
  std::map<std::string, const font*> fonts;
  const font* currentFont = nullptr;
  uint16_t cursorX=0;
  uint16_t cursorY=0;
  bool wrap = false;
  color textcolor = WHITE;
  color textbgcolor = BLACK;

  std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long, std::ratio<1, 1000000000>>> startedAt;
  

  olc::Pixel ColorToPixel(color color) {
    return olc::Pixel(color.r, color.g, color.b, color.a);
  }

  void DrawChar(uint16_t x, uint16_t y, unsigned char c, color fg, color bg) {
    c -= (unsigned char) currentFont->first;
    fontglyph *glyph = currentFont->glyph + c;
    uint8_t *bitmap = currentFont->bitmap;
    uint16_t bo = glyph->bitmapOffset;
    uint8_t w = glyph->width;
    uint8_t h = glyph->height;
    int8_t xo = glyph->xOffset;
    int8_t yo = glyph->yOffset;
    uint8_t xx, yy, bits = 0, bit = 0;
    int16_t xo16 = 0, yo16 = 0;
    for (yy = 0; yy < h; yy++) {
      for (xx = 0; xx < w; xx++) {
        if (!(bit++ & 7)) {
          bits = bitmap[bo++];
        }
        if (bits & 0x80) {
          pge->Draw(x + xo + xx, y + yo + yy, ColorToPixel(fg));
        }
        bits <<= 1;
      }
    }
  }

  size_t write(unsigned char c) {
    if (c == '\n') {
      cursorX = 0;
      cursorY += currentFont->yAdvance;
    } else if (c != '\r') {
      uint8_t first = currentFont->first;
      if ((c >= first) && (c <= currentFont->last)) {
        fontglyph *glyph = (fontglyph*) (currentFont->glyph + c - first);
        uint8_t w = glyph->width;
        uint8_t h = glyph->height;
        if ((w > 0) && (h > 0)) { // Is there an associated bitmap?
          int16_t xo = glyph->xOffset; // sic
          if (wrap && ((cursorX + xo + w) > GetScreenWidth())) {
            cursorX = 0;
            cursorY += currentFont->yAdvance;
          }
          DrawChar(cursorX, cursorY, c, textcolor, textbgcolor);
        }
        cursorX += glyph->xAdvance;
      }
    }
    return 1;
  }

  size_t write(const char *buffer, size_t size) {
    size_t n = 0;
    while (size--) {
      if (write(*buffer++)) n++;
      else break;
    }
    return n;
  }

  size_t write(const std::string &s) {
    return write(s.c_str(), s.length());
  }
};
