#include "DrzEngine.h"
#include "DrzEngine_PGE.h"

#include "../../../src/vanassistant/pages/assistant/AsmText.h"
#include "../../../src/vanassistant/pages/assistant/widgets/VerticalTextAnimator.h"
#include "../../../src/vanassistant/fonts/Solid_Mono8pt7b.h"

#include <memory>

using namespace drz;


class TestVerticalTextAnimatorApp : public IDrzEngineApp {
  public:
    TestVerticalTextAnimatorApp() {
      gfx = DrzGraphics::Get();
      engine = DrzEngine::Get();
      inputs = DrzInputs::Get();
    }

    //bool OnUserCreate() override {
    void Setup() override {

      //ConsoleCaptureStdOut(true);
      gfx->LoadFont("solidmono8", (font* )&Solid_Mono8pt7b);

      asmText = new AsmText();

      verticalTextAnimator = new VerticalTextAnimator(
        "solidmono8", 
        20, 
        0.05f, 
        1.5f, 
        0.2f,
        2,
        4,
        gfx->GetScreenWidth(), 
        gfx->GetScreenHeight()-30,
        8,
        14
      ); 

      verticalTextAnimator->QueueText("Hello World!");
      verticalTextAnimator->QueueText("This is a test.");
      verticalTextAnimator->QueueText("This is a test of the emergency broadcast system.");
      verticalTextAnimator->QueueText("This is only a test.");
      verticalTextAnimator->QueueText("If this were a real emergency, you would be instructed to...");
      verticalTextAnimator->QueueText("Do something.");
      verticalTextAnimator->QueueText("Do something else.");
    }

    void Update(float elapsedTime) override {

      // F1: show console
      //if(GetKey(olc::Key::F1).bPressed) {
      //  ConsoleShow(olc::Key::ESCAPE, false);
      //}

      if(verticalTextAnimator->GetQueueSize() < 4) {
        verticalTextAnimator->QueueText(asmText->GetLine());
      }

      gfx->Clear(BLACK);
      verticalTextAnimator->Update();
      //DrawText(std::to_string(engine->GetFPS()), 4, 14, YELLOW);
      //return true;v
      verticalTextAnimator->DrawText();
    }

    bool Command(const std::string& command) override {
      // Implement the Command method
      return false;
    }

    bool Data(const uint8_t* data, size_t size) override {
      // Implement the Data method
      return false;
    }

  private:
    VerticalTextAnimator* verticalTextAnimator;
    AsmText* asmText;
    IDrzGraphics* gfx;
    IDrzEngine* engine;
    IDrzInputs* inputs;

};

std::unique_ptr<IDrzEngine> drzenginepge = nullptr;
TestVerticalTextAnimatorApp* app;

int main() {

  //Engine implementation
  drzenginepge = std::make_unique<DrzEngine_PGE>(320, 240, 2);


  app = new TestVerticalTextAnimatorApp();

  DrzEngine::UseApp(app);
  DrzEngine::Setup();
  DrzEngine::Start();
}