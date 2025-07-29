#include "PageAssistant.h"

PageAssistant::PageAssistant(IDrzSam* sam) : sam(sam), DisplayPage("assistant") {
  gfx = DrzGraphics::Get();
  engine = DrzEngine::Get();
  inputs = DrzInputs::Get();
}

PageAssistant::PageAssistant() : DisplayPage("assistant") {
  gfx = DrzGraphics::Get();
  engine = DrzEngine::Get();
  inputs = DrzInputs::Get();
}

void PageAssistant::ReadInputs() {

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

void PageAssistant::Load() { 

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

void PageAssistant::Activate(DisplayPage* lastPage) {

}

/**
* Update assistant's logic, data, etc
*
* @param elapsedTime time since the last frame in seconds
*/
void PageAssistant::Update(float elapsedTime) {

  
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
void PageAssistant::Render() {

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

bool PageAssistant::Say(const std::string& text) {
  textAnimator->QueueText(text);
  return false;
  //TODO
  //return sam->Say(text);
}

//HELP NEEDED
void PageAssistant::BeforeSayCallback() {
  //Callback when TTS is about to say next text
  //TODO
}

void PageAssistant::ToggleDebug() {
  bShowDebug = !bShowDebug;
  sceneScout3d->ToggleDebugTriangles();
}

void PageAssistant::ToggleRenderMode() {
  sceneScout3d->ToggleRenderMode();
}

void PageAssistant::ToggleFps() {
  bShowFps = !bShowFps;
}

void PageAssistant::DbgNextTriangle() {
  sceneScout3d->DbgNextTriangle();
}

void PageAssistant::DbgPrevTriangle() {
  sceneScout3d->DbgPrevTriangle();
}

void  PageAssistant::DrawTitle() {
  //engine->FillRect(0,0, engine->GetScreenWidth() , 4, BLACK);
  //engine->SetFont("comp18");
  //engine->DrawText("Van Assistant",4, 20, WHITE);
}

void  PageAssistant::DrawFPS(uint32_t fps) {
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

