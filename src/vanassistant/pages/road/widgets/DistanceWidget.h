#pragma once

#include <DrzGraphics.h>
#include <vector>

#include "../../../Widget.h"

#include "../sprites/distanceback2.h"
#include "../sprites/counterdigits.h"
#include "../sprites/counterdigitmask.h"

//https://pgetinker.com/s/U8LkTZqfs-M

//counter (latest version)
//https://pgetinker.com/s/AlZV-7F7Qqv

using namespace drz;

struct AnimDigit {
    bool isAnimating=true;
    float animProgress=0.0f;
    int digitOffsetY=0;
    int currentDigit=0;
};

struct DigitSettings { 
  int spriteSheetColumn = 0;
  Color bgColor = BLACK;
  bool drawBackground = true;
  int marginLeft = 1;
};

class AnimatedDigitalCounter {
  public:
    AnimatedDigitalCounter(IDrzGraphics *gfx, int digits, Sprite* spritedigits, Sprite* spritedigitmask, vi2d digitSpriteSize, std::vector<DigitSettings> digitsSettings={}) : gfx(gfx), counterDigits(digits),  spriteDigits(spritedigits), spriteDigitMask(spritedigitmask), digitSpriteSize(digitSpriteSize), digitsSettings(digitsSettings) {
    }

    void SetCounter(unsigned int value) {

      int oldValue = counterTargetValue;
      int newValue = value;

      //check changed digits...
      auto oldValueDigits = splitDigits(oldValue);
      auto newValueDigits = splitDigits(newValue);

      //for each digits in new value vector, compare to old one
      for(int i=0;i<newValueDigits.size();i++) {
        int oldDigit=0;
        if(i<oldValueDigits.size()) {
          oldDigit = oldValueDigits.at(i);
        }
        int newDigit = newValueDigits.at(i);
        if(oldDigit!=newDigit) {
          animDigits.push_back({  
              true,
              0,
              0,
              i
          });
        }
      }
      oldValue = newValue;
      
      counterTargetValue = newValue;
    }

    void Update(float elapsedTime) { 
      
      //for each digit in the animation queue 
      std::vector<AnimDigit>::iterator animDigit;
      for(animDigit=animDigits.begin();animDigit!=animDigits.end(); animDigit++) {
        animDigit->animProgress+=elapsedTime; 
        if(animDigit->animProgress>animationSpeed) {
          animDigit->animProgress=0;
          animDigit->digitOffsetY=digitSpriteSize.y;
          animDigit->isAnimating = false;
          counterValue = counterTargetValue;
        } 
        animDigit->digitOffsetY = (animDigit->animProgress/animationSpeed) * (float)digitSpriteSize.y;
        if(animDigit->digitOffsetY >digitSpriteSize.y) {
          animDigit->digitOffsetY = digitSpriteSize.y;
          animDigit->isAnimating = false;
          counterValue = counterTargetValue;
        }
      }

      //erase all non animating digits
      animDigits.erase(std::remove_if(animDigits.begin(), animDigits.end(),[](const AnimDigit &p){
        return p.isAnimating == false;
      }), animDigits.end()); 

      //if animations are done, set counter value to target value
      if(animDigits.size() == 0) {
          counterValue = counterTargetValue;
      }
    }

    void Render(int x=0, int y=0) {

      //split iCounterValue into multiple digits
      std::vector<int> vecDigits = splitDigits(counterValue);
      
      //for each digit
      for(int iDigitPos=counterDigits-1; iDigitPos>=0; iDigitPos--) {
        int iDigit = 0;
        DigitSettings currentDigitSettings;

        int iDigitVecIndex = counterDigits - 1 - iDigitPos;
        if(iDigitVecIndex<vecDigits.size()) {
          iDigit = vecDigits[iDigitVecIndex]; 
        }
        
        if(digitsSettings.size()>iDigitVecIndex) {
          currentDigitSettings = digitsSettings.at(iDigitVecIndex);
        }
    
        int screenOffX = ((digitSpriteSize.x+1)*iDigitPos) + (currentDigitSettings.marginLeft);
        int spriteOffY = 180 + (9-iDigit)*digitSpriteSize.y;

        for(auto animDigit:animDigits) {
          if(animDigit.currentDigit == iDigitVecIndex) {
            spriteOffY-=animDigit.digitOffsetY;
          }
        }

        //draw digit background if exists
        //if(digitsBgColors.size()>iDigitVecIndex) { 
        if(currentDigitSettings.drawBackground) {
          gfx->FillRect(x+screenOffX, y, digitSpriteSize.x, digitSpriteSize.y, currentDigitSettings.bgColor);
        }

        gfx->SetPaintMode(Mode::ALPHA);
        gfx->DrawPartialSprite(screenOffX+x, y, spriteDigits, (currentDigitSettings.spriteSheetColumn*digitSpriteSize.x), spriteOffY,10,18);

        //draw digit mask
        
        gfx->DrawPartialSprite(screenOffX+x, y, spriteDigitMask, (currentDigitSettings.spriteSheetColumn*digitSpriteSize.x),0, 10,18);
        gfx->SetPaintMode(Mode::NORMAL);
      }
    }
  
