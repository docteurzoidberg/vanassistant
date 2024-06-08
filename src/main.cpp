#include "../include/DrzOlcPixelGameEngine.h"

#include "vanassistant/VanAssistant.h"

class Test3D : public DrzOlcPixelGameEngine {
   
public:
	Test3D() : DrzOlcPixelGameEngine(this) {
		sAppName = "VanAssistant";
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
  if (demo.Construct(SCREEN_W, SCREEN_H, 1, 1))
    demo.Start();
  return 0;
}
