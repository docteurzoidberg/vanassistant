#pragma once
 
#include <cmath>
#include <cstdint>
#include <string>

struct rect {
  int x, y, w, h;
};

struct vec2d {
  float x, y;
};

struct vec3d {
  float x, y, z, w;
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

class Matrix4x4 {
public:
    float m[4][4];

    static Matrix4x4 Identity() {
      Matrix4x4 matrix = {};
      for (int i = 0; i < 4; ++i) matrix.m[i][i] = 1.0f;
      return matrix;
    }

    static Matrix4x4 CreateRotationMatrixX(float angle) {
      Matrix4x4 matrix = Matrix4x4::Identity();
      matrix[1][1] = cos(angle);
      matrix[1][2] = -sin(angle);
      matrix[2][1] = sin(angle);
      matrix[2][2] = cos(angle);
      return matrix;
    }

    static Matrix4x4 CreateRotationMatrixY(float angle) {
      Matrix4x4 matrix = Matrix4x4::Identity();
      matrix[0][0] = cos(angle);
      matrix[0][2] = sin(angle);
      matrix[2][0] = -sin(angle);
      matrix[2][2] = cos(angle);
      return matrix;
    }

    static Matrix4x4 CreateRotationMatrixZ(float angle) {
      Matrix4x4 matrix = Matrix4x4::Identity();
      matrix[0][0] = cos(angle);
      matrix[0][1] = -sin(angle);
      matrix[1][0] = sin(angle);
      matrix[1][1] = cos(angle);
      return matrix;
    }

    static Matrix4x4 CreateRotationMatrix(float angleX, float angleY, float angleZ) {
      Matrix4x4 matrixX = Matrix4x4::CreateRotationMatrixX(angleX);
      Matrix4x4 matrixY = Matrix4x4::CreateRotationMatrixY(angleY);
      Matrix4x4 matrixZ = Matrix4x4::CreateRotationMatrixZ(angleZ);
      return matrixZ * matrixY * matrixX;
    }

    static Matrix4x4 CreateTranslationMatrix(float x, float y, float z) {
      Matrix4x4 matrix = Matrix4x4::Identity();
      matrix[0][3] = x;
      matrix[1][3] = y;
      matrix[2][3] = z;
      return matrix;
    }

    Matrix4x4 operator*(const Matrix4x4& other) const {
      Matrix4x4 result = {};
      for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
          for (int k = 0; k < 4; ++k) {
            result.m[i][j] += m[i][k] * other.m[k][j];
          }
        }
      }
      return result;
    }

    float* operator[](int index) { return m[index]; }
    const float* operator[](int index) const { return m[index]; }
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


struct trianglec {
  vec3d p[3];
  color col;
};

struct trianglecref {
  vec3d* p[3];
  color col;
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
  virtual hwbutton GetKey(uint8_t key) = 0;

