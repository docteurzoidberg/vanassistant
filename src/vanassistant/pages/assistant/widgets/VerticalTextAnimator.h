#pragma once

#include <DrzGraphics.h>
#include <DrzEngine.h>

#include <cstdint>
#include <iostream>
#include <queue>
#include <vector>

using namespace drz;

class VerticalTextAnimator {
public:
  VerticalTextAnimator(std::string fontname, uint8_t lineHeight, float typeSpeed, float pauseTime, float cursorBlinkRate, int rectX, int rectY, int rectWidth, int rectHeight, int cursorWidth, int cursorHeight)
    : engine(DrzEngine::Get()), gfx(DrzGraphics::Get()), fontname(fontname), lineHeight(lineHeight), typeSpeed(typeSpeed), pauseTime(pauseTime), cursorBlinkRate(cursorBlinkRate),
      rectX(rectX), rectY(rectY), rectWidth(rectWidth), rectHeight(rectHeight), cursorWidth(cursorWidth), cursorHeight(cursorHeight),
      currentIndex(0), isTyping(false), cursorVisible(true), displayText(true),
      currentLine(0), verticalOffset(0) {
    
    lastUpdate = engine->Now();
    lastCursorBlink = engine->Now();
    lastPauseStart = engine->Now();
  }

  void QueueText(const std::string& text) {
    std::vector<std::string> lines = SplitTextIntoLines(text);
    for (const std::string& line : lines) {
      textQueue.push(line);
    }
  }

  int GetQueueSize() {
    return textQueue.size();
  }

  void Update() {
    //std::cout << "Textanimator Update" << std::endl;
    auto now = engine->Now();
    //std::cout << "isTyping: " << isTyping << " textQueue empty: " << textQueue.empty() << std::endl;
    UpdateCursorBlink(now);
    if (isTyping) {
      UpdateTyping(now);
    } else {
      HandlePause(now);
    }
    
  }

  void DrawText(Color color = WHITE) {
    
    if (!displayText) {
      std::cout << "No text to display" << std::endl;
      return;
    }

    //std::cout << "DrawText" << std::endl;

    int drawY = rectY - verticalOffset;

    gfx->SetFont(fontname);

    for (size_t i = 0; i < drawnLines.size(); ++i) {

      auto y = drawY + lineHeight * (i+1);

      //std::cout << "drawY: " << drawY << " lineHeight: " << lineHeight << " i: " << i << " y: " << y << std::endl;

      //engine->SetCursorPos(rectX,  drawY + lineHeight * (i+1));
      gfx->DrawText(drawnLines[i], rectX, y, color);

      //TODO: remove lines as it scrolls off the screen
    }

    std::string toDraw = currentText.substr(0, currentIndex);
    //engine->SetCursorPos(rectX,  drawY+ lineHeight * drawnLines.size());
    auto y = drawY + lineHeight * (drawnLines.size()+1);
    //std::cout << "drawY: " << drawY << " lineHeight: " << lineHeight  << " y: " << y << std::endl;
    gfx->DrawText(toDraw, rectX, y, color);

    //DrawCursor(rectX,  -(lineHeight * (drawnLines.size()+1)), toDraw, color);
    DrawCursor(rectX,  y-cursorHeight, toDraw, color);
  }

private:
  IDrzEngine* engine;
  IDrzGraphics* gfx;
  std::string fontname;
  std::queue<std::string> textQueue;
  std::string currentText;
  std::vector<std::string> drawnLines;
  size_t currentIndex;
  float typeSpeed;
  float pauseTime;
  float cursorBlinkRate;
  int rectX, rectY, rectWidth, rectHeight;
  uint32_t lineHeight;
  uint32_t lastUpdate;
  uint32_t lastCursorBlink;
  uint32_t lastPauseStart;
  bool isTyping;
  bool cursorVisible;
  bool displayText;
  int currentLine;
  int verticalOffset=0;
  int cursorWidth;
  int cursorHeight;

  void StartTyping() {
    currentText = textQueue.front();
    textQueue.pop();
    currentIndex = 0;
    isTyping = true;
    displayText = true;
    lastUpdate = engine->Now();
  }

  void UpdateTyping(uint32_t now) {
    if (now - lastUpdate >= static_cast<uint32_t>(typeSpeed * 1000)) {
      currentIndex++;
      lastUpdate = now;
      if (currentIndex > currentText.size()) {
        drawnLines.push_back(currentText);
        currentIndex = 0;
        currentLine++;
        isTyping = false;
        lastPauseStart = now;
        if (textQueue.empty()) {
          //displayText = false;
          //drz: should wait last pause time before clearing the text
        }
        
        
        // Scroll logic to remove off-screen lines
        //if (currentLine * lineHeight > rectHeight) {
        if((drawnLines.size()+1) * lineHeight > rectHeight) {
          verticalOffset = ((drawnLines.size()+1) * lineHeight) - rectHeight;
          //if (drawnLines.size() * lineHeight > rectHeight) {
            drawnLines.erase(drawnLines.begin()); // Remove the first line that has scrolled off
            //verticalOffset -= lineHeight; // Adjust the vertical offset accordingly
          //}
        }
        
        //if (currentLine * lineHeight > rectHeight) {
        //  verticalOffset = (currentLine * lineHeight) - rectHeight + lineHeight;
        // }
      }
    }
  }

  void HandlePause(const uint32_t now) {
    if (now - lastPauseStart >= static_cast<uint32_t>(pauseTime * 1000)) {
      if (!textQueue.empty()) {
        StartTyping();
      } else {
        displayText = false;
      }
    } else {
      //draw cursor ?
    }
  }

  void UpdateCursorBlink(uint32_t now) {
    if (now - lastCursorBlink >= static_cast<uint32_t>(cursorBlinkRate * 1000)) {
      cursorVisible = !cursorVisible;
      lastCursorBlink = now;
    }
  }

  void DrawCursor(int x, int y, const std::string& toDraw, Color color) {
    //if (cursorVisible) {
    //if (cursorVisible && (isTyping || (!isTyping && !textQueue.empty()))) {
      //int cursorX = x + toDraw.length() * cursorWidth; // Use cursorWidth
        
      gfx->SetFont(fontname);
      
      auto textSize = gfx->GetTextBounds(toDraw,x,y);


      int cursorX = x + textSize.w+2;
      int cursorY = y;

      //if()

      //std::cout << "cursorX: " << cursorX << " cursorY: " << cursorY << " cursorWidth: " << cursorWidth << " cursorHeight: " << cursorHeight << "toDraw" << toDraw << std::endl;

      gfx->FillRect(cursorX, cursorY, cursorWidth, cursorHeight, color); // Draw the cursor using specified width and height
    //}
  }

  std::vector<std::string> SplitTextIntoLines(const std::string& text) {
    std::vector<std::string> lines;
    size_t pos = 0, prev = 0;

    while ((pos = text.find('\n', prev)) != std::string::npos) {
      lines.push_back(text.substr(prev, pos - prev));
      prev = pos + 1;
    }

    lines.push_back(text.substr(prev));
    return lines;
  }
};
