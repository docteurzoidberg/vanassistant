#include <ESP_8_BIT_GFX.h>
#include "Computerfont12pt7b.h"
#include "Computerfont8pt7b.h"

#include <cmath>
#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>
#include <array>
#include <cstring>

#define SCREEN_W 256
#define SCREEN_H 240

#define NUM_STARS 500
#define GRID_SIZE 10

// Create an instance of the graphics library
ESP_8_BIT_GFX videoOut(true /* = NTSC */, 8 /* = RGB332 color */);

unsigned long prevTime = 0;
uint8_t ticks = 0;

struct vec2d
{
	float x, y;
};

struct vec3d
{
	float x, y, z;
};

struct triangle
{
	vec3d p[3];
};

struct mat4x4
{
	float m[4][4] = { 0 };
};

struct Star 
{
	float x, y, z, o;
};

struct FaceMesh
{
	std::vector<triangle> tris = {
		{
			{{ {-0.221806,0.810647,0.529052}, {-0.092378,0.98376,0.689843}, {-0.309036,1.15845,0.722323} }},
			{{ {-0.889436,0.811903,0.334968}, {-1.10621,0.237401,0.359309}, {-0.788894,0.694305,0.423358} }},
			{{ {-1.03143,1.12304,0.427065}, {-0.486244,2.01793,0.34961}, {-1.08231,1.77302,-0.042465} }},
			{{ {-0.221806,0.810647,0.529052}, {-0.526123,0.696689,0.493042}, {-0.304605,0.230392,0.771424} }},
			{{ {-1.03143,1.12304,0.427065}, {-1.08231,1.77302,-0.042465}, {-1.15905,0.775762,0.067383} }},
			{{ {-0.075977,-0.180438,0.877235}, {-0.116373,-0.052441,1.16916}, {-0.300686,-0.093373,0.784912} }},
			{{ {-1.10621,0.237401,0.359309}, {-0.374356,0.084104,0.732422}, {-0.304605,0.230392,0.771424} }},
			{{ {-0.899232,-0.429866,0.271792}, {-0.542953,-0.545303,0.702881}, {-0.374356,0.084104,0.732422} }},
			{{ {-0.374356,0.084104,0.732422}, {-0.116373,-0.052441,1.16916}, {-0.119486,0.13388,1.2632} }},
			{{ {-0.075977,-0.180438,0.877235}, {-0.306486,-0.413877,0.890231}, {-0.078171,-0.407055,1.00258} }},
			{{ {-0.300686,-0.093373,0.784912}, {-0.542953,-0.545303,0.702881}, {-0.306486,-0.413877,0.890231} }},
			{{ {-0.542953,-0.545303,0.702881}, {-0.899232,-0.429866,0.271792}, {-0.559025,-1.01292,0.584976} }},
			{{ {-0.203802,-0.655114,0.898147}, {-0.268492,-0.834577,0.780437}, {0.008665,-0.658158,0.952657} }},
			{{ {-0.542953,-0.545303,0.702881}, {-0.268492,-0.834577,0.780437}, {-0.203802,-0.655114,0.898147} }},
			{{ {-0.542953,-0.545303,0.702881}, {-0.300686,-0.093373,0.784912}, {-0.374356,0.084104,0.732422} }},
			{{ {-1.03143,1.12304,0.427065}, {-0.510646,0.950842,0.456529}, {-0.309036,1.15845,0.722323} }},
			{{ {-1.03143,1.12304,0.427065}, {-0.889436,0.811903,0.334968}, {-0.802036,0.935402,0.396395} }},
			{{ {-0.788894,0.694305,0.423358}, {-0.304605,0.230392,0.771424}, {-0.526123,0.696689,0.493042} }},
			{{ {-0.510646,0.950842,0.456529}, {-0.221806,0.810647,0.529052}, {-0.309036,1.15845,0.722323} }},
			{{ {-0.268492,-0.834577,0.780437}, {-0.542953,-0.545303,0.702881}, {-0.559025,-1.01292,0.584976} }},
			{{ {-0.290718,-1.11718,0.883348}, {-0.268492,-0.834577,0.780437}, {-0.559025,-1.01292,0.584976} }},
			{{ {-0.119486,0.13388,1.2632}, {-0.304605,0.230392,0.771424}, {-0.374356,0.084104,0.732422} }},
			{{ {-0.092378,0.98376,0.689843}, {-0.304605,0.230392,0.771424}, {-0.119486,0.13388,1.2632} }},
			{{ {0.238624,0.810647,0.529052}, {0.325855,1.15845,0.722323}, {0.109196,0.98376,0.689843} }},
			{{ {1.12303,0.237401,0.359309}, {0.906254,0.811903,0.334968}, {0.805713,0.694305,0.423358} }},
			{{ {1.04825,1.12304,0.427065}, {0.503063,2.01793,0.34961}, {0.325855,1.15845,0.722323} }},
			{{ {0.542941,0.696689,0.493042}, {0.238624,0.810647,0.529052}, {0.321424,0.230392,0.771424} }},
			{{ {1.04825,1.12304,0.427065}, {1.17587,0.775762,0.067383}, {1.09554,1.76419,-0.03635} }},
			{{ {0.092796,-0.180438,0.877235}, {0.317505,-0.093373,0.784912}, {0.133191,-0.052441,1.16916} }},
			{{ {0.391175,0.084104,0.732422}, {1.12303,0.237401,0.359309}, {0.321424,0.230392,0.771424} }},
			{{ {0.916051,-0.429866,0.271792}, {0.391175,0.084104,0.732422}, {0.559772,-0.545303,0.702881} }},
			{{ {0.133191,-0.052441,1.16916}, {0.391175,0.084104,0.732422}, {0.136304,0.13388,1.2632} }},
			{{ {0.092796,-0.180438,0.877235}, {0.323305,-0.413877,0.890231}, {0.317505,-0.093373,0.784912} }},
			{{ {0.317505,-0.093373,0.784912}, {0.323305,-0.413877,0.890231}, {0.559772,-0.545303,0.702881} }},
			{{ {0.559772,-0.545303,0.702881}, {0.575843,-1.01292,0.584976}, {0.916051,-0.429866,0.271792} }},
			{{ {0.22062,-0.655114,0.898147}, {0.008665,-0.658158,0.952657}, {0.28531,-0.834577,0.780437} }},
			{{ {0.559772,-0.545303,0.702881}, {0.22062,-0.655114,0.898147}, {0.28531,-0.834577,0.780437} }},
			{{ {-0.268492,-0.834577,0.780437}, {0.28531,-0.834577,0.780437}, {0.008665,-0.658158,0.952657} }},
			{{ {0.559772,-0.545303,0.702881}, {0.391175,0.084104,0.732422}, {0.317505,-0.093373,0.784912} }},
			{{ {0.527464,0.950842,0.456529}, {1.04825,1.12304,0.427065}, {0.325855,1.15845,0.722323} }},
			{{ {1.04825,1.12304,0.427065}, {0.906254,0.811903,0.334968}, {1.17587,0.775762,0.067383} }},
			{{ {0.321424,0.230392,0.771424}, {0.805713,0.694305,0.423358}, {0.542941,0.696689,0.493042} }},
			{{ {0.527464,0.950842,0.456529}, {0.238624,0.810647,0.529052}, {0.368223,0.799255,0.429694} }},
			{{ {0.28531,-0.834577,0.780437}, {0.575843,-1.01292,0.584976}, {0.559772,-0.545303,0.702881} }},
			{{ {0.307536,-1.11718,0.883348}, {0.575843,-1.01292,0.584976}, {0.28531,-0.834577,0.780437} }},
			{{ {0.136304,0.13388,1.2632}, {0.391175,0.084104,0.732422}, {0.321424,0.230392,0.771424} }},
			{{ {0.321424,0.230392,0.771424}, {0.109196,0.98376,0.689843}, {0.136304,0.13388,1.2632} }},
			{{ {-0.075977,-0.180438,0.877235}, {0.094989,-0.407055,1.00258}, {0.092796,-0.180438,0.877235} }},
			{{ {-0.116373,-0.052441,1.16916}, {0.092796,-0.180438,0.877235}, {0.133191,-0.052441,1.16916} }},
			{{ {0.133191,-0.052441,1.16916}, {-0.119486,0.13388,1.2632}, {-0.116373,-0.052441,1.16916} }},
			{{ {0.136304,0.13388,1.2632}, {-0.092378,0.98376,0.689843}, {-0.119486,0.13388,1.2632} }},
			{{ {0.109196,0.98376,0.689843}, {-0.309036,1.15845,0.722323}, {-0.092378,0.98376,0.689843} }},
			{{ {0.325855,1.15845,0.722323}, {-0.486244,2.01793,0.34961}, {-0.309036,1.15845,0.722323} }},
			{{ {-0.268492,-0.834577,0.780437}, {0.307536,-1.11718,0.883348}, {0.28531,-0.834577,0.780437} }},
			{{ {-0.889436,0.811903,0.334968}, {-1.15905,0.775762,0.067383}, {-1.10621,0.237401,0.359309} }},
			{{ {-1.03143,1.12304,0.427065}, {-0.309036,1.15845,0.722323}, {-0.486244,2.01793,0.34961} }},
			{{ {-0.221806,0.810647,0.529052}, {-0.351405,0.799255,0.429694}, {-0.526123,0.696689,0.493042} }},
			{{ {-1.10621,0.237401,0.359309}, {-0.899232,-0.429866,0.271792}, {-0.374356,0.084104,0.732422} }},
			{{ {-0.374356,0.084104,0.732422}, {-0.300686,-0.093373,0.784912}, {-0.116373,-0.052441,1.16916} }},
			{{ {-0.075977,-0.180438,0.877235}, {-0.300686,-0.093373,0.784912}, {-0.306486,-0.413877,0.890231} }},
			{{ {-1.03143,1.12304,0.427065}, {-0.802036,0.935402,0.396395}, {-0.510646,0.950842,0.456529} }},
			{{ {-1.03143,1.12304,0.427065}, {-1.15905,0.775762,0.067383}, {-0.889436,0.811903,0.334968} }},
			{{ {-0.788894,0.694305,0.423358}, {-1.10621,0.237401,0.359309}, {-0.304605,0.230392,0.771424} }},
			{{ {-0.510646,0.950842,0.456529}, {-0.351405,0.799255,0.429694}, {-0.221806,0.810647,0.529052} }},
			{{ {-0.092378,0.98376,0.689843}, {-0.221806,0.810647,0.529052}, {-0.304605,0.230392,0.771424} }},
			{{ {1.12303,0.237401,0.359309}, {1.17587,0.775762,0.067383}, {0.906254,0.811903,0.334968} }},
			{{ {1.04825,1.12304,0.427065}, {1.09554,1.76419,-0.03635}, {0.503063,2.01793,0.34961} }},
			{{ {0.542941,0.696689,0.493042}, {0.368223,0.799255,0.429694}, {0.238624,0.810647,0.529052} }},
			{{ {0.391175,0.084104,0.732422}, {0.916051,-0.429866,0.271792}, {1.12303,0.237401,0.359309} }},
			{{ {0.133191,-0.052441,1.16916}, {0.317505,-0.093373,0.784912}, {0.391175,0.084104,0.732422} }},
			{{ {0.092796,-0.180438,0.877235}, {0.094989,-0.407055,1.00258}, {0.323305,-0.413877,0.890231} }},
			{{ {0.527464,0.950842,0.456529}, {0.818854,0.935402,0.396395}, {1.04825,1.12304,0.427065} }},
			{{ {1.04825,1.12304,0.427065}, {0.818854,0.935402,0.396395}, {0.906254,0.811903,0.334968} }},
			{{ {0.321424,0.230392,0.771424}, {1.12303,0.237401,0.359309}, {0.805713,0.694305,0.423358} }},
			{{ {0.527464,0.950842,0.456529}, {0.325855,1.15845,0.722323}, {0.238624,0.810647,0.529052} }},
			{{ {0.321424,0.230392,0.771424}, {0.238624,0.810647,0.529052}, {0.109196,0.98376,0.689843} }},
			{{ {-0.075977,-0.180438,0.877235}, {-0.078171,-0.407055,1.00258}, {0.094989,-0.407055,1.00258} }},
			{{ {-0.116373,-0.052441,1.16916}, {-0.075977,-0.180438,0.877235}, {0.092796,-0.180438,0.877235} }},
			{{ {0.133191,-0.052441,1.16916}, {0.136304,0.13388,1.2632}, {-0.119486,0.13388,1.2632} }},
			{{ {0.136304,0.13388,1.2632}, {0.109196,0.98376,0.689843}, {-0.092378,0.98376,0.689843} }},
			{{ {0.109196,0.98376,0.689843}, {0.325855,1.15845,0.722323}, {-0.309036,1.15845,0.722323} }},
			{{ {0.325855,1.15845,0.722323}, {0.503063,2.01793,0.34961}, {-0.486244,2.01793,0.34961} }},
			{{ {-0.268492,-0.834577,0.780437}, {-0.290718,-1.11718,0.883348}, {0.307536,-1.11718,0.883348} }}
		}
	};
};

