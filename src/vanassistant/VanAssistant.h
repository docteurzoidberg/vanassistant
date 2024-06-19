#include "IDrzSam.h"

#define SCREEN_W 256
#define SCREEN_H 240
#define NUM_STARS 750

#include "AsmText.h"
#include "Road.h"
#include "Scene.h"
#include "ScoutModel.h"
#include "Starfield.h" 
#include "TextAnimator.h"
#include "VerticalTextAnimator.h"

#include <cstdint>

using namespace drz;

class VanAssistant {

public:

  VanAssistant(IDrzEngine* engine, IDrzSam* sam) : engine(engine), sam(sam) {}
  
  ///Load assets, initialize variables, etc
  void Setup() { 
  
    // Setup
    asmText = new AsmText(engine);
    scene = new Scene(engine);
    scout = new Scout(scene);

    road = new Road(engine);
    starfield = new Starfield(engine, NUM_STARS);
    
    verticalTextAnimator = new VerticalTextAnimator(
      engine, 
      "solidmono4", 
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
    scout->SetJawOpening(0.5f);

    // Setup say text animator
    textAnimator = new TextAnimator(engine, "solidmono8", 0.1f, 1.5f, 0.5f, 8, 16, 4, engine->GetScreenHeight() - 6);
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
  
  /**
  * Update assistant's logic, data, etc
  *
  * @param elapsedTime time since the last frame in seconds
  */
  void Update(float elapsedTime) {
   
    textAnimator->Update();
    verticalTextAnimator->Update();
    road->Update(elapsedTime);
    scout->Update(elapsedTime);

    if(verticalTextAnimator->GetQueueSize() <= 4) {
      //fetch next line from asm text and queue it
      auto nextLine = asmText->GetLine();
      verticalTextAnimator->QueueText(nextLine);
    }

    starfield->Update(elapsedTime);
    scene->Update(elapsedTime);
  }

  /**
  * Render the assistant
  */
  void Render() {

    //Clear screen
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

  bool Say(const std::string& text) {
    textAnimator->QueueText(text);
    return sam->Say(text);
  }

  //HELP NEEDED
  void BeforeSayCallback() {
    //Callback when TTS is about to say next text
    //TODO
  }

  void ToggleDebug() {
    bShowDebug = !bShowDebug;
    scene->ToggleDebugTriangles();
  }

  void ToggleRenderMode() {
    scene->ToggleRenderMode();
  }

  void ToggleFps() {
    bShowFps = !bShowFps;
  }

  void DbgNextTriangle() {
    scene->DbgNextTriangle();
  }

  void DbgPrevTriangle() {
    scene->DbgPrevTriangle();
  }

  Scene* getScene() {
    return scene;
  }

private: 
  IDrzEngine* engine;
  IDrzSam* sam;

  Scene* scene;
  AsmText* asmText;
  Scout *scout;
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
