#pragma once

#include <IDrzEngine.h>

#include <cmath>
#include <vector>

using namespace drz;

struct star {
  float x, y, z, o;
};

class Starfield {
public:
  Starfield(IDrzEngine* engine, uint16_t starcount) : engine(engine), starcount(starcount) {
    for(int i = 0; i < starcount; i++){
      stars.push_back({
        .x = (float)engine->GetRandomFloat() * engine->GetScreenWidth(),
        .y = (float)engine->GetRandomFloat() * engine->GetScreenHeight(),
        .z = (float)engine->GetRandomFloat() * engine->GetScreenWidth(),
        .o = ((float)floor(engine->GetRandomFloat() * 99) + 1) / 10
      });
    }
    centerX = (float)engine->GetScreenWidth() / 2;
    centerY = (float)engine->GetScreenHeight() / 2;
    focalLength = engine->GetScreenWidth()*2;
  }

  void Update(float elapsedTime) {
    //TODO
  }

  void Render() {
    int pixelX;
    int pixelY;
    unsigned int pixelRadius;
    for(auto &star : stars){
      star.z-= 0.1f;
      if(star.z <= 0){
        star.z = engine->GetScreenWidth();
      }
      pixelX = (star.x - centerX) * (focalLength / star.z);
      pixelX += centerX;
      pixelY = (star.y - centerY) * (focalLength / star.z);
      pixelY += centerY;
      pixelRadius = 1 * (focalLength / star.z);
      uint8_t brightness = star.o * 255;
      auto pixelColor = color(brightness,brightness,brightness);
      engine->FillRect(pixelX, pixelY, pixelRadius, pixelRadius, pixelColor);
    }
  }
  
  std::vector<star> stars;
  
private: 
  IDrzEngine* engine;
  bool warp = false;
  uint16_t starcount=0;
  int focalLength = 0;
  float centerX =0.0f;// (float)engine->GetScreenWidth() / 2;
  float centerY =0.0f;// (float)engine->GetScreenHeight() / 2;
};