  // Drawing methods
  virtual void Clear(color color) = 0;
  virtual void DrawPixel(int x, int y, color color) = 0;  
  virtual void DrawLine(int x1, int y1, int x2, int y2, color color) = 0;
  virtual void DrawLine(vec2d p1, vec2d p2, color color) = 0;
  virtual void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, color color) = 0;
  virtual void FillRect(int x, int y, int w, int h, color color) = 0;
  virtual void FillTriangle(vec2d p1, vec2d p2, vec2d p3, color color) = 0;
  virtual void FillCircle(int x, int y, int radius, color color) = 0;

  // Font-related methods
  virtual const font* LoadFont(const std::string& fontName, const font* font) = 0;
  virtual void SetFont(const std::string& fontName) = 0;
  virtual void SetFont(const font* font) = 0;
  virtual void SetTextColor(color color) = 0;
  virtual void DrawText(const std::string& text, float x, float y, color color) = 0;

  virtual void SetCursorPos(uint16_t x, uint16_t y) = 0;
  virtual void SetWrap(bool wrap) = 0;

  virtual int GetScreenWidth() = 0; 
  virtual int GetScreenHeight() = 0;  

  virtual rect GetTextBounds(const std::string& text, int x, int y) = 0;

  //retreive a random float between 0 and 1
  virtual float GetRandomFloat() = 0;

  virtual uint32_t GetFPS() = 0;
  
  //get the current time in milliseconds
  virtual long Now() = 0;

  vec3d Matrix_MultiplyVector(mat4x4 &m, vec3d &i) {
    vec3d v;
    v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
    v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
    v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
    v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
    return v;
  }

  mat4x4 Matrix_MakeIdentity() {
    mat4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    return matrix;
  }

  mat4x4 Matrix_MakeRotationX(float fAngleRad) {
    mat4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = cosf(fAngleRad);
    matrix.m[1][2] = sinf(fAngleRad);
    matrix.m[2][1] = -sinf(fAngleRad);
    matrix.m[2][2] = cosf(fAngleRad);
    matrix.m[3][3] = 1.0f;
    return matrix;
  }

  mat4x4 Matrix_MakeRotationY(float fAngleRad) {
    mat4x4 matrix;
    matrix.m[0][0] = cosf(fAngleRad);
    matrix.m[0][2] = sinf(fAngleRad);
    matrix.m[2][0] = -sinf(fAngleRad);
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = cosf(fAngleRad);
    matrix.m[3][3] = 1.0f;
    return matrix;
  }

  mat4x4 Matrix_MakeRotationZ(float fAngleRad) {
    mat4x4 matrix;
    matrix.m[0][0] = cosf(fAngleRad);
    matrix.m[0][1] = sinf(fAngleRad);
    matrix.m[1][0] = -sinf(fAngleRad);
    matrix.m[1][1] = cosf(fAngleRad);
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    return matrix;
  }

  mat4x4 Matrix_MakeTranslation(float x, float y, float z) {
    mat4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    matrix.m[3][0] = x;
    matrix.m[3][1] = y;
    matrix.m[3][2] = z;
    return matrix;
  }

  mat4x4 Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar) {
    float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
    mat4x4 matrix;
    matrix.m[0][0] = fAspectRatio * fFovRad;
    matrix.m[1][1] = fFovRad;
    matrix.m[2][2] = fFar / (fFar - fNear);
    matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matrix.m[2][3] = 1.0f;
    matrix.m[3][3] = 0.0f;
    return matrix;
  }

  mat4x4 Matrix_MultiplyMatrix(mat4x4 &m1, mat4x4 &m2) {
    mat4x4 matrix;
    for (int c = 0; c < 4; c++)
      for (int r = 0; r < 4; r++)
        matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
    return matrix;
  }

  mat4x4 Matrix_PointAt(vec3d &pos, vec3d &target, vec3d &up) {
    // Calculate new forward direction
    vec3d newForward = Vector_Sub(target, pos);
    newForward = Vector_Normalise(newForward);

    // Calculate new Up direction
    vec3d a = Vector_Mul(newForward, Vector_DotProduct(up, newForward));
    vec3d newUp = Vector_Sub(up, a);
    newUp = Vector_Normalise(newUp);

    // New Right direction is easy, its just cross product
    vec3d newRight = Vector_CrossProduct(newUp, newForward);

    // Construct Dimensioning and Translation Matrix	
    mat4x4 matrix;
    matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;	matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;		matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;	matrix.m[2][3] = 0.0f;
    matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1.0f;
    return matrix;

  }

  mat4x4 Matrix_QuickInverse(mat4x4 &m) { 
    // Only for Rotation/Translation Matrices
    mat4x4 matrix;
    matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
    matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
    matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
    matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
    matrix.m[3][3] = 1.0f;
    return matrix;
  }

  vec3d Vector_Add(vec3d &v1, vec3d &v2) {
    return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
  }

  vec3d Vector_Sub(vec3d &v1, vec3d &v2) {
    return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
  }

  vec3d Vector_Mul(vec3d &v1, float k) {
    return { v1.x * k, v1.y * k, v1.z * k };
  }

  vec3d Vector_Div(vec3d &v1, float k) {
    return { v1.x / k, v1.y / k, v1.z / k };
  }

  float Vector_DotProduct(vec3d &v1, vec3d &v2) {
    return v1.x*v2.x + v1.y*v2.y + v1.z * v2.z;
  }

  float Vector_Length(vec3d &v) {
    return sqrtf(Vector_DotProduct(v, v));
  }

  vec3d Vector_Normalise(vec3d &v) {
    float l = Vector_Length(v);
    return { v.x / l, v.y / l, v.z / l };
  }

  vec3d Vector_CrossProduct(vec3d &v1, vec3d &v2) {
    vec3d v;
    v.x = v1.y * v2.z - v1.z * v2.y;
    v.y = v1.z * v2.x - v1.x * v2.z;
    v.z = v1.x * v2.y - v1.y * v2.x;
    return v;
  }

  vec3d Vector_IntersectPlane(vec3d &plane_p, vec3d &plane_n, vec3d &lineStart, vec3d &lineEnd) {
    plane_n = Vector_Normalise(plane_n);
    float plane_d = -Vector_DotProduct(plane_n, plane_p);
    float ad = Vector_DotProduct(lineStart, plane_n);
    float bd = Vector_DotProduct(lineEnd, plane_n);
    float t = (-plane_d - ad) / (bd - ad);
    vec3d lineStartToEnd = Vector_Sub(lineEnd, lineStart);
    vec3d lineToIntersect = Vector_Mul(lineStartToEnd, t);
    return Vector_Add(lineStart, lineToIntersect);
  }

  int Triangle_ClipAgainstPlane(vec3d plane_p, vec3d plane_n, trianglec &in_tri, trianglec &out_tri1, trianglec &out_tri2) {
    // Make sure plane normal is indeed normal
    plane_n = Vector_Normalise(plane_n);

    // Return signed shortest distance from point to plane, plane normal must be normalised
    auto dist = [&](vec3d &p)
    {
      vec3d n = Vector_Normalise(p);
      return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - Vector_DotProduct(plane_n, plane_p));
    };

    // Create two temporary storage arrays to classify points either side of plane
    // If distance sign is positive, point lies on "inside" of plane
    vec3d* inside_points[3];  int nInsidePointCount = 0;
    vec3d* outside_points[3]; int nOutsidePointCount = 0;

    // Get signed distance of each point in triangle to plane
    float d0 = dist(in_tri.p[0]);
    float d1 = dist(in_tri.p[1]);
    float d2 = dist(in_tri.p[2]);

    if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[0]; }
    else { outside_points[nOutsidePointCount++] = &in_tri.p[0]; }
    if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[1]; }
    else { outside_points[nOutsidePointCount++] = &in_tri.p[1]; }
    if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[2]; }
    else { outside_points[nOutsidePointCount++] = &in_tri.p[2]; }

    // Now classify triangle points, and break the input triangle into 
    // smaller output triangles if required. There are four possible
    // outcomes...

    if (nInsidePointCount == 0)
    {
      // All points lie on the outside of plane, so clip whole triangle
      // It ceases to exist

      return 0; // No returned triangles are valid
    }

    if (nInsidePointCount == 3)
    {
      // All points lie on the inside of plane, so do nothing
      // and allow the triangle to simply pass through
      out_tri1 = in_tri;

      return 1; // Just the one returned original triangle is valid
    }

    if (nInsidePointCount == 1 && nOutsidePointCount == 2)
    {
      // Triangle should be clipped. As two points lie outside
      // the plane, the triangle simply becomes a smaller triangle

      // Copy appearance info to new triangle
      out_tri1.col =  in_tri.col;

      // The inside point is valid, so keep that...
      out_tri1.p[0] = *inside_points[0];

      // but the two new points are at the locations where the 
      // original sides of the triangle (lines) intersect with the plane
      out_tri1.p[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
      out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

      return 1; // Return the newly formed single triangle
    }

    if (nInsidePointCount == 2 && nOutsidePointCount == 1)
    {
      // Triangle should be clipped. As two points lie inside the plane,
      // the clipped triangle becomes a "quad". Fortunately, we can
      // represent a quad with two new triangles

      // Copy appearance info to new triangles
      out_tri1.col =  in_tri.col;

      out_tri2.col =  in_tri.col;


      // The first triangle consists of the two inside points and a new
      // point determined by the location where one side of the triangle
      // intersects with the plane
      out_tri1.p[0] = *inside_points[0];
      out_tri1.p[1] = *inside_points[1];
      out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

      // The second triangle is composed of one of he inside points, a
      // new point determined by the intersection of the other side of the 
      // triangle and the plane, and the newly created point above
      out_tri2.p[0] = *inside_points[1];
      out_tri2.p[1] = out_tri1.p[2];
      out_tri2.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

      return 2; // Return two newly formed triangles which form a quad
    }
    return 0; 
  }
};

