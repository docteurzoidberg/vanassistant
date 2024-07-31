#pragma once

#include <IDrzEngine.h>

#include "../sprites/compassbar.h"
#include "../sprites/compassarrow.h"
#include "../sprites/compassdir.h"

//https://pgetinker.com/s/xShtLdEVCjH

class CompassWidget : public Widget {
  public:

    CompassWidget(int screenX, int screenY, int width, int heigth) : Widget(screenX, screenY, width, heigth) {

    }

    void SetHeading(float heading) {
      this->trueHeading = heading;
      this->heading = trueHeading + 360.0f;
    }

    void Load() override {
      compassBarSprite = engine->CreateSpriteFromData(COMPASSBAR_SPRITE_DATA, COMPASSBAR_SPRITE_WIDTH, COMPASSBAR_SPRITE_HEIGHT);
      compassArrowSpriteSheet = engine->CreateSpriteFromData(COMPASSARROW_SPRITE_DATA, COMPASSARROW_SPRITE_WIDTH, COMPASSARROW_SPRITE_HEIGHT);
      compassDirSpriteSheet = engine->CreateSpriteFromData(COMPASSDIR_SPRITE_DATA, COMPASSDIR_SPRITE_WIDTH, COMPASSDIR_SPRITE_HEIGHT);
    }

    void Update(float elapsedTime) override {
      heading+=elapsedTime*15.0f;
      if(heading>720.0f) heading-=360.0f;
      if(heading<=360.0f) heading+=360.0f;
    }

    void Render() override {
      //compass bar background
      engine->FillRect(screenX+compassBarWidgetX, screenY+compassBarWidgetY, compassBarWidth, compassBarHeight, BLACK);
      //compass bar
      DrawCompassBar();
      //compass arrow
      DrawArrow();
      //compass direction text
      DrawDirection();
      //compass heading
      DrawHeadingText();
      //Rect around compass
      engine->DrawRect(screenX+compassBarWidgetX-3, screenY+compassBarWidgetY-2, compassBarWidth+3, compassBarHeight+2, BLACK);
      engine->DrawRect(screenX+compassBarWidgetX-2, screenY+compassBarWidgetY-1, compassBarWidth+1, compassBarHeight, WHITE);
    }

  private:

    Sprite* compassBarSprite;
    Sprite* compassArrowSpriteSheet;
    Sprite* compassDirSpriteSheet;

    float heading = 360.0f;
    float trueHeading = 0.0f;

    //compass bar position relative to the widget
    const int compassBarWidgetX = 5;
    const int compassBarWidgetY = 26;
    const int compassBarWidth = 71;
    const int compassBarHeight = 24;
    //compass arrow 
    const int compassArrowWidgetX = 0;
    const int compassArrowWidgetY = 5;
    const vi2d compassArrowSize = {24,18};
    //compass direction
    const int compassDirWidgetX = 60;
    const int compassDirWidgetY = 5;
    const vi2d compassDirSize = {20,14};
    //compass heading text
    const int compassHeadingWidgetX = 40;
    const int compassHeadingWidgetY = 5;

    void DrawArrow() {
      int spriteIndex = (int)(((heading-360.0f)/360.0f)*16);
      vi2d spriteSheetPos = {spriteIndex*compassArrowSize.x,0};
      engine->DrawPartialSprite({screenX+compassArrowWidgetX, screenY+compassArrowWidgetY}, compassArrowSpriteSheet, spriteSheetPos, compassArrowSize);
    }

    void DrawCompassBar() {
      compassBarSprite->SetSampleMode(Sprite::Mode::PERIODIC);
      int nCompassOffset = (heading/360.0f)*(float)compassBarSprite->width - (((float)compassBarWidth)/2.0f);
      vi2d displaySize{compassBarWidth+2,compassBarSprite->height};
      engine->DrawPartialSprite({compassBarWidgetX+screenX-2, compassBarWidgetY+screenY + ((compassBarHeight-compassBarSprite->height)/2) },compassBarSprite,{nCompassOffset,0},displaySize);
    }

    void DrawDirection() {
      int dirIndex = (int)(((heading-360.0f)/360.0f)*8);
      vi2d spriteSheetPos = {dirIndex*compassDirSize.x,0};
      engine->DrawPartialSprite({screenX+compassDirWidgetX, screenY+compassDirWidgetY}, compassDirSpriteSheet, spriteSheetPos, compassDirSize);
    }

    void DrawHeadingText() {
      engine->SetFont("solidmono8");
      auto text = std::to_string((int)(heading-360.0f));
      auto textBounds = engine->GetTextBounds(text, 0, 0);
      engine->DrawText(text, screenX + compassHeadingWidgetX - (textBounds.w/2) , screenY + compassHeadingWidgetY + textBounds.h, WHITE);
    }
};