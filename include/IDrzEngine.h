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

enum color {
  WHITE = 0xFFFFFF,
  BLACK = 0x000000,
  RED = 0xFF0000,
  GREEN = 0x00FF00,
  BLUE = 0x0000FF,
  YELLOW = 0xFFFF00,
  CYAN = 0x00FFFF,
  MAGENTA = 0xFF00FF
};

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
    uint8_t width, height; // Bitmap dimensions in pixels
    uint8_t xAdvance;      // Distance to advance cursor (x axis)
    int8_t xOffset, yOffset; // Dist from cursor pos to UL corner
};

/// Font structure (take from Adafruit GFX library)
struct font {
    uint8_t *bitmap;       // Glyph bitmaps, concatenated
    fontglyph *glyph;       // Glyph array
    uint8_t first, last;   // ASCII extents
    uint8_t yAdvance;      // Newline distance (y axis)
};

class IDrzEngine {
public:

  // Input methods
  virtual void ReadKeys() = 0;
  virtual hwbutton GetKey(int key) = 0;

  // Drawing methods
  virtual void DrawLine(float x1, float y1, float x2, float y2, color color) = 0;
  virtual void DrawTriangle(const vec3d& p1, const vec3d& p2, const vec3d& p3, color color) = 0;
  virtual void Clear(color color) = 0;
  virtual void FillRect(int x, int y, int w, int h, color color) = 0;

  // Font-related methods
  virtual void LoadFont(const std::string& fontName, const font* font) = 0;
  virtual void SetFont(const std::string& fontName) = 0;
  virtual void DrawText(const std::string& text, float x, float y, color color) = 0;

  virtual int GetScreenWidth() = 0; 
  virtual int GetScreenHeight() = 0;  

  //retreive a random float between 0 and 1
  virtual float GetRandomFloat() = 0;
  
  //get the current time in milliseconds
  virtual uint32_t Now() = 0;
};

