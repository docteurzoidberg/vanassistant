#include "../../../src/Drz_PGE_Engine.h"
#include "../../../src/vanassistant/VerticalTextAnimator.h"
#include "../../../src/fonts/Solid_Mono8pt7b.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class TestVerticalTextAnimator : public Drz_PGE_Engine {
  public:
    TestVerticalTextAnimator() : Drz_PGE_Engine(this) {
      sAppName = "TestVerticalTextAnimator";
      //Load font
      LoadFont("solidmono8", &Solid_Mono8pt7b);
    }

    bool OnUserCreate() override {
      ConsoleCaptureStdOut(true);
      verticalTextAnimator = new VerticalTextAnimator(
        this, 
        "solidmono8", 
        8, 
        0.01f, 
        0.05f, 
        0.5f,
        2,
        4,
        ScreenWidth(), 
        ScreenHeight()-30,
        5,
        8
      ); 

      verticalTextAnimator->QueueText("Hello World!");
      verticalTextAnimator->QueueText("This is a test.");
      verticalTextAnimator->QueueText("This is a test of the emergency broadcast system.");
      verticalTextAnimator->QueueText("This is only a test.");
      verticalTextAnimator->QueueText("If this were a real emergency, you would be instructed to...");
      verticalTextAnimator->QueueText("Do something.");
      verticalTextAnimator->QueueText("Do something else.");

      return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
      Clear(BLACK);
      verticalTextAnimator->Update();
      verticalTextAnimator->DrawText();
      return true;
    }

    bool OnConsoleCommand(const std::string& text) override {
      return true;
    }

  private:
    VerticalTextAnimator* verticalTextAnimator;
};

int main() {
  TestVerticalTextAnimator app;
  if (app.Construct(256, 240, 2, 2)) {
    app.Start();
  }
  return 0;
}