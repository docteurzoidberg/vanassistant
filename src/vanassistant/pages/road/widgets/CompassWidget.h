#pragma once

#include <DrzGraphics.h>

#include "../../../Widget.h"

#include "../sprites/compassbar.h"
#include "../sprites/compassarrow.h"
#include "../sprites/compassdir.h"

//https://pgetinker.com/s/xShtLdEVCjH

using namespace drz;

class CompassWidget : public Widget {
  public:

    CompassWidget(int screenX, int screenY, int width, int heigth) : Widget(screenX, screenY, width, heigth) {    
      compassBarSprite = new Sprite(COMPASSBAR_SPRITE_WIDTH, COMPASSBAR_SPRITE_HEIGHT, COMPASSBAR_SPRITE_DATA);
      compassArrowSpriteSheet = new Sprite( COMPASSARROW_SPRITE_WIDTH, COMPASSARROW_SPRITE_HEIGHT, COMPASSARROW_SPRITE_DATA);
      compassDirSpriteSheet = new Sprite(COMPASSDIR_SPRITE_WIDTH, COMPASSDIR_SPRITE_HEIGHT, COMPASSDIR_SPRITE_DATA);
    }

    float GetHeading() {
      return trueHeading;
    }

    void SetHeading(float heading) {
      this->trueHeading = heading;
      this->heading = trueHeading + 360.0f;
    }

    void Load() override {
      this->SetHeading(0);
    }

    void Update(float elapsedTime) override {
      //heading+=elapsedTime*15.0f;
      if(heading>720.0f) heading-=360.0f;
      if(heading<360.0f) heading+=360.0f;
    }

    void Render() override {
      //compass bar background
      gfx->FillRect(screenX+compassBarWidgetX, screenY+compassBarWidgetY, compassBarWidth, compassBarHeight, BLACK);
      //compass bar
      DrawCompassBar();
      //compass arrow
      DrawArrow();
      //compass direction text
      DrawDirection();
      //compass heading
      DrawHeadingText();
      //Rect around compass
      gfx->DrawRect(screenX+compassBarWidgetX-3, screenY+compassBarWidgetY-2, compassBarWidth+3, compassBarHeight+2, BLACK);
      gfx->DrawRect(screenX+compassBarWidgetX-2, screenY+compassBarWidgetY-1, compassBarWidth+1, compassBarHeight, WHITE);
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
      gfx->DrawPartialSprite({screenX+compassArrowWidgetX, screenY+compassArrowWidgetY}, compassArrowSpriteSheet, spriteSheetPos, compassArrowSize);
    }

    void DrawCompassBar() {
      compassBarSprite->SetSampleMode(Sprite::Mode::PERIODIC);
      int nCompassOffset = (heading/360.0f)*(float)compassBarSprite->width - (((float)compassBarWidth)/2.0f);
      vi2d displaySize{compassBarWidth+2,compassBarSprite->height};
      gfx->DrawPartialSprite({compassBarWidgetX+screenX-2, compassBarWidgetY+screenY + ((compassBarHeight-compassBarSprite->height)/2) },compassBarSprite,{nCompassOffset,0},displaySize);
    }

    void DrawDirection() {
      int dirIndex = (int)(((heading-360.0f)/360.0f)*8);
      vi2d spriteSheetPos = {dirIndex*compassDirSize.x,0};
      gfx->DrawPartialSprite({screenX+compassDirWidgetX, screenY+compassDirWidgetY}, compassDirSpriteSheet, spriteSheetPos, compassDirSize);
    }

    void DrawHeadingText() {
      gfx->SetFont("solidmono8");
      auto text = std::to_string((int)(heading-360.0f));
      auto textBounds = gfx->GetTextBounds(text, 0, 0);
      gfx->DrawText(text, screenX + compassHeadingWidgetX - (textBounds.w/2) , screenY + compassHeadingWidgetY + textBounds.h, WHITE);

      //draw "degree" symbol as 3x3 outline square
      gfx->DrawRect(screenX + compassHeadingWidgetX + textBounds.w/2 + 3, screenY + compassHeadingWidgetY - 1, 3, 3, WHITE);
    }
};