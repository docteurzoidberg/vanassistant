#include "Drz_PGE_Engine.h"
#include "Drz_Miniaudio_Sam.h"

#include "fonts/Computerfont18pt7b.h"

#include "fonts/Mono_Regular18pt7b.h"
#include "fonts/Mono_Regular14pt7b.h"
#include "fonts/Mono_Regular12pt7b.h"
#include "fonts/Mono_Regular8pt7b.h"

#include "fonts/Solid_Mono8pt7b.h"
#include "fonts/Solid_Mono6pt7b.h"
#include "fonts/Solid_Mono4pt7b.h"
 
#include "vanassistant/VanAssistant.h"

#define OLC_PGE_APPLICATION
#include <olcPixelGameEngine.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <memory>

class VanAssistantPGE : public Drz_PGE_Engine {
   
public:
	VanAssistantPGE() : Drz_PGE_Engine(this) {
		sAppName = "VanAssistant";
    //Load fonts
    const font* comp18 = LoadFont("comp18", &Computerfont18pt7b);
    const font* mono18 = LoadFont("mono18", &Mono_Regular18pt7b);
    const font* mono14 = LoadFont("mono14", &Mono_Regular14pt7b);
    const font* mono12 = LoadFont("mono12", &Mono_Regular12pt7b);
    const font* mono8 = LoadFont("mono8", &Mono_Regular8pt7b);
    const font* solidmono8 = LoadFont("solidmono8", &Solid_Mono8pt7b);
    const font* solidmono6 = LoadFont("solidmono6", &Solid_Mono6pt7b);
    const font* solidmono4 = LoadFont("solidmono4", &Solid_Mono4pt7b);
    //Load sprites when any...
    //Load sounds when any...
	}

	bool OnUserCreate() override {
    ConsoleCaptureStdOut(true);
    sam = std::make_unique<Drz_Miniaudio_Sam>();
    vanassistant = std::make_unique<VanAssistant>(this, sam.get());
    vanassistant->Setup();
		return true;
	}

  bool OnUserUpdate(float fElapsedTime) override {
    ReadInputs(fElapsedTime);
    vanassistant->Update(fElapsedTime);
    vanassistant->Render();
    return true;
  }

  bool OnConsoleCommand(const std::string& text) override {
    vanassistant->Say(text);
    return true;
  }

  void Say(std::string text) {
    vanassistant->Say(text);
  }

private:
  std::unique_ptr<VanAssistant> vanassistant; 
  std::unique_ptr<IDrzSam> sam;
  
  void ReadInputs(float fElapsedTime) {
        
    // Q: exit the app
    if(GetKey(olc::Key::Q).bPressed) {
      exit(0);
    }

    // F1: show console
    if(GetKey(olc::Key::F1).bPressed) {
      ConsoleShow(olc::Key::ESCAPE, false);
    }

    // B: toggle debug mode
    if(GetKey(olc::Key::B).bPressed) {
      vanassistant->ToggleDebug();
    }

    // F: toggle fps
    if(GetKey(olc::Key::F).bPressed) {
      vanassistant->ToggleFps();
    }

    //R: toggle render mode
    if(GetKey(olc::Key::R).bPressed) {
      vanassistant->ToggleRenderMode();
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
};

VanAssistantPGE app;

int main() {
  if (app.Construct(SCREEN_W, SCREEN_H, 2, 2, false, true))
    app.Start();
  return 0;
}



//emscripten exposes the Say function to the javascript
void Say(std::string text) {
  app.Say(text);
}

//if emscripten is defined, we need to bind the functions to the javascript
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("Say", &Say);
}
#endif