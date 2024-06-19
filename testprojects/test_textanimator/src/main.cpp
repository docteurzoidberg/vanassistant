#include "../../../src/Drz_PGE_Engine.h"
#include "../../../src/vanassistant/TextAnimator.h"
#include "../../../src/fonts/Solid_Mono8pt7b.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class TestTextAnimator : public Drz_PGE_Engine {
  public:
    TestTextAnimator() : Drz_PGE_Engine(this) {
      sAppName = "TestTextAnimator";
      //Load font
      LoadFont("solidmono8", &Solid_Mono8pt7b);
      SetFont("solidmono8");
    }

    bool OnUserCreate() override {
      ConsoleCaptureStdOut(true);
      textAnimator = new TextAnimator(this, "solidmono8", 0.1f, 1.5f, 0.5f, 8, 16, 4, ScreenHeight() - 6);
      textAnimator->QueueText("Hello, I am your assistant.");
      textAnimator->QueueText("I am here to help you.");
      return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
      
      // F1: show console
      if(GetKey(olc::Key::F1).bPressed) {
        ConsoleShow(olc::Key::ESCAPE, false);
      }

      Clear(BLACK);
      textAnimator->Update();
      textAnimator->Render();
      DrawText(std::to_string(GetFPS()), 4, 14, YELLOW);
      return true;
    }

    bool OnConsoleCommand(const std::string& text) override {
      textAnimator->QueueText(text);
      return true;
    }

  private:
    TextAnimator* textAnimator;
};

int main() {
  TestTextAnimator app;
  if (app.Construct(256, 240, 2, 2)) {
    app.Start();
  }
  return 0;
}