  private:
    IDrzGraphics* gfx;
    Sprite* spriteDigitMask;
    Sprite* spriteDigits;

    std::vector<DigitSettings> digitsSettings={};

    vi2d digitSpriteSize;
    int counterDigits=0;
    int counterValue=0; //counter current value
    int counterTargetValue=0; //counter target value

    float animationSpeed = 0.3f;    //times it take to go from one digit to another

    std::vector<AnimDigit> animDigits;

    std::vector<int> splitDigits(int number) {
      std::vector<int> digits;
      while (number > 0) {
        int digit = number % 10; // Get the last digit
        digits.push_back(digit); // Store the digit
        number /= 10; // Remove the last digit
      }
      return digits;
    }
};

class DistanceWidget : public Widget { 
  
  public:
    DistanceWidget(int screenX, int screenY, int width, int heigth) : Widget(screenX, screenY, width, heigth) { 
      distanceBackSprite = new Sprite(DISTANCEBACK2_SPRITE_WIDTH, DISTANCEBACK2_SPRITE_HEIGHT, DISTANCEBACK2_SPRITE_DATA);
      counterSpritesheet = new Sprite(COUNTERDIGITS_SPRITE_WIDTH, COUNTERDIGITS_SPRITE_HEIGHT, COUNTERDIGITS_SPRITE_DATA);
      counterDigitMask = new Sprite(COUNTERDIGITMASK_SPRITE_WIDTH, COUNTERDIGITMASK_SPRITE_HEIGHT, COUNTERDIGITMASK_SPRITE_DATA);

      counterSpritesheet->SetSampleMode(Sprite::Mode::PERIODIC);
      
      counterTotal = new AnimatedDigitalCounter(gfx, 6, counterSpritesheet, counterDigitMask, {10,18}, {
        {.spriteSheetColumn=0, .bgColor=BLACK},
        {.spriteSheetColumn=0, .bgColor=BLACK},
        {.spriteSheetColumn=0, .bgColor=BLACK},
        {.spriteSheetColumn=0, .bgColor=BLACK},
        {.spriteSheetColumn=0, .bgColor=BLACK},
        {.spriteSheetColumn=0, .bgColor=BLACK}
      });
      //counterTotal = new AnimatedDigitalCounter(gfx, 6, counterSpritesheet, counterDigitMask, {10,18}, {0,1,1,1,1,1}, {BLACK, WHITE, WHITE, WHITE, WHITE, WHITE});
      counterTotal->SetCounter(totalDistance);
      
      //counterTrip= new AnimatedDigitalCounter(gfx, 4, counterSpritesheet, counterDigitMask, {10,18},{1,0,0,0}, {WHITE, BLACK, BLACK, BLACK});
      counterTrip= new AnimatedDigitalCounter(gfx, 5, counterSpritesheet, counterDigitMask, {10,18}, {
        {.spriteSheetColumn=0, .bgColor=RED, .drawBackground=true, .marginLeft=4},
        {.spriteSheetColumn=0, .bgColor=BLACK, .drawBackground=false },
        {.spriteSheetColumn=0, .bgColor=BLACK, .drawBackground=false },
        {.spriteSheetColumn=0, .bgColor=BLACK, .drawBackground=false },
        {.spriteSheetColumn=0, .bgColor=BLACK, .drawBackground=false }
      });
      counterTrip->SetCounter(tripDistance*10);
    }

    void SetTotalDistance(int distance) {
      totalDistance = distance;
      counterTotal->SetCounter(totalDistance);
    }

    void SetTripDistance(float distance) {
      tripDistance = distance;
      counterTrip->SetCounter(tripDistance*10);
    }

    float GetTripDistance() {
      return tripDistance;
    }

    int GetTotalDistance() {
      return totalDistance;
    }

    void Load() override {
    }

    void Update(float elapsedTime) override {
      /*
      fCurrentIncrement += elapsedTime;
      if(fCurrentIncrement >= fIncrementEvery) {
        fCurrentIncrement = 0.0f;
        tripDistance += 0.1f;
        totalDistance += 1;
        counterTrip->SetCounter(tripDistance*10);
        counterTotal->SetCounter(totalDistance);
      }
      */
      //TODO
      counterTotal->Update(elapsedTime);
      counterTrip->Update(elapsedTime);
    }

    void Render() override {

      //gfx->FillRect(screenX, screenY, width, height, Color(0,255,255));
      //distance back
      gfx->DrawSprite(screenX, screenY, distanceBackSprite);
      
      counterTotal->Render(screenX+20,screenY+2);
      counterTrip->Render(screenX+20+7, screenY+23);
    }

  private: 
    int totalDistance=123456;
    float tripDistance=234.5f;
    Sprite* distanceBackSprite;
    Sprite* counterSpritesheet;
    Sprite* counterDigitMask;
    AnimatedDigitalCounter* counterTotal;
    AnimatedDigitalCounter* counterTrip;

    float fIncrementEvery = 0.5f;
    float fCurrentIncrement = 0.0f;
};