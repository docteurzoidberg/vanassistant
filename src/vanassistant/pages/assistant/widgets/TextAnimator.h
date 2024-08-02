#pragma once

#include <DrzEngine.h>
#include <DrzGraphics.h>

#include "../../../Widget.h"

#include <queue>

using namespace drz;

class TextAnimator : public Widget {
public:
  TextAnimator(std::string fontname, float typeSpeed, float pauseTime, float cursorBlinkRate, float cursorWidth, float cursorHeight, int x, int y, int width, int height, Color col= WHITE, bool fillBg = true)
    : Widget(x, y, width, height), engine(DrzEngine::Get()), gfx(DrzGraphics::Get()), fontname(fontname), typeSpeed(typeSpeed), pauseTime(pauseTime), cursorBlinkRate(cursorBlinkRate), cursorWidth(cursorWidth), cursorHeight(cursorHeight), col(col), 
      currentIndex(0), isTyping(false), cursorVisible(true), firstMessage(true), displayText(true), fillBg(fillBg) {

    lastUpdate = engine->Now();
    lastCursorBlink = engine->Now();
    lastPauseStart = engine->Now();
  }

  void QueueText(const std::string& text) {
    textQueue.push(text);
  }

  void Load() override {

  }

  void Update(float fElapsedTime) override {
    
    auto now = engine->Now();

    if (firstMessage) {
      StartTyping();
      firstMessage = false;
      return;
    }

    if (isTyping) {
      UpdateTyping(now);
    } else {
      HandlePause(now);
    }

    UpdateCursorBlink(now);
  }

  void Render() override {
    if (displayText) {
      //draw black rectangle behind text
      if(fillBg) {
        gfx->FillRect(0, yOffset-18, gfx->GetScreenWidth() , 24, BLACK);
      }
      std::string toDraw = currentText.substr(0, currentIndex);

      gfx->SetFont(fontname); //TODO: set font
      gfx->DrawText(toDraw, xOffset, yOffset-2, WHITE);
    
      DrawCursor(xOffset, yOffset, toDraw, col);
    }
  }

private:
  IDrzEngine* engine;
  IDrzGraphics* gfx;
  std::string fontname;
  std::queue<std::string> textQueue;
  std::string currentText;
  size_t currentIndex;
  float typeSpeed;
  float pauseTime;
  float cursorBlinkRate; 
  float cursorHeight;
  float cursorWidth;
  uint32_t lastUpdate;
  uint32_t lastCursorBlink;
  uint32_t lastPauseStart;
  bool isTyping;
  bool isPaused;
  bool cursorVisible;
  bool firstMessage;
  bool displayText;
  bool fillBg = true;
  int xOffset;
  int yOffset;
  Color col;

  void StartTyping() {
    currentText = textQueue.front();
    textQueue.pop();
    currentIndex = 0;
    isTyping = true;
    displayText = true;
    lastUpdate = engine->Now();
  }

  void UpdateTyping(uint32_t now) {
    if (now - lastUpdate >= typeSpeed*1000) {
      currentIndex++;
      lastUpdate = now;
      if (currentIndex > currentText.size()) { 
        //current queud text is done typing
        isTyping = false;
        lastPauseStart = now;
      }
    }
  }

  void HandlePause(const uint32_t now) {

    //check time if pause is over
    auto pauseIsDone = (!isTyping) && (now - lastPauseStart >= pauseTime*1000);

    //if not over. return
    //if over, check if there is more text to display

    if(!pauseIsDone) {
      isPaused = true;
      return;
    }

    if (textQueue.empty()) {
      displayText = false;
    } else {
      StartTyping();
    }
  }

  void UpdateCursorBlink(uint32_t now) {
    if (now - lastCursorBlink >= cursorBlinkRate*1000) {
      cursorVisible = !cursorVisible;
      lastCursorBlink = now;
    }
  }

  void DrawCursor(int x, int y, const std::string& toDraw, Color color) {
    if (cursorVisible && displayText) {

      gfx->SetFont(fontname);
      auto textSize = gfx->GetTextBounds(toDraw,x,y);

      //todo: do not use font yAdvance... as font isn't exposed in the engine
      //auto cursorWidth = 8;
      //auto cursorHeight = f->yAdvance - 4;

      int cursorX = x + textSize.w+2;
      int cursorY = y - cursorHeight;

      gfx->FillRect(cursorX, cursorY, cursorWidth, cursorHeight, color); // Draw the cursor as an 8x8 rectangle
    }
  }
};
