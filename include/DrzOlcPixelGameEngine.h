#pragma once

#include "olcPixelGameEngine.h"
#include "olcPGEX_Font.h"

#include "IDrzEngine.h"
#include <cstdint>

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

  void Clear(color color) override {
    pge->Clear(ColorToPixel(color));
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
    if(!currentFont) return;
    write(text);
    /*
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
                pge->Draw(cursor_x + glyph->xOffset + xx, cursor_y + glyph->yOffset + yy, ColorToPixel(color));
              }
            }
          }
          cursor_x += glyph->xAdvance;
        }
      }
    }
    */
  }

  void SetCursorPos(uint16_t x, uint16_t y) override {
    cursorX = x;
    cursorY = y;
  }

  olc::Pixel ColorToPixel(color color) {
    return olc::Pixel(color.r, color.g, color.b, color.a);
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

  uint16_t cursorX=0, cursorY=0;
  bool wrap = false;
  color textcolor = WHITE;
  color textbgcolor = BLACK;


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
