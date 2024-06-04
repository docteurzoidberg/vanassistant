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
    faceModel->Update();
  }

  /**
  * Render the assistant
  */
  void Render() {
    //TODO
    engine->Clear(color::BLACK);

    DrawTitle();
         
    starfield->Render();
    grid->Render(); 
    faceModel->Render();

    DrawFPS(GetFPS());
  }

  /**
  * Toggle debug mode
  */
  void ToggleDebug() {
    bShowDebug = !bShowDebug;
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
    faceModel->dbgNextTriangle();
  }

  /**
  * Debug previous triangle
  */
  void DbgPrevTriangle() {
    faceModel->dbgPrevTriangle();
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
    //engine->drawString(10, 10, "VanAssistant", color::WHITE, "mono18");
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
