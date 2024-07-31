#pragma once

#include <IDrzEngine.h>

#include "../sprites/speedmask.h"
#include "../sprites/j7sprite.h"

//https://pgetinker.com/s/0n49K0w0p4c

class RoadWidget : public Widget {
  public:

    RoadWidget(int screenX, int screenY, int width, int heigth) : Widget(screenX, screenY, width, heigth), roadOffset(0.0), eyePos({0, 0, 400}) {
      speedValueBarHeight = ((float)speed/(float)speedMax) * speedValueBarMaxHeight;
      eyePos.x = width / 2.0f;
      eyePos.y = height / 2.0f;

      // calculate grid offset to center the grid in the widget
      gridOffsetX = (width - roadWidth * roadGridSize) / 2.0f;
    }

    void SetSpeed(int speed) {
      if(speed<0) speed=0;
      this->speed = speed;
    }

    void Load() override {
      speedMask = engine->CreateSpriteFromData(SPEEDMASK_SPRITE_DATA, SPEEDMASK_SPRITE_WIDTH, SPEEDMASK_SPRITE_HEIGHT);
      j7Sprite = engine->CreateSpriteFromData(J7SPRITE_SPRITE_DATA, J7SPRITE_SPRITE_WIDTH, J7SPRITE_SPRITE_HEIGHT);
    }

    void Update(float elapsedTime) override {
      speedValueBarHeight = ((float)speed/(float)speedMax) * speedValueBarMaxHeight;

      //road
      auto roadSpeed = speed * 2.0f; 
      roadOffset += roadSpeed * elapsedTime;
      if (roadOffset >= roadGridSize) {
        roadOffset -= roadGridSize;
      }
    }

    void SetFOV(float fov) {
      this->fov = fov;
      eyePos.z = fov;
    }

    void SetPitch(float pitch) {
      this->pitch = pitch;
    }

    void Render() override {
      
      //background
      engine->FillRect(screenX, screenY, width, height, BLACK);

      //draw grid road

      engine->SetPaintMode(color::ALPHA);

      // draw vertical lines, no speed offset
      for (int i = 0; i <= roadWidth; ++i) {
        float x = gridOffsetX + i * roadGridSize;
        //if first or last column draw speed indicator lines
        bool isSpeedIndicator = (i==0) || (i==(roadWidth-1));
        if(isSpeedIndicator) {
          //fill column with vertical lines proportionnal to the speed value
          auto speedLineHeight = ((float)speed/(float)speedMax) * roadHeight * roadGridSize;
          for(int s=1; s <= roadGridSize+-1; s++) {
            drawLine3D(x+s, 0, 0, x+s, 0, speedLineHeight, color(255,255,255,64));
          }
        }
        drawLine3D(x, 0, 0, x, 0, roadHeight * roadGridSize, color(255, 255, 255, 64));
      }

      // draw horizontal lines with offset based on speed to achieve a scrolling road effect
      for (int j = -1; j <= roadHeight*2; ++j) {
        float y = j * roadGridSize - roadOffset;
        if (y >= -roadGridSize && y <= roadHeight * roadGridSize) { // Only draw lines within the visible area
          drawLine3D(gridOffsetX, 0, y, gridOffsetX + roadWidth * roadGridSize, 0, y, color(255, 255, 255, 64));
        }
      }

      engine->SetPaintMode(color::NORMAL);


      //clear speed value bar area
      //engine->FillRect(screenX+speedValueBarDrawStartX, screenY+speedValueBarDrawStartY, speedValueBarWidth, speedValueBarMaxHeight, BLACK);

      //draw speed bar
      //auto endValueDrawY = speedValueBarMaxHeight - speedValueBarHeight;
      //engine->FillRect(screenX + speedValueBarDrawStartX, screenY + speedValueBarDrawStartY + endValueDrawY, speedValueBarWidth, speedValueBarHeight, WHITE);
      

      //speed mask
      //engine->DrawMaskSprite(speedMask, screenX, screenY);

      //j7 sprite
      engine->DrawMaskSprite(j7Sprite, screenX + J7offsetX, screenY + J7offsetY);
    }

  private:
    Sprite* speedMask; 
    Sprite* j7Sprite;

    const int speedMax = 110;
    const int speedValueBarMaxHeight=159;
    const int speedValueBarWidth =151;
    const int speedValueBarDrawStartY=1;
    const int speedValueBarDrawStartX=4;

    const int J7offsetX = 52;
    const int J7offsetY = 71;

    const int roadWidth = 5; //in grid cell
    const int roadHeight = 100; //in grid cell
    const int roadGridSize = 50; //in pixel

    float gridOffsetX; //for centering the grid horizontally in the widget
  
    int speed = 0;
    int speedValueBarHeight; 
    float roadOffset   = 0.0f;

    vec3d eyePos;
    float fov;
    float pitch = -0.12f;

    vec2d get3DCoords(float x, float y, float z) {
      float perspective = eyePos.z / (eyePos.z + z);
      float screenXCoord = screenX + (width / 2.0f) + (x - eyePos.x) * perspective;
      
      // Apply pitch to the y-coordinate
      float adjustedY = (y - eyePos.y) * cos(pitch) - z * sin(pitch);
      float screenYCoord = screenY + (height / 2.0f) - adjustedY * perspective;

      return { screenXCoord, screenYCoord };
    }

    void drawLine3D(float x1, float y1, float z1, float x2, float y2, float z2, color lineColor) {
      vec2d p1 = get3DCoords(x1, y1, z1);
      vec2d p2 = get3DCoords(x2, y2, z2);
      //if ((p1.x >= screenX && p1.x <= screenX + width && p1.y >= screenY && p1.y <= screenY + height) ||
      //    (p2.x >= screenX && p2.x <= screenX + width && p2.y >= screenY && p2.y <= screenY + height)) {
        engine->DrawLine(p1.x, p1.y, p2.x, p2.y, lineColor);
      //}
    }
};