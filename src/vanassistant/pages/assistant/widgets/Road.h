#pragma once

#include "DrzGraphics.h"

using namespace drz;

class Road {
  public:
  
    Road() : gfx(DrzGraphics::Get()), roadSpeed(88.0f), roadOffset(0.0f), currentFrame(0), eyePos({0, 0, 400})  {
      screenWidth = gfx->GetScreenWidth();
      screenHeight = gfx->GetScreenHeight();
    }

    void Update(float elapsedTime) {
      roadOffset += roadSpeed * elapsedTime;
      if (roadOffset >= squaresize) {
        roadOffset -= squaresize;
        currentFrame += 1;
      }
    }

    void Render() {
      eyePos.x = screenWidth / 2.0f;
      eyePos.y = screenHeight / 2.0f;
      float end = gridEnd * screenHeight * 2;
      for (int i = -100; i < end; i += squaresize) {
        float currentz = i - roadOffset;
        drawLine3D(-0.0f * screenWidth, 50, currentz, 1.0f * screenWidth, 50, currentz, WHITE);
      }
      for (int i = -0.0f * screenWidth; i < 1.0f * screenWidth; i += squaresize) {
        drawLine3D(i, 50, -200, i, 50, end, WHITE);
      }
      drawLine3D(-0.0f * screenWidth, 50, end, screenWidth * 1.0f, 50, end, WHITE);
    }

  private: 
  
    IDrzGraphics* gfx;
    
    int screenWidth;
    int screenHeight;
    float roadSpeed;
    float roadOffset;
    const int squaresize = 50;
    const float gridEnd = 0.6f;
    int currentFrame;
    vec3d eyePos;

    vec2d get3DCoords(float x, float y, float z) {
      float perspective = eyePos.z / (eyePos.z + z);
      float screenX = (screenWidth / 2.0f) + (x - eyePos.x) * perspective;
      float screenY = (screenHeight / 2.0f) - (y - eyePos.y) * perspective;
      return { screenX, screenY };
    }

    void drawLine3D(float x1, float y1, float z1, float x2, float y2, float z2, Color color) {
      vec2d p1 = get3DCoords(x1, y1, z1);
      vec2d p2 = get3DCoords(x2, y2, z2);
      gfx->DrawLine(p1.x, p1.y, p2.x, p2.y, color);
    }
};
