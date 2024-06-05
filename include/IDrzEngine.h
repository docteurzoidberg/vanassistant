#pragma once
 
#include <cstdint>
#include <string>

struct vec2d {
  float x, y;
};

struct vec3d {
  float x, y, z;
};

struct face {
  int f[3];
};

struct triangle {
  vec3d p[3];
};

struct triangleref {
  vec3d* p[3];
};

struct trianglepointindex {
  int triIndex;
  int pointIndex;
};

struct mat4x4 {
  float m[4][4] = { 0 };
};

constexpr uint8_t  nDefaultAlpha = 0xFF;
constexpr uint32_t nDefaultColor = (nDefaultAlpha << 24);

struct color
{
  union
  {
    uint32_t n = nDefaultColor;
    struct { uint8_t r; uint8_t g; uint8_t b; uint8_t a; };
  };

  enum Mode { NORMAL, MASK, ALPHA, CUSTOM };

  color();
  color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = nDefaultAlpha);
  color(uint32_t p);
  color& operator = (const color& v) = default;
  bool   operator ==(const color& p) const;
  bool   operator !=(const color& p) const;
  color  operator * (const float i) const;
  color  operator / (const float i) const;
  color& operator *=(const float i);
  color& operator /=(const float i);
  color  operator + (const color& p) const;
  color  operator - (const color& p) const;
  color& operator +=(const color& p);
  color& operator -=(const color& p);
  color  operator * (const color& p) const;
  color& operator *=(const color& p);
  color  inv() const;
};

color ColorF(float red, float green, float blue, float alpha = 1.0f);
color ColorLerp(const color& p1, const color& p2, float t);

static const color
  GREY(192, 192, 192), DARK_GREY(128, 128, 128), VERY_DARK_GREY(64, 64, 64),
  RED(255, 0, 0), DARK_RED(128, 0, 0), VERY_DARK_RED(64, 0, 0),
  YELLOW(255, 255, 0), DARK_YELLOW(128, 128, 0), VERY_DARK_YELLOW(64, 64, 0),
  GREEN(0, 255, 0), DARK_GREEN(0, 128, 0), VERY_DARK_GREEN(0, 64, 0),
  CYAN(0, 255, 255), DARK_CYAN(0, 128, 128), VERY_DARK_CYAN(0, 64, 64),
  BLUE(0, 0, 255), DARK_BLUE(0, 0, 128), VERY_DARK_BLUE(0, 0, 64),
  MAGENTA(255, 0, 255), DARK_MAGENTA(128, 0, 128), VERY_DARK_MAGENTA(64, 0, 64),
  WHITE(255, 255, 255), BLACK(0, 0, 0), BLANK(0, 0, 0, 0);


  inline color::color()
  { r = 0; g = 0; b = 0; a = nDefaultAlpha; }

  inline color::color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
  { n = red | (green << 8) | (blue << 16) | (alpha << 24); } // Thanks jarekpelczar 

  inline color::color(uint32_t p)
  { n = p; }

  inline bool color::operator==(const color& p) const
  { return n == p.n; }

  inline bool color::operator!=(const color& p) const
  { return n != p.n; }

  inline color color::operator * (const float i) const
  {
    float fR = std::min(255.0f, std::max(0.0f, float(r) * i));
    float fG = std::min(255.0f, std::max(0.0f, float(g) * i));
    float fB = std::min(255.0f, std::max(0.0f, float(b) * i));
    return color(uint8_t(fR), uint8_t(fG), uint8_t(fB), a);
  }

  inline color color::operator / (const float i) const
  {
    float fR = std::min(255.0f, std::max(0.0f, float(r) / i));
    float fG = std::min(255.0f, std::max(0.0f, float(g) / i));
    float fB = std::min(255.0f, std::max(0.0f, float(b) / i));
    return color(uint8_t(fR), uint8_t(fG), uint8_t(fB), a);
  }

  inline color& color::operator *=(const float i)
  {
    this->r = uint8_t(std::min(255.0f, std::max(0.0f, float(r) * i)));
    this->g = uint8_t(std::min(255.0f, std::max(0.0f, float(g) * i)));
    this->b = uint8_t(std::min(255.0f, std::max(0.0f, float(b) * i)));
    return *this;
  }

  inline color& color::operator /=(const float i)
  {
    this->r = uint8_t(std::min(255.0f, std::max(0.0f, float(r) / i)));
    this->g = uint8_t(std::min(255.0f, std::max(0.0f, float(g) / i)));
    this->b = uint8_t(std::min(255.0f, std::max(0.0f, float(b) / i)));
    return *this;
  }

  inline color color::operator + (const color& p) const
  {
    uint8_t nR = uint8_t(std::min(255, std::max(0, int(r) + int(p.r))));
    uint8_t nG = uint8_t(std::min(255, std::max(0, int(g) + int(p.g))));
    uint8_t nB = uint8_t(std::min(255, std::max(0, int(b) + int(p.b))));
    return color(nR, nG, nB, a);
  }

  inline color color::operator - (const color& p) const
  {
    uint8_t nR = uint8_t(std::min(255, std::max(0, int(r) - int(p.r))));
    uint8_t nG = uint8_t(std::min(255, std::max(0, int(g) - int(p.g))));
    uint8_t nB = uint8_t(std::min(255, std::max(0, int(b) - int(p.b))));
    return color(nR, nG, nB, a);
  }

  inline color& color::operator += (const color& p)
  {
    this->r = uint8_t(std::min(255, std::max(0, int(r) + int(p.r))));
    this->g = uint8_t(std::min(255, std::max(0, int(g) + int(p.g))));
    this->b = uint8_t(std::min(255, std::max(0, int(b) + int(p.b))));
    return *this;
  }

  inline color& color::operator -= (const color& p) // Thanks Au Lit
  {
    this->r = uint8_t(std::min(255, std::max(0, int(r) - int(p.r))));
    this->g = uint8_t(std::min(255, std::max(0, int(g) - int(p.g))));
    this->b = uint8_t(std::min(255, std::max(0, int(b) - int(p.b))));
    return *this;
  }

  inline color color::operator * (const color& p) const
  {
    uint8_t nR = uint8_t(std::min(255.0f, std::max(0.0f, float(r) * float(p.r) / 255.0f)));
    uint8_t nG = uint8_t(std::min(255.0f, std::max(0.0f, float(g) * float(p.g) / 255.0f)));
    uint8_t nB = uint8_t(std::min(255.0f, std::max(0.0f, float(b) * float(p.b) / 255.0f)));
    uint8_t nA = uint8_t(std::min(255.0f, std::max(0.0f, float(a) * float(p.a) / 255.0f)));
    return color(nR, nG, nB, nA);
  }

  inline color& color::operator *=(const color& p)
  {
    this->r = uint8_t(std::min(255.0f, std::max(0.0f, float(r) * float(p.r) / 255.0f)));
    this->g = uint8_t(std::min(255.0f, std::max(0.0f, float(g) * float(p.g) / 255.0f)));
    this->b = uint8_t(std::min(255.0f, std::max(0.0f, float(b) * float(p.b) / 255.0f)));
    this->a = uint8_t(std::min(255.0f, std::max(0.0f, float(a) * float(p.a) / 255.0f)));
    return *this;
  }

  inline color color::inv() const
  {
    uint8_t nR = uint8_t(std::min(255, std::max(0, 255 - int(r))));
    uint8_t nG = uint8_t(std::min(255, std::max(0, 255 - int(g))));
    uint8_t nB = uint8_t(std::min(255, std::max(0, 255 - int(b))));
    return color(nR, nG, nB, a);
  }

  inline color ColorF(float red, float green, float blue, float alpha)
  { return color(uint8_t(red * 255.0f), uint8_t(green * 255.0f), uint8_t(blue * 255.0f), uint8_t(alpha * 255.0f)); }

  inline color ColorLerp(const color& p1, const color& p2, float t)
  { return (p2 * t) + p1 * (1.0f - t); }


