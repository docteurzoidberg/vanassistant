#include <cstdint>
#define SCREEN_W 256
#define SCREEN_H 240

#define NUM_STARS 750
#define GRID_SIZE 12

#include "../fonts/Computerfont18pt7b.h"
 
#include "Scene.h"
#include "Model.h"
#include "Animated3DObject.h"
#include "TextAnimator.h"
#include "Starfield.h" 
#include "Grid.h"
#include "FaceModel.h"

class VanAssistant {

public:

  VanAssistant(IDrzEngine* engine) : engine(engine) {

  }

  /**
  * Load assets, initialize variables, etc
  */
  void Setup() { 
    //Load fonts
    const font* mono18 = engine->LoadFont("mono18", &Computerfont18pt7b);
    // Setup
    scene = new Scene(engine);
    faceModel = new FaceModel(engine);
    grid = new Grid(engine, 1000.0f, 200.0f, GRID_SIZE);
    starfield = new Starfield(engine, NUM_STARS);
    textAnimator = new TextAnimator(engine, &Computerfont18pt7b, 0.5f, 0.5f, 0.5f);

    // Add models to scene
    scene->AddModel(faceModel);

    faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::CLOSE, 3.0f);
    faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::OPEN, 1.0f);
    faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::CLOSE, 0.5f);
    faceModel->mouth->QueueAnimation(MouthPart::KEY_FRAME::OPEN, 0.5f);

    textAnimator->QueueText("Hello, I am your assistant.");
    textAnimator->QueueText("I am here to help you.");
  }
  
  /**
  * Update assistant's logic, data, etc
  *
  * @param elapsedTime time since the last frame in seconds
  */
  void Update(float elapsedTime) {
    scene->Update(elapsedTime);
    //textAnimator->Update();
    //starfield->Update(elapsedTime);
    //faceModel->Update(elapsedTime);
  }

  /**
  * Render the assistant
  */
  void Render() {

    engine->Clear(BLACK);

    scene->Render();
     
    //DrawTitle();

    // Draw text
    //textAnimator->DrawText(20,SCREEN_H-40);
         
    //starfield->Render();
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
  FaceModel* faceModel;
  Grid* grid;
  Starfield* starfield;
  TextAnimator* textAnimator; 

  bool bShowFps = true;
  bool bShowDebug = false;

  void DrawTitle() {
    //TODO
    engine->SetFont("mono18");
    engine->SetCursorPos(0, 40);
    engine->DrawText("VanAssistant", 40, 40, WHITE);
  }

  void DrawFPS(uint32_t fps) {
    if(bShowFps) {
      //TODO
      engine->SetFont("mono18");
      engine->SetCursorPos(0, 0);
      engine->DrawText("FPS: " + std::to_string(fps), 0, 0, WHITE);
    }
  }
};
