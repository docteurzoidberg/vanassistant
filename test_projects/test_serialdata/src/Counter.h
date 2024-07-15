#pragma once
#include "olcPixelGameEngine.h"

//https://pgetinker.com/s/ndGcN-fvm

class Counter {
  public:
    Counter(olc::PixelGameEngine *pge, int digits, olc::Sprite* spritesheet, olc::vi2d digitSpriteSize) : pge(pge), counterDigits(digits), digitSpriteSize(digitSpriteSize), spritesheet(spritesheet) {
      mySprite = new olc::Sprite(digits*8,10);
    }
    void SetCounter(unsigned int value) {
      counterValue=value;
    }
    void Render(int x, int y) {
      //split iCounterValue into multiple digits
      std::vector<int> vecDigits = splitDigits(counterValue);

      //for each digit show sprite (for now)
      for(int iDigitPos=counterDigits-1; iDigitPos>=0; iDigitPos--) {
        int iDigit = 0;
        int iDigitVecIndex = counterDigits - 1 - iDigitPos;
        if(iDigitVecIndex<vecDigits.size())
            iDigit = vecDigits[iDigitVecIndex];

        //TODO: calculate y offset over time
        int screenOffX = 9*(iDigitPos);
        
        int spriteOffY = (9-iDigit)*9; //(9 * (fCurrentIncrement/fIncrementEvery));
        
        //TODO: draw on internal sprite instead
        pge->DrawPartialSprite(screenOffX+x, y, spritesheet, 0, spriteOffY,9,10,1,olc::Sprite::NONE );
      }
  }
  private:
    olc::PixelGameEngine* pge;
    olc::Sprite* spritesheet;
    olc::Sprite* mySprite;
    olc::vi2d digitSpriteSize;
    int counterDigits=0;
    int counterValue=0;

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