float fAccumulatedTime = 0.0f;
float fTargetFrameTime = 100/1000.0f;

double lastUpdate=0;

bool bShowFps = true;
bool bShowDebug = false;

//Face
FaceMesh meshFace;
mat4x4 matProj;
float fTheta = 2* 3.14159f;

//STARFIELD
int focalLength = SCREEN_W*2;
bool warp = false;
float centerX = SCREEN_W / 2;
float centerY = SCREEN_H / 2;
std::vector<Star> stars;

//GRID
const float gridDepth = 300.0f; // Depth of the grid in the Z direction
const float gridHeight = 120.0f; // Height of the grid in the Y direction
const float fov = 90.0f; // Field of view in degrees
const float aspectRatio = static_cast<float>(SCREEN_W) / static_cast<float>(SCREEN_H);
const float fovRad = 1.0f / tan(fov * 0.5f / 180.0f * 3.14159f);

float getRandomFloat() {
  //uint32_t randomInt = esp_random();
  float randomValue = random(0, 10000) / 10000.0f;
  // Normalize the random integer to a float in the range [0.0, 1.0]
  //float randomValue = static_cast<float>(randomInt) / 4294967295.0f;
  return randomValue;
}

void MultiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m)
{
  o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
  o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
  o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
  float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];
  if (w != 0.0f)
  {
    o.x /= w; o.y /= w; o.z /= w;
  }
}

