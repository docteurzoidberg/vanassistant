#pragma once

#include "../../include/IDrzEngine.h"
#include <memory>
#include <queue>

class TextAnimator {
public:
  TextAnimator(IDrzEngine* engine, float typeSpeed, float pauseTime, float cursorBlinkRate)
    : engine(engine), typeSpeed(typeSpeed), pauseTime(pauseTime), cursorBlinkRate(cursorBlinkRate),
      currentIndex(0), isTyping(false), cursorVisible(true), firstMessage(true), displayText(true) {

    //Todo: load font
    //font = std::make_unique<olc::Font>( "./sprites/test3d/font_monocode_14.png");

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
      std::string toDraw = currentText.substr(0, currentIndex);
		  //TODO
      //font->DrawStringPropDecal( {(float)x,(float)y}, toDraw, olc::WHITE, {1.0f, 1.0f} );
      //pge->DrawString(x, y, toDraw, color);
      DrawCursor(x, y, toDraw, color);
    }
  }

private:
  IDrzEngine* engine;
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

  void StartTyping() {
    currentText = textQueue.front();
    textQueue.pop();
    currentIndex = 0;
    isTyping = true;
    lastUpdate = engine->Now();
  }

  void UpdateTyping(uint32_t now) {
    if (now - lastUpdate >= typeSpeed) {
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
    if (firstMessage || now - lastUpdate >= pauseTime) {
      if (!textQueue.empty()) {
        StartTyping();
        firstMessage = false;
      }
    }

    if (textQueue.empty() && now - lastPauseStart >= pauseTime) {
      displayText = false;
    }
  }

  void UpdateCursorBlink(uint32_t now) {
    if (now - lastCursorBlink >= cursorBlinkRate) {
      cursorVisible = !cursorVisible;
      lastCursorBlink = now;
    }
  }

  void DrawCursor(int x, int y, const std::string& toDraw, color color) {
    if (cursorVisible && (isTyping || (!isTyping && !textQueue.empty()))) {
      int cursorX = x + toDraw.length() * 8; // Assuming each character is 8 pixels wide
      engine->FillRect(cursorX, y, 8, 14, color); // Draw the cursor as an 8x8 rectangle
    }
  }
};
