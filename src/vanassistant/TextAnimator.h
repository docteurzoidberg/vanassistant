#pragma once

#include "../../include/IDrzEngine.h"
#include <memory>
#include <queue>

class TextAnimator {
public:
  TextAnimator(IDrzEngine* engine, const font* f, float typeSpeed, float pauseTime, float cursorBlinkRate, bool fillBg = true)
    : engine(engine), typeSpeed(typeSpeed), pauseTime(pauseTime), cursorBlinkRate(cursorBlinkRate),
      currentIndex(0), isTyping(false), cursorVisible(true), firstMessage(true), displayText(true), fillBg(fillBg) {

    this->f = f;
    lastUpdate = engine->Now();
    lastCursorBlink = engine->Now();
    lastPauseStart = engine->Now();
  }

  void QueueText(const std::string& text) {
    textQueue.push(text);
  }

  void Update() {
    auto now = engine->Now();

    if (isTyping) {
      UpdateTyping(now);
    } else {
      HandlePause(now);
    }

    UpdateCursorBlink(now);
  }

  void DrawText(int x, int y, color color = WHITE) {
    if (displayText) {
      //draw black rectangle behind text
      if(fillBg) {
        engine->FillRect(0, y-18, engine->GetScreenWidth() , 24, BLACK);
      }
      std::string toDraw = currentText.substr(0, currentIndex);
		  //TODO 
      engine->SetFont(f); //TODO: set font
      engine->DrawText(toDraw, x, y-2, WHITE);
    
      DrawCursor(x, y, toDraw, color);
    }
  }

  const font* GetFont() {
    return f;
  }

private:
  IDrzEngine* engine;
  const font* f;
  std::queue<std::string> textQueue;
  std::string currentText;
  size_t currentIndex;
  float typeSpeed;
  float pauseTime;
  float cursorBlinkRate;
  uint32_t lastUpdate;
  uint32_t lastCursorBlink;
  uint32_t lastPauseStart;
  bool isTyping;
  bool cursorVisible;
  bool firstMessage;
  bool displayText;
  bool fillBg = true;

  void StartTyping() {
    currentText = textQueue.front();
    textQueue.pop();
    currentIndex = 0;
    isTyping = true;
    lastUpdate = engine->Now();
  }

  void UpdateTyping(uint32_t now) {
    if (now - lastUpdate >= typeSpeed*1000) {
      currentIndex++;
      lastUpdate = now;
      if (currentIndex > currentText.size()) {
        isTyping = false;
        lastPauseStart = now;
        if (textQueue.empty()) {
          displayText = false;
        }
      }
    }
  }

  void HandlePause(const uint32_t now) {
    if (firstMessage || now - lastUpdate >= pauseTime*1000) {
      if (!textQueue.empty()) {
        StartTyping();
        firstMessage = false;
      }
    }

    if (textQueue.empty() && now - lastPauseStart >= pauseTime*1000) {
      displayText = false;
    }
  }

  void UpdateCursorBlink(uint32_t now) {
    if (now - lastCursorBlink >= cursorBlinkRate*1000) {
      cursorVisible = !cursorVisible;
      lastCursorBlink = now;
    }
  }

  void DrawCursor(int x, int y, const std::string& toDraw, color color) {
    if (cursorVisible && (isTyping || (!isTyping && !textQueue.empty()))) {

      engine->SetFont(f);
      auto textSize = engine->GetTextBounds(toDraw,x,y);

      auto cursorWidth = 8;
      auto cursorHeight = f->yAdvance - 4;

      int cursorX = x + textSize.w+2;
      int cursorY = y - f->yAdvance +6;
     
      engine->FillRect(cursorX, cursorY, cursorWidth, cursorHeight, color); // Draw the cursor as an 8x8 rectangle
    }
  }
};
