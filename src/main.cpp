#include <DrzOlcPixelGameEngine.h>

#include "fonts/Computerfont18pt7b.h"

#include "fonts/Mono_Regular18pt7b.h"
#include "fonts/Mono_Regular14pt7b.h"
#include "fonts/Mono_Regular12pt7b.h"
#include "fonts/Mono_Regular8pt7b.h"

#include "fonts/Solid_Mono8pt7b.h"
#include "fonts/Solid_Mono6pt7b.h"
#include "fonts/Solid_Mono4pt7b.h"
 
#include "vanassistant/VanAssistant.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

class Test3D : public DrzOlcPixelGameEngine {
   
public:
	Test3D() : DrzOlcPixelGameEngine(this) {
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
	}

	bool OnUserCreate() override {
    vanassistant = std::make_unique<VanAssistant>(this);
    vanassistant->Setup();
		return true;
	}

  bool OnUserUpdate(float fElapsedTime) override {
    ReadInputs(fElapsedTime);
    vanassistant->Update(fElapsedTime);
    vanassistant->Render();
    return true;
  }

private:
  std::unique_ptr<VanAssistant> vanassistant; 
  
  void ReadInputs(float fElapsedTime) {
        
    // ESC: exit the app
    if(GetKey(olc::Key::ESCAPE).bPressed) {
      exit(0);
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

int main() {
	Test3D demo;
  if (demo.Construct(SCREEN_W, SCREEN_H, 1, 1, false, true))
    demo.Start();
  return 0;
}
