#pragma once

#include <DrzEngine.h>
#include <DrzGraphics.h>

#include "../../../Widget.h"

#include <cmath>
#include <cstdint>
#include <vector>

using namespace drz;

struct star {
  float x, y, z, o;
};

//TODO: widget do not take screenX and screenY into  account
//TODO: widget should have speed and use Update()
class Starfield : public Widget {
public:
  Starfield(int screenX, int screenY, int width, int height, uint16_t starcount) : Widget(screenX,screenY,width,height), gfx(DrzGraphics::Get()), engine(DrzEngine::Get()), starcount(starcount) {
    for(int i = 0; i < starcount; i++){
      stars.push_back({
        .x = (float)engine->GetRandomFloat() * width,
        .y = (float)engine->GetRandomFloat() * height,
        .z = (float)engine->GetRandomFloat() * width,
        .o = ((float)floor(engine->GetRandomFloat() * 99) + 1) / 10
      });
    }
    centerX = (float)width/ 2;
    centerY = (float)height / 2;
    focalLength = width*2;
  }

  void Load() {
    //TODO
  }

  //update stars with speed and elapsed time since last frame
  void Update(float elapsedTime) {

    totalTime += elapsedTime;
    if(totalTime > 0.0001f*speed){
      totalTime-=0.0001f*speed;
    } else {
      return;
    }
    for(auto &star : stars){
      star.z-= 0.1f;
      if(star.z <= 0){
        star.z = width;
      }
    }
  }

  void Render() {
    int pixelX;
    int pixelY;
    unsigned int pixelRadius;
    for(auto &star : stars){
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
  float centerX =0.0f;
  float centerY =0.0f;
  float totalTime = 0.0f;
  float speed = 1.0f;
};