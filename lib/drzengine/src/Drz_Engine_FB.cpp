#include <Drz_Engine_FB.h>

namespace drz 
{
  Drz_FB_Engine::Drz_FB_Engine() { 
    startedAt = std::chrono::system_clock::now();
    DrzEngine::Set(this);
  }

  void Drz_FB_Engine::SetFBG(struct _fbg *fbg) {
    this->fbg = fbg;
    std::cout << "SetFBG " << fbg << std::endl;
  }

  long Drz_FB_Engine::Now()  {
    auto elapsed = std::chrono::system_clock::now() - startedAt;
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
    return millis.count();
  }

  /* Get */

  float Drz_FB_Engine::GetRandomFloat()  {
    //return random float between 0 and 1
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
  }

  int Drz_FB_Engine::GetScreenWidth()  {
    //std::cout << "gsw: fbg " << fbg << std::endl;
    return fbg->width;
  }

  int Drz_FB_Engine::GetScreenHeight()  {
    return fbg->height;
  }

  uint32_t Drz_FB_Engine::GetFPS()  {
    return fbg_getFramerate(fbg, 0);
  }

  hwbutton Drz_FB_Engine::GetKey(Key key) {
    //TODO

    auto drzInputs = DrzInputs::Get();

    if(drzInputs!=nullptr) {
      return drzInputs->GetKey(key);
    }
    else {
      std::cout << "DrzInputs not set" << std::endl;
      exit(1);
    }

    return { 
      .bPressed = false,
      .bReleased = false,
      .bHeld = false
    };
  }

  /* Set */
  void Drz_FB_Engine::SetFont(const std::string& fontName) {
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

  void Drz_FB_Engine::SetFont(const font* f) {
    currentFont = f;
  }

  void Drz_FB_Engine::SetCursorPos(uint16_t x, uint16_t y) {
    cursorX = x;
    cursorY = y;
  }

  void Drz_FB_Engine::SetTextColor(color color) {
    textcolor = color;
  }

  void Drz_FB_Engine::SetWrap(bool w) {
    wrap = w;
  }

  /* Drawing methods */

  void Drz_FB_Engine::Clear(color color) {
    fbg_background(fbg, color.r, color.g, color.b);
  }

  bool Drz_FB_Engine::DrawPixel(int x, int y, color color) {
    fbg_pixel(fbg, x, y, color.r, color.g, color.b);
    return true;
  }
/*
  void DrawLine(vec2d p1, vec2d p2, color color) override {
    pge->DrawLine(p1.x, p1.y, p2.x, p2.y, ColorToPixel(color));
  }
*/
  void Drz_FB_Engine::DrawLine(int x1, int y1, int x2, int y2, color color) {
    fbg_line(fbg, x1, y1, x2, y2, color.r, color.g, color.b);
  }

  void Drz_FB_Engine::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, color color) {
    //TODO
    DrawLine(x1, y1, x2, y2, color);
    DrawLine(x2, y2, x3, y3, color);
    DrawLine(x3, y3, x1, y1, color);
  }

  void Drz_FB_Engine::FillRect(int x, int y, int w, int h, color color) {
    
    //std::cout << "fillrect " << x << " " << y << " " << w << " " << h << std::endl;


    if(x<0) {
      w += x;
      x = 0;
    }

    if(y<0) {
      h += y;
      y = 0;
    }

    if(x+w > GetScreenWidth()) {
      w = GetScreenWidth() - x;
    }

    if(y+h > GetScreenHeight()) {
      h = GetScreenHeight() - y;
    }

    fbg_rect(fbg, x, y, w, h, color.r, color.g, color.b);
  }

