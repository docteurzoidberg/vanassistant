#pragma once

#include <IDrzEngine.h>

#include <olcPixelGameEngine.h>

#ifdef DrawText
#undef DrawText
#endif

#include <chrono>
#include <cstdint>
#include <string>

namespace drz
{

class Drz_PGE_Engine : public olc::PixelGameEngine, public IDrzEngine {

public:
  Drz_PGE_Engine(olc::PixelGameEngine* p);

  long Now() override;

  /* Get */

  float GetRandomFloat() override;
  int GetScreenWidth() override;
  int GetScreenHeight() override;
  uint32_t GetFPS() override;
  /* Set */

  void SetFont(const std::string& fontName) override;

  void SetFont(const font* f) override;

  void SetPaintMode(color::Mode mode) override;

  void SetCursorPos(int x, int y) override;

  void SetTextColor(color color) override;

  void SetWrap(bool w) override;

  /* Drawing methods */

  void Clear(color color) override;

  bool DrawPixel(int x, int y, color color) override;
/*
  void DrawLine(vec2d p1, vec2d p2, color color) override {
    pge->DrawLine(p1.x, p1.y, p2.x, p2.y, ColorToPixel(color));
  }
*/
  void DrawLine(int x1, int y1, int x2, int y2, color color) override;

  void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, color color) override;

  void DrawRect(int x, int y, int w, int h, color color) override;

  void FillRect(int x, int y, int w, int h, color color) override;
/*
  void FillTriangle(vec2d p1, vec2d p2, vec2d p3, color color) override {
    pge->FillTriangle((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, (int)p3.x, (int)p3.y, ColorToPixel(color));
  }
*/
  void FillTriangle( int x1, int y1, int x2, int y2, int x3, int y3, color col) override;
  void FillCircle(int x, int y, int radius, color color) override;
  

  /* Text */

  const font* LoadFont(const std::string& fontName, const font* font) override;

  void DrawText(const std::string& text, float x, float y, color color) override;

  rect GetTextBounds(const std::string& text, int x, int y) override;

  void DrawPartialSprite(vi2d pos, Sprite* sprite, vi2d sourcePos, vi2d size) override;
  void DrawSprite(drz::Sprite* sprite, int x, int y) override;
  void DrawMaskSprite(drz::Sprite* sprite, int x, int y) override;

  // Function to create olc::Sprite from raw data
  drz::Sprite* CreateSpriteFromData(const unsigned int* data, int width, int height) override;

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
  

  olc::Pixel ColorToPixel(color color);
  olc::Pixel::Mode ModeToPixelMode(color::Mode mode);

  void DrawChar(uint16_t x, uint16_t y, unsigned char c, color fg, color bg);

  size_t write(unsigned char c);
  size_t write(const char *buffer, size_t size);
  size_t write(const std::string &s);

  /**************************************************************************/
  /*!
      @brief  Helper to determine size of a character with current font/size.
              Broke this out as it's used by both the PROGMEM- and RAM-resident
              getTextBounds() functions.
      @param  c     The ASCII character in question
      @param  x     Pointer to x location of character. Value is modified by
                    this function to advance to next character.
      @param  y     Pointer to y location of character. Value is modified by
                    this function to advance to next character.
      @param  minx  Pointer to minimum X coordinate, passed in to AND returned
                    by this function -- this is used to incrementally build a
                    bounding rectangle for a string.
      @param  miny  Pointer to minimum Y coord, passed in AND returned.
      @param  maxx  Pointer to maximum X coord, passed in AND returned.
      @param  maxy  Pointer to maximum Y coord, passed in AND returned.
  */
  /**************************************************************************/
  void _charBounds(unsigned char c, int16_t *x, int16_t *y, int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy);
  
  void _getTextBounds(const char *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);

  /**************************************************************************/
  /*!
      @brief    Helper to determine size of a string with current font/size. Pass
    string and a cursor position, returns UL corner and W,H.
      @param    str    The ascii string to measure (as an arduino String() class)
      @param    x      The current cursor X
      @param    y      The current cursor Y
      @param    x1     The boundary X coordinate, set by function
      @param    y1     The boundary Y coordinate, set by function
      @param    w      The boundary width, set by function
      @param    h      The boundary height, set by function
  */
  /**************************************************************************/
  void _getTextBounds(const std::string &str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);

};

} // namespace drz