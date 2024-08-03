#pragma once

#include <DrzGraphics.h>

#include "../../../Widget.h"

#include "../sprites/distanceback.h"

using namespace drz;

class DistanceWidget : public Widget { 
  
  public:
    DistanceWidget(int screenX, int screenY, int width, int heigth) : Widget(screenX, screenY, width, heigth) {
    }

    void SetTotalDistance(int distance) {
      this->totalDistance = distance;
    }

    void SetTripDistance(float distance) {
      this->tripDistance = distance;
    }

    void Load() override {
      distanceBackSprite = new Sprite(DISTANCEBACK_SPRITE_WIDTH, DISTANCEBACK_SPRITE_HEIGHT, DISTANCEBACK_SPRITE_DATA);
    }

    void Update(float elapsedTime) override {
      //TODO
    }

    void Render() override {
      //distance back
      gfx->DrawSprite(screenX, screenY, distanceBackSprite);
    }

  private: 
    int totalDistance=0;
    float tripDistance=0.0f;
    Sprite* distanceBackSprite;
};