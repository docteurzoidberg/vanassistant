#include "AsmText.h"
#include "TextAnimator.h"
#include <cstdint>
#include <iostream>
#define SCREEN_W 256
#define SCREEN_H 240

#define NUM_STARS 750
#define GRID_SIZE 12

#include "../fonts/Monofont18pt7b.h"
#include "../fonts/Computerfont18pt7b.h"
#include "../fonts/Monofont8pt7b.h"
#include "../fonts/Solid_Mono8pt7b.h"
#include "../fonts/Solid_Mono6pt7b.h"
#include "../fonts/Solid_Mono4pt7b.h"
 
#include "Scene.h"
#include "Model.h"
#include "AnimatedObject.h"
#include "VerticalTextAnimator.h"
#include "TextAnimator.h"
#include "Starfield.h" 
#include "Grid.h"
//#include "FaceModel.h"
#include "ScoutModel.h"

#include "../sam/mysam.h"

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
    const font* mono18 = engine->LoadFont("mono18", &Monofont18pt7b);
    const font* mono8 = engine->LoadFont("mono8", &Monofont8pt7b);
    const font* solidmono8 = engine->LoadFont("mono8", &Solid_Mono8pt7b);
    const font* solidmono6 = engine->LoadFont("mono6", &Solid_Mono6pt7b);
    const font* solidmono4 = engine->LoadFont("mono4", &Solid_Mono4pt7b);

    // Setup
    asmText = new AsmText(engine);
    scene = new Scene(engine);
    //faceModel = new FaceModel(engine);
    scoutHead = new ScoutHeadModel(engine);
    scoutJaw = new ScoutJawModel(engine);

    grid = new Grid(engine, 1000.0f, 200.0f, GRID_SIZE);
    starfield = new Starfield(engine, NUM_STARS);
    
    verticalTextAnimator = new VerticalTextAnimator(
      engine, 
      solidmono4, 
      8, 
      0.01f, 
      0.05f, 
      0.5f,
      0,
      0,
      engine->GetScreenWidth(), 
      engine->GetScreenHeight(),
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

    textAnimator = new TextAnimator(engine, solidmono8, 0.1f, 1.5f, 0.5f);
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
    std::string text1 = "HELLO";
    sam->Say(text1);
    //sam->Say("Can you hear me now?");
  }
  
  /**
  * Update assistant's logic, data, etc
  *
  * @param elapsedTime time since the last frame in seconds
  */
  void Update(float elapsedTime) {
   
    textAnimator->Update();
    verticalTextAnimator->Update();

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

    
     
    //DrawTitle();

    // Draw text
    auto offset = engine->GetScreenHeight()-4;
    textAnimator->DrawText(2,offset);

    verticalTextAnimator->DrawText();
    starfield->Render();
    scene->Render();
         
    //grid->Render(); 
    //faceModel->Render();
    
    //DrawFPS( engine->GetFPS());
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
  Grid* grid;
  Starfield* starfield;
  VerticalTextAnimator* verticalTextAnimator; 
  TextAnimator* textAnimator; 

  bool bShowFps = true;
  bool bShowDebug = false;

  void DrawTitle() {
    //TODO
    engine->SetFont("mono18");
    engine->SetCursorPos(0, 24);
    engine->DrawText("VanAssistant",0, 0, WHITE);
  }

  void DrawFPS(uint32_t fps) {
    if(bShowFps) {
      //TODO
      engine->SetFont("comp18");
      engine->SetCursorPos(0, engine->GetScreenHeight()-12);
      engine->DrawText("FPS: " + std::to_string(fps), 0, 0, WHITE);
    }
  }
};