  void Drz_FB_Engine::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, color col) {
    
    auto drawLine = [this, &col](int sx, int ex, int ny) {
      if (sx > ex) std::swap(sx, ex);
      for (int i = sx; i <= ex; i++) {
        DrawPixel(i, ny, col);
      }
    };

    auto sortVertices = [](int& x1, int& y1, int& x2, int& y2) {
      if (y1 > y2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
      }
    };

    // Sort vertices by y-coordinate
    sortVertices(x1, y1, x2, y2);
    sortVertices(x1, y1, x3, y3);
    sortVertices(x2, y2, x3, y3);

    int total_height = y3 - y1;
    for (int i = 0; i < total_height; i++) {
      bool second_half = i > y2 - y1 || y2 == y1;
      int segment_height = second_half ? y3 - y2 : y2 - y1;
      float alpha = (float)i / total_height;
      float beta = (float)(i - (second_half ? y2 - y1 : 0)) / segment_height; // be careful: with above conditions no division by zero here
      int ax = x1 + (x3 - x1) * alpha;
      int bx = second_half ? x2 + (x3 - x2) * beta : x1 + (x2 - x1) * beta;

      if (ax > bx) std::swap(ax, bx);
      drawLine(ax, bx, y1 + i);
    }
  }


  void Drz_FB_Engine::FillCircle(int x, int y, int radius, color color) {
    //TODO
    
  }

  /* Text */

  const font* Drz_FB_Engine::LoadFont(const std::string& fontName, const font* font) {
    fonts[fontName] = font;
    return font;
  }

  void Drz_FB_Engine::DrawText(const std::string& text, float x, float y, color color) {
    if(!currentFont) {
      std::cout << "No font set" << std::endl;
      return;
    } 
    SetCursorPos((uint16_t) x, (uint16_t) y);
    SetTextColor(color);
    write(text);
  }

  rect Drz_FB_Engine::GetTextBounds(const std::string& text, int x, int y) {
    if(!currentFont) {
      std::cout << "No font set" << std::endl;
      return {0, 0, 0, 0};
    } 
    //call internal method
    int16_t x1, y1;
    uint16_t w1, h1;
    _getTextBounds(text, x, y, &x1, &y1, &w1, &h1);
    return {x1, y1, w1, h1};
  }

  void Drz_FB_Engine::DrawChar(uint16_t x, uint16_t y, unsigned char c, color fg, color bg) {
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
          DrawPixel(x + xo + xx, y + yo + yy, fg);
        }
        bits <<= 1;
      }
    }
  }

  size_t Drz_FB_Engine::write(unsigned char c) {
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

  size_t Drz_FB_Engine::write(const char *buffer, size_t size) {
    size_t n = 0;
    while (size--) {
      if (write(*buffer++)) n++;
      else break;
    }
    return n;
  }

  size_t Drz_FB_Engine::write(const std::string &s) {
    return write(s.c_str(), s.length());
  }

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
  void Drz_FB_Engine::_charBounds(unsigned char c, int16_t *x, int16_t *y, int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy) {

    if (!currentFont) {
      return;
    }

    if (c == '\n') { // Newline?
      *x = 0;        // Reset x to zero, advance y by one line
      *y += currentFont->yAdvance;
    } else if (c != '\r') { // Not a carriage return; is normal char
      uint8_t first = currentFont->first,
              last = currentFont->last;
      if ((c >= first) && (c <= last)) { // Char present in this font?
        fontglyph *glyph = (fontglyph*) (currentFont->glyph +  (c - first));
        
        uint8_t gw = glyph->width,
                gh = glyph->height,
                xa = glyph->xAdvance;
        int8_t xo = glyph->xOffset,
                yo = glyph->yOffset;
        if (wrap && ((*x + (((int16_t)xo + gw) * 1)) > Drz_FB_Engine::GetScreenWidth())) {

          *x = 0; // Reset x to zero, advance y by one line
          *y += currentFont->yAdvance;
        }
        int16_t
                x1 = *x + xo * 1, y1 = *y + yo * 1, x2 = x1 + gw * 1 - 1,
                y2 = y1 + gh * 1 - 1;
        if (x1 < *minx)
          *minx = x1;
        if (y1 < *miny)
          *miny = y1;
        if (x2 > *maxx)
          *maxx = x2;
        if (y2 > *maxy)
          *maxy = y2;
        *x += xa * 1;
      }
    }
  }
  
  void Drz_FB_Engine::_getTextBounds(const char *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {

    uint8_t c; // Current character
    int16_t minx = 0x7FFF, miny = 0x7FFF, maxx = -1, maxy = -1; // Bound rect
    // Bound rect is intentionally initialized inverted, so 1st char sets it

    *x1 = x; // Initial position is value passed in
    *y1 = y;
    *w = *h = 0; // Initial size is zero

    while ((c = *str++)) {
      // charBounds() modifies x/y to advance for each character,
      // and min/max x/y are updated to incrementally build bounding rect.
      Drz_FB_Engine::_charBounds(c, &x, &y, &minx, &miny, &maxx, &maxy);
    }

    if (maxx >= minx) {     // If legit string bounds were found...
      *x1 = minx;           // Update x1 to least X coord,
      *w = maxx - minx + 1; // And w to bound rect width
    }
    if (maxy >= miny) { // Same for height
      *y1 = miny;
      *h = maxy - miny + 1;
    }
  }

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
  void Drz_FB_Engine::_getTextBounds(const std::string &str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
    if (str.length() != 0) {
      _getTextBounds(const_cast<char *>(str.c_str()), x, y, x1, y1, w, h);
    }
  }



} // namespace drz