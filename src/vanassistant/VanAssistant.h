#include "AsmText.h"
#include "TextAnimator.h"
#include <cstdint>
#include <iostream>
#define SCREEN_W 256
#define SCREEN_H 240

#define NUM_STARS 750
#define GRID_SIZE 12

#include "../fonts/Computerfont18pt7b.h"

#include "../fonts/Mono_Regular18pt7b.h"
#include "../fonts/Mono_Regular14pt7b.h"
#include "../fonts/Mono_Regular12pt7b.h"
#include "../fonts/Mono_Regular8pt7b.h"

#include "../fonts/Solid_Mono8pt7b.h"
#include "../fonts/Solid_Mono6pt7b.h"
#include "../fonts/Solid_Mono4pt7b.h"
 
#include "Scene.h"
#include "Model.h"
#include "AnimatedObject.h"
#include "VerticalTextAnimator.h"
#include "TextAnimator.h"
#include "Starfield.h" 
#include "Road.h"
//#include "FaceModel.h"
#include "ScoutModel.h"

#include "MySam.h"

class VanAssistant {

public:

  VanAssistant(IDrzEngine* engine) : engine(engine) {

  }

  /**
  * Load assets, initialize variables, etc
  */
  void Setup() { 
    //Load fonts
    const font* comp18 = engine->LoadFont("comp18", &Computerfont18pt7b);
    const font* mono18 = engine->LoadFont("mono18", &Mono_Regular18pt7b);
    const font* mono14 = engine->LoadFont("mono14", &Mono_Regular14pt7b);
    const font* mono12 = engine->LoadFont("mono12", &Mono_Regular12pt7b);
    const font* mono8 = engine->LoadFont("mono8", &Mono_Regular8pt7b);
    const font* solidmono8 = engine->LoadFont("solidmono8", &Solid_Mono8pt7b);
    const font* solidmono6 = engine->LoadFont("solidmono6", &Solid_Mono6pt7b);
    const font* solidmono4 = engine->LoadFont("solidmono4", &Solid_Mono4pt7b);

    // Setup
    asmText = new AsmText(engine);
    scene = new Scene(engine);
    //faceModel = new FaceModel(engine);
    scoutHead = new ScoutHeadModel(engine);
    scoutJaw = new ScoutJawModel(engine);

    road = new Road(engine);
    starfield = new Starfield(engine, NUM_STARS);
    
    verticalTextAnimator = new VerticalTextAnimator(
      engine, 
      solidmono4, 
      8, 
      0.01f, 
      0.05f, 
      0.5f,
      2,
      4,
      engine->GetScreenWidth(), 
      engine->GetScreenHeight()-30,
      5,
      8
    );

    // Add models to scene
    //scene->AddModel(faceModel);
    scene->AddModel(scoutHead);
    scene->AddModel(scoutJaw);

    //faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::CLOSE, 3.0f);
    //faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::OPEN, 1.0f);
    //faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::CLOSE, 0.5f);
    //faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::OPEN, 0.5f);

    verticalTextAnimator->QueueText("Hello, I am your assistant.\nI am here to help you.");
    verticalTextAnimator->QueueText("How are you today ?\nShall we play a game ?");
    verticalTextAnimator->QueueText("Line1\nline2\nline3\nline4\nline5\nline6\nline7\nline8");
    verticalTextAnimator->QueueText("Line9\nline10\nline11\nline12\nline13\nline14\nline15\nline16");

    textAnimator = new TextAnimator(engine, solidmono8, 0.1f, 1.5f, 0.5f, 4, engine->GetScreenHeight() - 6);
    textAnimator->QueueText("Hello, I am your assistant.");
    textAnimator->QueueText("I am here to help you.");
    textAnimator->QueueText("Hello, I am your assistant.");
    textAnimator->QueueText("I am here to help you.");
    textAnimator->QueueText("Hello, I am your assistant.");
    textAnimator->QueueText("I am here to help you.");
    textAnimator->QueueText("Hello, I am your assistant.");
    textAnimator->QueueText("I am here to help you.");


    auto text = "TEST";
    engine->SetFont("mono8");
    auto textsize = engine->GetTextBounds(text, 0, 20);
    std::cout << "Text size: w=" << textsize.w << " h=" << textsize.h << std::endl;

    MySam *sam = new MySam();
    if(!sam->Init()) {
      std::cout << "Failed to initialize SAM" << std::endl;
    }
    std::string text1 = "Hello, I am your assistant.";
    std::string text2 = "I am here to help you.";
    std::string text3 = "Shall we play a game ?";
    sam->Say(text1);
    
    //Drzoid: only first text is spoken, the rest are ignored...
    //+thread is blocked we sam speaks

    //sam->Say(text2);
    //sam->Say(text3);
  }
  
  /**
  * Update assistant's logic, data, etc
  *
  * @param elapsedTime time since the last frame in seconds
  */
  void Update(float elapsedTime) {
   
    textAnimator->Update();
    verticalTextAnimator->Update();
    road->Update(elapsedTime);

    if(verticalTextAnimator->GetQueueSize() <= 4) {
      //fetch next line from asm text and queue it
      //auto nextLine = "This is a test line";
      auto nextLine = asmText->GetLine();
     // std::cout << "Next line: " << nextLine << std::endl;
      verticalTextAnimator->QueueText(nextLine);
    }

    starfield->Update(elapsedTime);
    //faceModel->Update(elapsedTime); 
    scene->Update(elapsedTime);
  }

  /**
  * Render the assistant
  */
  void Render() {


    engine->Clear(BLACK);
    
    verticalTextAnimator->DrawText();
    starfield->Render();
    road->Render(); 
    scene->Render();
    textAnimator->Render();
         
    //faceModel->Render();
    DrawTitle();
    DrawFPS( engine->GetFPS());
  }

  /**
  * Toggle debug mode
  */
  void ToggleDebug() {
    bShowDebug = !bShowDebug;
    scene->ToggleDebugTriangles();
  }

  void ToggleRenderMode() {
    scene->ToggleRenderMode();
  }

  /**
  * Toggle fps
  */
  void ToggleFps() {
    bShowFps = !bShowFps;
  }

  /**
  * Debug next triangle
  */
  void DbgNextTriangle() {
    scene->DbgNextTriangle();
  }

  /**
  * Debug previous triangle
  */
  void DbgPrevTriangle() {
    scene->DbgPrevTriangle();
  }

  Scene* getScene() {
    return scene;
  }

private: 
  IDrzEngine* engine;

  Scene* scene;
  AsmText* asmText;
  //FaceModel* faceModel;
  ScoutHeadModel* scoutHead;
  ScoutJawModel* scoutJaw;
  Road* road;
  Starfield* starfield;
  VerticalTextAnimator* verticalTextAnimator; 
  TextAnimator* textAnimator; 

  bool bShowFps = true;
  bool bShowDebug = false;

  void DrawTitle() {
    engine->FillRect(0,0, engine->GetScreenWidth() , 4, BLACK);
    //engine->SetFont("comp18");
    //engine->DrawText("Van Assistant",4, 20, WHITE);
  }

  void DrawFPS(uint32_t fps) {
    if(bShowFps) {
      //TODO
      engine->SetFont("solidmono8");
      //engine->SetCursorPos(0, engine->GetScreenHeight()-12);
      auto text = std::to_string(fps);
      auto textsize = engine->GetTextBounds(text, 0, 0);
      auto x = engine->GetScreenWidth() - textsize.w - 4;
      auto y = 18;
      engine->DrawText(text, x, y, YELLOW);
    }
  }
};