///Represents a pysical button state (keyboard, mouse, etc) that can be pressed, released or held (taken from olcPixelGameEngine.h)
struct hwbutton
{
  bool bPressed = false;	// Set once during the frame the event occurs
  bool bReleased = false;	// Set once during the frame the event occurs
  bool bHeld = false;		// Set true for all frames between pressed and released events
};

/// Font glyph structure (taken from Adafruit GFX library)
struct fontglyph {
    uint16_t bitmapOffset; // Pointer into GFXfont->bitmap
    uint8_t width;
    uint8_t height; // Bitmap dimensions in pixels
    uint8_t xAdvance;      // Distance to advance cursor (x axis)
    int8_t xOffset;
    int8_t yOffset; // Dist from cursor pos to UL corner
};

/// Font structure (take from Adafruit GFX library)
struct font {
    uint8_t *bitmap;       // Glyph bitmaps, concatenated
    fontglyph *glyph;       // Glyph array
    uint16_t first;
    uint16_t last;   // ASCII extents
    uint8_t yAdvance;      // Newline distance (y axis)
};

class IDrzEngine {
public:

  // Input methods
  virtual void ReadKeys() = 0;
  virtual hwbutton GetKey(int key) = 0;

  // Drawing methods
  virtual void DrawLine(int x1, int y1, int x2, int y2, color color) = 0;
  virtual void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, color color) = 0;
  virtual void Clear(color color) = 0;
  virtual void FillRect(int x, int y, int w, int h, color color) = 0;
  virtual void FillTriangle(vec2d p1, vec2d p2, vec2d p3, color color) = 0;
  virtual void FillCircle(int x, int y, int radius, color color) = 0;

  // Font-related methods
  virtual void LoadFont(const std::string& fontName, const font* font) = 0;
  virtual void SetFont(const std::string& fontName) = 0;
  virtual void DrawText(const std::string& text, float x, float y, color color) = 0;
  virtual void SetCursorPos(uint16_t x, uint16_t y) = 0;

  virtual int GetScreenWidth() = 0; 
  virtual int GetScreenHeight() = 0;  

  //retreive a random float between 0 and 1
  virtual float GetRandomFloat() = 0;
  
  //get the current time in milliseconds
  virtual uint32_t Now() = 0;
};

