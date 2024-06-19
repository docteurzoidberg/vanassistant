#pragma once

#include <IDrzEngine.h>

#include <queue>

using namespace drz;

class TextAnimator {
public:
  TextAnimator(IDrzEngine* engine, std::string fontname, float typeSpeed, float pauseTime, float cursorBlinkRate, float cursorWidth, float cursorHeight, int xOffset=0, int yOffset=0, color col= WHITE, bool fillBg = true)
    : engine(engine), fontname(fontname), typeSpeed(typeSpeed), pauseTime(pauseTime), cursorBlinkRate(cursorBlinkRate), cursorWidth(cursorWidth), cursorHeight(cursorHeight), col(col), xOffset(xOffset), yOffset(yOffset),
      currentIndex(0), isTyping(false), cursorVisible(true), firstMessage(true), displayText(true), fillBg(fillBg) {

    lastUpdate = engine->Now();
    lastCursorBlink = engine->Now();
    lastPauseStart = engine->Now();
  }

  void QueueText(const std::string& text) {
    //is first message in queue?
    //if(textQueue.empty()) {
    //  firstMessage = true;
      //Update();
    //}
    textQueue.push(text);
  }

  void Update() {
    
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

  void Render() {
    if (displayText) {
      //draw black rectangle behind text
      if(fillBg) {
        engine->FillRect(0, yOffset-18, engine->GetScreenWidth() , 24, BLACK);
      }
      std::string toDraw = currentText.substr(0, currentIndex);
		  //TODO 
      engine->SetFont(fontname); //TODO: set font
      engine->DrawText(toDraw, xOffset, yOffset-2, WHITE);
    
      DrawCursor(xOffset, yOffset, toDraw, col);
    }
  }

private:
  IDrzEngine* engine;
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
  color col;

  void StartTyping() {
    currentText = textQueue.front();
    textQueue.pop();
    currentIndex = 0;
    isTyping = true;
    //firstMessage = true;
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

  void DrawCursor(int x, int y, const std::string& toDraw, color color) {
    if (cursorVisible && displayText) {

      engine->SetFont(fontname);
      auto textSize = engine->GetTextBounds(toDraw,x,y);

      //todo: do not use font yAdvance... as font isn't exposed in the engine
      //auto cursorWidth = 8;
      //auto cursorHeight = f->yAdvance - 4;

      int cursorX = x + textSize.w+2;
      int cursorY = y - cursorHeight;
     
      engine->FillRect(cursorX, cursorY, cursorWidth, cursorHeight, color); // Draw the cursor as an 8x8 rectangle
    }
  }
};
