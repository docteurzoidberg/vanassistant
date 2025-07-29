#pragma once

#include <IDrzEngine.h>
#include <IDrzSam.h>

#include "DisplayPageManager.h"
#include "DrzGraphics.h"
#include "DrzInputs.h"

#define NUM_STARS 750

#include "pages/common/widgets/Starfield.h" 

#include "pages/assistant/widgets/TextAnimator.h"
#include "pages/assistant/widgets/VerticalTextAnimator.h"
#include "pages/assistant/widgets/Road.h"
#include "pages/assistant/widgets/ScoutScene3d.h"

#include "pages/assistant/AsmText.h"

#include <cstdint>
#include <iostream>

using namespace drz;

class PageAssistant : public DisplayPage {

public:

  PageAssistant(IDrzSam* sam);
  
  PageAssistant();
  void ReadInputs() override;
  ///Load assets, initialize variables, etc
  void Load() override;
  void Activate(DisplayPage* lastPage) override;
  /**
  * Update assistant's logic, data, etc
  *
  * @param elapsedTime time since the last frame in seconds
  */
  void Update(float elapsedTime) override ;

  /**
  * Render the assistant
  */
  void Render() override;
  bool Say(const std::string& text) ;
  void BeforeSayCallback();
  void ToggleDebug();
  void ToggleRenderMode();
  void ToggleFps();
  void DbgNextTriangle();
  void DbgPrevTriangle();

private: 
  IDrzEngine* engine;
  IDrzInputs* inputs;
  IDrzGraphics* gfx;
  IDrzSam* sam;

  AsmText* asmText;
  Road* road;
  Starfield* starfield;
  VerticalTextAnimator* verticalTextAnimator; 
  TextAnimator* textAnimator; 

  WidgetScoutScene3d* sceneScout3d;

  bool bShowFps = true;
  bool bShowDebug = false;

  void DrawTitle();
  void DrawFPS(uint32_t fps);
};