// Function to project 3D coordinates to 2D screen coordinates
vec2d Project(float x, float y, float z ) {
  float screenX = (x / (z * aspectRatio)) * fovRad * SCREEN_W + SCREEN_W / 2.0f;
  float screenY = (y / z) * fovRad * SCREEN_H + SCREEN_H / 2.0f;
  return {screenX, screenY};
}

void InitStartField() {
  stars.clear();
  for(int i = 0; i < NUM_STARS; i++){
    stars.push_back({
      x: getRandomFloat() * (float)SCREEN_W,
      y: getRandomFloat() * (float)SCREEN_H,
      z: getRandomFloat() * (float)SCREEN_W,
      o: (floor(getRandomFloat() * 99.0f) + 1.0f) / 10
    });
  }
}


void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int color) {
  videoOut.drawLine(x1, y1, x2, y2, color);
  videoOut.drawLine(x2, y2, x3, y3, color);
  videoOut.drawLine(x3, y3, x1, y1, color);
}

void DrawLine(int x1, int y1, int x2, int y2, int color) {
  videoOut.drawLine(x1, y1, x2, y2, color);
}

void FillRect(int x, int y, int w, int h, int color) {
  videoOut.fillRect(x,y,w,h,color);
}

void DrawHeader() {
  videoOut.setCursor(20,20);
  videoOut.setFont(&Computerfont12pt7b);
  videoOut.print(" Van Assistant 2000");
  videoOut.drawLine(40,25, 216,25, 0xFF);
}

