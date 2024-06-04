#pragma once

#include "../../include/IDrzEngine.h"

#include <cmath>
#include <vector>

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
  }

  void Update(float elapsedTime) {
    //TODO
  }

  void Render() {
    //TODO
  }
  
  std::vector<star> stars;
  
private: 
  IDrzEngine* engine;
  bool warp = false;
  uint16_t starcount=0;
  int focalLength = engine->GetScreenWidth()*2;
  float centerX = (float)engine->GetScreenWidth() / 2;
  float centerY = (float)engine->GetScreenHeight() / 2;
};