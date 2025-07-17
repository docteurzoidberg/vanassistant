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

  PageAssistant(IDrzSam* sam) : sam(sam), DisplayPage("assistant") {
    gfx = DrzGraphics::Get();
    engine = DrzEngine::Get();
    inputs = DrzInputs::Get();
  }
  
  PageAssistant() : DisplayPage("assistant") {
    gfx = DrzGraphics::Get();
    engine = DrzEngine::Get();
    inputs = DrzInputs::Get();
  }

  void ReadInputs() override{

    if(
      inputs->GetKey(Key::K1).isPressed || 
      inputs->GetKey(Key::K2).isPressed || 
      inputs->GetKey(Key::K3).isPressed || 
      inputs->GetKey(Key::K4).isPressed
    ) {
      //PageMenu* pageMenu = (PageMenu*) DisplayPageManager::GetPage("menu");
      //pageMenu->StartTimer();
      DisplayPageManager::GoToPage("menu");
    }

    //R: toggle render mode
    if(inputs->GetKey(drz::Key::R).isPressed) {
      ToggleRenderMode();
    }

    // UP: show next triangle
    //if(GetKey(olc::Key::UP).bPressed) {
    //  vanassistant->DbgNextTriangle();
    //}

    // DOWN: show previous triangle
    //if(GetKey(olc::Key::DOWN).bPressed) {
    //  vanassistant->DbgPrevTriangle();
    //}
  }


  ///Load assets, initialize variables, etc
  
  void Load() override { 

    if(gfx == nullptr) {
      std::cerr << "gfx is null" << std::endl;
      exit(1);
    }
  
    // Setup

    asmText = new AsmText();

    // Load widgets

    starfield = new Starfield(0,0,gfx->GetScreenWidth(), gfx->GetScreenHeight(), NUM_STARS);
    //road = new Road();
    sceneScout3d = new WidgetScoutScene3d(0, 0, gfx->GetScreenWidth(), gfx->GetScreenHeight());
    
    verticalTextAnimator = new VerticalTextAnimator(
      "solidmono4", 
      9, 
      0.1f, 
      1.0f, 
      0.2f,
      0,
      0,
      gfx->GetScreenWidth(), 
      gfx->GetScreenHeight(),
      5,
      6
    );
    

  
    //scout
    //sceneScout3d->SetJawOpening(0.5f);

    //textanimator
    textAnimator = new TextAnimator("solidmono8", 0.1f, 1.5f, 0.5f, 8, 16, 4, gfx->GetScreenHeight() - 6, gfx->GetScreenWidth() - 8, 6, WHITE, true);
    textAnimator->QueueText("Hello, I am your assistant.");
    textAnimator->QueueText("I am here to help you.");
    

    //------------------------------------------
    // TTS test code !
    //------------------------------------------
    //Say("Hello, I am your assistant.");

    //MySam *sam = new MySam();
    //if(!sam->Init()) {
    //  std::cout << "Failed to initialize SAM" << std::endl;
    //}
    //std::string text1 = "Hello, I am your assistant.";
    //std::string text2 = "I am here to help you.";
    //std::string text3 = "Shall we play a game ?";
    //Say(text1);

    //!\\ as sam code runs on another thread. need to find a way to have a queue and a callback when next text is ready to be spoken

    //Say(text2);
    //Say(text3);
    //-- end of TTS test code

    
  }

  void Activate(DisplayPage* lastPage) override {

  }
  
  /**
  * Update assistant's logic, data, etc
  *
  * @param elapsedTime time since the last frame in seconds
  */
  void Update(float elapsedTime) override {

    
    starfield->Update(elapsedTime);

    //textAnimator->Update(elapsedTime);
    verticalTextAnimator->Update();
    //road->Update(elapsedTime);
    
    
    if(verticalTextAnimator->GetQueueSize() <= 4) {
      //fetch next line from asm text and queue it
      auto nextLine = asmText->GetLine();
      verticalTextAnimator->QueueText(nextLine);
    }
    
    //starfield->Update(elapsedTime);
    sceneScout3d->Update(elapsedTime);
  }

  /**
  * Render the assistant
  */
  void Render() override {

    //Clear screen
    gfx->Clear(BLACK);
    
    starfield->Render(); 
    verticalTextAnimator->DrawText();
    //road->Render(); 
    sceneScout3d->Render();
    //textAnimator->Render();
    //faceModel->Render(); 

    DrawTitle();
    //TODO
    //DrawFPS( gfx->GetFPS());
  }

  bool Say(const std::string& text) {
    textAnimator->QueueText(text);
    return false;
    //TODO
    //return sam->Say(text);
  }

  //HELP NEEDED
  void BeforeSayCallback() {
    //Callback when TTS is about to say next text
    //TODO
  }

  void ToggleDebug() {
    bShowDebug = !bShowDebug;
    sceneScout3d->ToggleDebugTriangles();
  }

  void ToggleRenderMode() {
    sceneScout3d->ToggleRenderMode();
  }

  void ToggleFps() {
    bShowFps = !bShowFps;
  }

  void DbgNextTriangle() {
    sceneScout3d->DbgNextTriangle();
  }

  void DbgPrevTriangle() {
    sceneScout3d->DbgPrevTriangle();
  }

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

  void DrawTitle() {
    //engine->FillRect(0,0, engine->GetScreenWidth() , 4, BLACK);
    //engine->SetFont("comp18");
    //engine->DrawText("Van Assistant",4, 20, WHITE);
  }

  void DrawFPS(uint32_t fps) {
    if(bShowFps) {
      //TODO
      gfx->SetFont("solidmono8");
      //engine->SetCursorPos(0, engine->GetScreenHeight()-12);
      auto text = std::to_string(fps);
      auto textsize = gfx->GetTextBounds(text, 0, 0);
      auto x = gfx->GetScreenWidth() - textsize.w - 4;
      auto y = 18;
      gfx->DrawText(text, x, y, YELLOW);
    }
  }
};
