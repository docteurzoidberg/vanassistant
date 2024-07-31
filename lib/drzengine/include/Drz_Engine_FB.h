#pragma once

#include <IDrzEngine.h>
#include <IDrzInputs.h>

#include <chrono>
#include <cstdint>
#include <map>
#include <iostream>
#include <csignal>

#include <fb/fbgraphics.h>
#include <fb/fbg_fbdev.h>

namespace drz 
{

class Drz_FB_Engine : public IDrzEngine {

public:

  Drz_FB_Engine();

  void SetFBG(struct _fbg *fbg);

  long Now() override = 0;

  /* Get */

  float GetRandomFloat() override = 0;
  int GetScreenWidth() override = 0;
  int GetScreenHeight() override = 0;
  uint32_t GetFPS() override = 0;
  hwbutton GetKey(Key key) override = 0;

  /* Set */
  void SetFont(const std::string& fontName) override = 0;
  void SetFont(const font* f) override = 0;
  void SetCursorPos(uint16_t x, uint16_t y) override  = 0;
  void SetTextColor(color color) override  = 0;
  void SetWrap(bool w) override  = 0;

  /* Drawing methods */

  void Clear(color color) override = 0;
  bool DrawPixel(int x, int y, color color) override = 0;
  void DrawLine(int x1, int y1, int x2, int y2, color color) override = 0;
  void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, color color) override = 0;
  void FillRect(int x, int y, int w, int h, color color) override = 0;
  void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, color col) override = 0;
  void FillCircle(int x, int y, int radius, color color) override = 0;
  
  /* Text */

  const font* LoadFont(const std::string& fontName, const font* font) override = 0;
  void DrawText(const std::string& text, float x, float y, color color) override = 0;
  rect GetTextBounds(const std::string& text, int x, int y) override = 0;

private:
  struct _fbg *fbg;
  std::map<std::string, const font*> fonts;
  const font* currentFont = nullptr;
  uint16_t cursorX=0;
  uint16_t cursorY=0;
  bool wrap = false;
  color textcolor = WHITE;
  color textbgcolor = BLACK;

  std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long, std::ratio<1, 1000000000>>> startedAt;

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