void DrawFps(uint8_t fps) {
  // Draw text in the middle of the screen
  videoOut.setFont(&Computerfont8pt7b);
  videoOut.setCursor(130,220);
  videoOut.setTextColor(0xFF);
  videoOut.print("FPS:");
  videoOut.print(fps);
}

void DrawGrid() {
  // Draw the one-axis perspective grid (Z-axis lines)
  for (int i = -GRID_SIZE; i <= GRID_SIZE; ++i) {
    for (int j = 1; j < gridDepth; j += GRID_SIZE) {
      
      // Lines parallel to the X-axis
      vec2d startX = Project(-GRID_SIZE * GRID_SIZE, gridHeight, j);
      vec2d endX = Project(GRID_SIZE * GRID_SIZE, gridHeight, j);
      DrawLine(startX.x, startX.y, endX.x, endX.y, 0xFF);

      // Lines parallel to the Z-axis
      vec2d startZ = Project(i * GRID_SIZE, gridHeight, j);
      vec2d endZ = Project(i * GRID_SIZE, gridHeight, j + GRID_SIZE);
      DrawLine(startZ.x, startZ.y, endZ.x, endZ.y, 0xFF);
    }
  }
}

void DrawStarfield() {
  int pixelX;
  int pixelY;
  int pixelRadius;
  for(auto &star : stars){
    star.z-= 1.0f;
    if(star.z <= 0){
      star.z = SCREEN_W;
    }
    pixelX = (star.x - centerX) * (focalLength / star.z);
    pixelX += centerX;
    pixelY = (star.y - centerY) * (focalLength / star.z);
    pixelY += centerY;
    pixelRadius = 1 * (focalLength / star.z);
    uint8_t brightness = star.o * 255;

    // Calculate the RGB values based on adjusted grayscale
    //uint8_t red = (brightness >> 5) & 0x07; // Top 3 bits for red
    //uint8_t green = (brightness >> 2) & 0x07; // Next 3 bits for green
    //uint8_t blue = (brightness >> 6) & 0x03; // Top 2 bits for blue (shifted to fit 2 bits)

    // Combine the RGB values into RGB332 format
    //uint8_t rgb332 = (red << 5) | (green << 2) | blue;

    FillRect(pixelX, pixelY, pixelRadius, pixelRadius, brightness);
  }
}

