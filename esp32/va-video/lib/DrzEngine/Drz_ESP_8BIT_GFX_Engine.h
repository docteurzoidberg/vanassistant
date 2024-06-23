#pragma once

#include <Arduino.h>
#include <ESP_8_BIT_GFX.h>
#include "IDrzEngine.h"
#include <chrono>
#include <cstdint>
#include <map>

class DrzGameEngine : public IDrzEngine {

public:
  DrzGameEngine(ESP_8_BIT_GFX* p) : gfx(p) { 
    startedAt = millis();
  }

  long Now() override {
    return millis();
  }
   
  /* Get */
  float GetRandomFloat() override {
    //return random float between 0 and 1
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
  }

  int GetScreenWidth() override {
    return gfx->width();
  }

  int GetScreenHeight() override {
    return gfx->height();
  }

  uint32_t GetFPS() override {
    //TODO
    return 0;
    //return gfx->GetFPS();
  }

  /* Set */

  void SetFont(const std::string& fontName) override {
    auto newfont = fonts[fontName];
    if (newfont) {          // Font struct pointer passed in?
      if (!currentFont) { // And no current font struct?
        // Switching from classic to new font behavior.
        // Move cursor pos down 6 pixels so it's on baseline.
        //cursorY += 6;
      }
    } else if (currentFont) { // NULL passed.  Current font struct defined?
      // Switching from new to classic font behavior.
      // Move cursor pos up 6 pixels so it's at top-left of char.
      //cursorY -= 6;
    }
    currentFont = newfont;
  }

  void SetFont(const font* f) override {
    currentFont = f;
  }

  void SetCursorPos(uint16_t x, uint16_t y) override {
    gfx->setCursor(x, y);
  }

  void SetTextColor(color color) override {
    textcolor = color;
    gfx->setTextColor(color.toRGB332());
  }

  void SetWrap(bool w) override {
    wrap = w;
  }

  /* Drawing methods */
  void Clear(color color) override {
    gfx->fillScreen(color.toRGB332());
  }

  void DrawPixel(int x, int y, color color) override {
    gfx->drawPixel(x, y, color.toRGB332());
  }

  void DrawLine(vec2d p1, vec2d p2, color color) override {
    gfx->drawLine(p1.x, p1.y, p2.x, p2.y, color.toRGB332());
  }

  void DrawLine(int x1, int y1, int x2, int y2, color color) override {
    gfx->drawLine(x1, y1, x2, y2, color.toRGB332());
  }

  void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, color color) override {
    gfx->drawTriangle(x1, y1, x2, y2, x3, y3, color.toRGB332());
  }

  void FillRect(int x, int y, int w, int h, color color) override {
    gfx->fillRect(x, y, w, h, color.toRGB332());
  }

  void FillTriangle(vec2d p1, vec2d p2, vec2d p3, color color) override {
    gfx->fillTriangle((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, (int)p3.x, (int)p3.y, color.toRGB332());
  }

  void FillCircle(int x, int y, int radius, color color) override {
    gfx->fillCircle(x, y, radius,color.toRGB332());
  }

  /* Text */

  const GFXFont* LoadFont(const std::string& fontName, const GFXFont* font) override {
    fonts[fontName] = font;
    return font;
  }

  void DrawText(const std::string& text, float x, float y, color color) override {
    if(!currentFont) {
      std::cout << "No font set" << std::endl;
      return;
    } 
    gfx->setCursor((uint16_t) x, (uint16_t) y);
    gfx->setTextColor(color.toRGB332());
    //TODO
  }

  rect GetTextBounds(const std::string& text, int x, int y) override {
    if(!currentFont) {
      std::cout << "No font set" << std::endl;
    } 
    return {0, 0, 0, 0};
    //TODO
  }

private:
  ESP_8_BIT_GFX* gfx;
  std::map<std::string, const GFXFont*> fonts;
  const GFXFont* currentFont = nullptr;
  uint16_t cursorX=0;
  uint16_t cursorY=0;
  bool wrap = false;
  color textcolor = WHITE;
  color textbgcolor = BLACK;
  long startedAt = 0;
};
