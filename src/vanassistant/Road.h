#pragma once

#include "../../include/IDrzEngine.h"
#include <vector>

class Road {
  public:
  
    Road(IDrzEngine* engine) : engine(engine), roadSpeed(88.0f), roadOffset(0.0f), currentFrame(0), eyePos({0, 0, 400})  {
      screenWidth = engine->GetScreenWidth();
      screenHeight = engine->GetScreenHeight();
    }
 
    void Update(float elapsedTime) {
      roadOffset += roadSpeed * elapsedTime;
      if (roadOffset >= squaresize) {
        roadOffset -= squaresize;
        currentFrame += 1;
      }
    }

    void Render() {
      eyePos.x = engine->GetScreenWidth() / 2;
      eyePos.y = engine->GetScreenHeight() / 2;
      float end = gridEnd * engine->GetScreenHeight() * 2;
      for (int i = -100; i < end; i += squaresize) {
          float currentz = i - roadOffset;
          drawLine3D(-0.0f * engine->GetScreenWidth(), 50, currentz, 1.0f * engine->GetScreenWidth(), 50, currentz, WHITE);
      }
      for (int i = -0.0f * engine->GetScreenWidth(); i < 1.0f * engine->GetScreenWidth(); i += squaresize) {
          drawLine3D(i, 50, -200, i, 50, end, WHITE);
      }
      drawLine3D(-0.0f * engine->GetScreenWidth(), 50, end, engine->GetScreenWidth() * 1.0f, 50, end, WHITE);
    }

  private: 
  
    IDrzEngine* engine;
    
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
      float screenX = (engine->GetScreenWidth() / 2.0f) + (x - eyePos.x) * perspective;
      float screenY = (engine->GetScreenHeight() / 2.0f) - (y - eyePos.y) * perspective;
      return { screenX, screenY };
    }

    void drawLine3D(float x1, float y1, float z1, float x2, float y2, float z2, color color) {
      vec2d p1 = get3DCoords(x1, y1, z1);
      vec2d p2 = get3DCoords(x2, y2, z2);
      engine->DrawLine(p1, p2, color);
    }
};
