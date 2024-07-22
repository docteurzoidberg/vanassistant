#include <Drz_Engine_PGE.h>
#include "../../../src/vanassistant/pages/assistant/AsmText.h"
#include "../../../src/vanassistant/pages/assistant/widgets/VerticalTextAnimator.h"
#include "../../../src/fonts/Solid_Mono8pt7b.h"


class TestVerticalTextAnimator : public Drz_PGE_Engine {
  public:
    TestVerticalTextAnimator() : Drz_PGE_Engine(this) {
      sAppName = "TestVerticalTextAnimator";
      //Load font
      LoadFont("solidmono8", &Solid_Mono8pt7b);
      SetFont("solidmono8");
    }

    bool OnUserCreate() override {

      ConsoleCaptureStdOut(true);

      asmText = new AsmText();

      verticalTextAnimator = new VerticalTextAnimator(
        this, 
        "solidmono8", 
        20, 
        0.05f, 
        1.5f, 
        0.2f,
        2,
        4,
        ScreenWidth(), 
        ScreenHeight()-30,
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

      return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {

      // F1: show console
      if(GetKey(olc::Key::F1).bPressed) {
        ConsoleShow(olc::Key::ESCAPE, false);
      }

      if(verticalTextAnimator->GetQueueSize() < 4) {
        verticalTextAnimator->QueueText(asmText->GetLine());
      }

      Clear(BLACK);
      verticalTextAnimator->Update();
      verticalTextAnimator->DrawText();
      DrawText(std::to_string(GetFPS()), 4, 14, YELLOW);
      return true;
    }

    bool OnConsoleCommand(const std::string& text) override {
      return true;
    }

  private:
    VerticalTextAnimator* verticalTextAnimator;
    AsmText* asmText;

};

int main() {
  TestVerticalTextAnimator app;
  if (app.Construct(256, 240, 2, 2)) {
    app.Start();
  }
  return 0;
}