void DrawFace3d() {
  mat4x4 matRotZ, matRotX;
  fTheta += 0.1f;

  // Rotation Z
  matRotZ.m[0][0] = cosf(fTheta);
  matRotZ.m[0][1] = sinf(fTheta);
  matRotZ.m[1][0] = -sinf(fTheta);
  matRotZ.m[1][1] = cosf(fTheta);
  matRotZ.m[2][2] = 1;
  matRotZ.m[3][3] = 1;

  // Rotation X
  matRotX.m[0][0] = 1;
  matRotX.m[1][1] = cosf(fTheta * 0.5f);
  matRotX.m[1][2] = sinf(fTheta * 0.5f);
  matRotX.m[2][1] = -sinf(fTheta * 0.5f);
  matRotX.m[2][2] = cosf(fTheta * 0.5f);
  matRotX.m[3][3] = 1;

  // Draw Triangles
  for (auto tri : meshFace.tris)
  {
    triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

    // Rotate in Z-Axis
    MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
    MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
    MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);

    // Rotate in X-Axis
    MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
    MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
    MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

    // Offset into the screen
    triTranslated = triRotatedZX;
    triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
    triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
    triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

    // Project triangles from 3D --> 2D
    MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
    MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
    MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

    // Scale into view
    triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
    triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
    triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
    triProjected.p[0].x *= 0.5f * (float)SCREEN_W;
    triProjected.p[0].y *= 0.5f * (float)SCREEN_H;
    triProjected.p[1].x *= 0.5f * (float)SCREEN_W;
    triProjected.p[1].y *= 0.5f * (float)SCREEN_H;
    triProjected.p[2].x *= 0.5f * (float)SCREEN_W;
    triProjected.p[2].y *= 0.5f * (float)SCREEN_H;

    // Rasterize triangle
    DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
      triProjected.p[1].x, triProjected.p[1].y,
      triProjected.p[2].x, triProjected.p[2].y, 0xFF);

    //videoOut.drawLine((int)triProjected.p[0].x, (int)triProjected.p[0].y,
    //  (int)triProjected.p[1].x, (int)triProjected.p[1].y, 0xFF);
    //videoOut.drawLine(triProjected.p[1].x, (int)triProjected.p[1].y,
    //  (int)triProjected.p[2].x, (int)triProjected.p[2].y, 0xFF);
    //videoOut.drawLine(triProjected.p[2].x, (int)triProjected.p[2].y,
    //  (int)triProjected.p[0].x, (int)triProjected.p[0].y, 0xFF);

  }
}

void setup() {
  prevTime = millis();

  // Initial setup of graphics library
  videoOut.begin();

  // Measure size of rendered text
  videoOut.setFont(&Computerfont12pt7b);
  videoOut.setTextColor(0xFF);
  //videoOut.setTextSize(1);

  float fNear = 0.1f;
  float fFar = 1000.0f;
  float fFov = 90.0f;
  float fAspectRatio = 208.0f / 240.0f;
  float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

  matProj.m[0][0] = fAspectRatio * fFovRad;
  matProj.m[1][1] = fFovRad;
  matProj.m[2][2] = fFar / (fFar - fNear);
  matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
  matProj.m[2][3] = 1.0f;
  matProj.m[3][3] = 0.0f;

  InitStartField();
}


void loop() {

  unsigned long now = millis();
  static int64_t stime  = esp_timer_get_time();
  static int FPS        = 0;
  static int FPSCounter = 0;

  // Wait for the next frame to minimize chance of visible tearing
  videoOut.waitForFrame();

  if (esp_timer_get_time() - stime > 1000000) {
    // calculate FPS
    FPS = FPSCounter;
    stime = esp_timer_get_time();
    FPSCounter = 0;
  }
  ++FPSCounter;

  //float fElapsedTime= ((float)(stime)) * 0.000001f;
  //fAccumulatedTime += fElapsedTime;

  // Clear screen
  videoOut.fillScreen(0);

  DrawHeader();
  DrawStarfield();
  //videoOut.waitForFrame();
	//DrawGrid();
  DrawFace3d();
  DrawFps(FPS);

  lastUpdate=millis();
}
