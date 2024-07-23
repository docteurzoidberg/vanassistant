#ifdef _WIN32
#include <windows.h>
#undef DrawText
#endif

#include "Drz_Engine_PGE.h"
#include "Drz_Sam_Miniaudio.h"

#include "IDrzSerial.h"
#include "fonts/Solid_Mono8pt7b.h"
#include "fonts/Solid_Mono4pt7b.h"
 
#include "vanassistant/VanAssistant.h"


class VanAssistantPGE : public Drz_PGE_Engine {
   
public:

  std::string serial_port = "/dev/ttyUSB0";
  int serial_baudrate = 115200;

	VanAssistantPGE(int arc, char* argv[]) : Drz_PGE_Engine(this) {

    //Set window title
		sAppName = "VanAssistant";

    //Load fonts
    const font* solidmono8 = LoadFont("solidmono8", &Solid_Mono8pt7b);
    const font* solidmono4 = LoadFont("solidmono4", &Solid_Mono4pt7b);
    //Load sprites when any...

    //Load sounds when any...

    //Load 3d models when any...
	}

	bool OnUserCreate() override {

    sam = std::make_unique<Drz_Miniaudio_Sam>();
    vanassistant = std::make_unique<VanAssistant>(this, sam.get(), nullptr);
    vanassistant->Setup();

    DisplayPageManager::Load();
    
    ConsoleCaptureStdOut(true);
		return true;
	}

  bool OnUserUpdate(float fElapsedTime) override {

    // Q: exit the app
    if(GetKey(olc::Key::Q).bPressed) {
      exit(0);
    }

    // F1: show console
    if(GetKey(olc::Key::F1).bPressed) {
      ConsoleShow(olc::Key::ESCAPE, false);
    }
    
    // B: toggle debug mode
    //if(GetKey(drz::Key::B).bPressed) {
    //  ToggleDebug();
    //}

    // F: toggle fps
    //if(GetKey(drz::Key::F).bPressed) {
    //  ToggleFps();
   // }


   
   
    //vanassistant->ReadInputs(fElapsedTime);
    vanassistant->Update(fElapsedTime);
    vanassistant->Render();
    return true;
  }

  bool OnConsoleCommand(const std::string& text) override {
    vanassistant->Say(text);
    return true;
  }

  bool Say(std::string text) {
    return vanassistant->Say(text);
  }

private:
  std::unique_ptr<VanAssistant> vanassistant; 
  std::unique_ptr<IDrzSam> sam;
  std::unique_ptr<IDrzSerial> serial;
};

std::unique_ptr<VanAssistantPGE> app;

int main(int argc, char* argv[]) {
  app = std::make_unique<VanAssistantPGE>(argc, argv);
  if (app->Construct(SCREEN_W, SCREEN_H, 2, 2, false, true))
    app->Start();
  return 0;
}

#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    return main(__argc, __argv);
}
#endif