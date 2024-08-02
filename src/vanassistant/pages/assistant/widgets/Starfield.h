#pragma once

#include <IDrzEngine.h>
#include <DrzGraphics.h>

#include <cmath>
#include <cstdint>
#include <vector>

using namespace drz;

struct star {
  float x, y, z, o;
};

class Starfield {
public:
  Starfield(uint16_t starcount) : gfx(DrzGraphics::Get()), starcount(starcount) {
    for(int i = 0; i < starcount; i++){
      stars.push_back({
        .x = (float)engine->GetRandomFloat() * gfx->GetScreenWidth(),
        .y = (float)engine->GetRandomFloat() * gfx->GetScreenHeight(),
        .z = (float)engine->GetRandomFloat() * gfx->GetScreenWidth(),
        .o = ((float)floor(engine->GetRandomFloat() * 99) + 1) / 10
      });
    }
    centerX = (float)gfx->GetScreenWidth() / 2;
    centerY = (float)gfx->GetScreenHeight() / 2;
    focalLength = gfx->GetScreenWidth()*2;
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
        star.z = gfx->GetScreenWidth();
      }
      pixelX = (star.x - centerX) * (focalLength / star.z);
      pixelX += centerX;
      pixelY = (star.y - centerY) * (focalLength / star.z);
      pixelY += centerY;
      pixelRadius = 1 * (focalLength / star.z);
      uint8_t brightness = star.o * 255;
      auto pixelColor = Color(brightness,brightness,brightness);
      gfx->FillRect(pixelX, pixelY, pixelRadius, pixelRadius, pixelColor);
    }
  }
  
  std::vector<star> stars;
  
private:  
  IDrzEngine* engine;
  IDrzGraphics* gfx;
  bool warp = false;
  uint16_t starcount=0;
  int focalLength = 0;
  float centerX =0.0f;// (float)engine->GetScreenWidth() / 2;
  float centerY =0.0f;// (float)engine->GetScreenHeight() / 2;
};