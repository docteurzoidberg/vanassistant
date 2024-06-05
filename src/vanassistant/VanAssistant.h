#define SCREEN_W 256
#define SCREEN_H 240

#define NUM_STARS 750
#define GRID_SIZE 12

#include "../fonts/Computerfont18pt7b.h"

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
    engine->LoadFont("mono18", &Computerfont18pt7b);
    // Setup
    faceModel = new FaceModel(engine);
    grid = new Grid(engine, 1000.0f, 200.0f, GRID_SIZE);
    starfield = new Starfield(engine, NUM_STARS);
    textAnimator = new TextAnimator(engine, 0.5f, 0.5f, 0.5f);
  }
  
  /**
  * Update assistant's logic, data, etc
  *
  * @param elapsedTime time since the last frame in seconds
  */
  void Update(float elapsedTime) {
    textAnimator->Update();
    faceModel->Update(elapsedTime);
  }

  /**
  * Render the assistant
  */
  void Render() {

    engine->Clear(BLACK);

    //engine->DrawTriangle({0,0}, {0, (float) engine->GetScreenWidth()}, {(float) engine->GetScreenWidth(), (float) engine->GetScreenHeight()}, RED);

    DrawTitle();
         
    //starfield->Render();
    //grid->Render(); 
    //faceModel->Render();

    //DrawFPS(GetFPS());
  }

  /**
  * Toggle debug mode
  */
  void ToggleDebug() {
    bShowDebug = !bShowDebug;
    faceModel->ToggleDebugTriangle();
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
    faceModel->DbgNextTriangle();
  }

  /**
  * Debug previous triangle
  */
  void DbgPrevTriangle() {
    faceModel->DbgPrevTriangle();
  }

private: 
  IDrzEngine* engine;

  FaceModel* faceModel;
  Grid* grid;
  Starfield* starfield;
  TextAnimator* textAnimator; 

  bool bShowFps = false;
  bool bShowDebug = false;

  void DrawTitle() {
    //TODO
    engine->SetFont("mono18");
    engine->SetCursorPos(0, 40);
    engine->DrawText("ABCD", 40, 40, WHITE);
  }

  void DrawFPS(int fps) {
    if(bShowFps) {
      //TODO
      //engine->drawString(10, 10, "FPS: " + std::to_string(fps), color::WHITE, "mono18");
    }
  }

  int GetFPS() {
    //TODO
    return 0;
  }
};
