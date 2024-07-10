#include "Drz_FB_Engine.h"
#include "Drz_Miniaudio_Sam.h"

#include "fonts/Solid_Mono8pt7b.h"
#include "fonts/Solid_Mono4pt7b.h"

#include "vanassistant/VanAssistant.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>
 
class VanAssistantFB : public Drz_FB_Engine {
   
public:
	VanAssistantFB() : Drz_FB_Engine() {
    //Load fonts
    const font* solidmono8 = LoadFont("solidmono8", &Solid_Mono8pt7b);
    const font* solidmono4 = LoadFont("solidmono4", &Solid_Mono4pt7b);
    //Load sprites when any...
    //Load sounds when any...

    sam = std::make_unique<Drz_Miniaudio_Sam>();
    vanassistant = std::make_unique<VanAssistant>(this, sam.get());
    vanassistant->Setup();
  }
    
  bool Loop(float fElapsedTime) {
    ReadInputs(fElapsedTime);
    vanassistant->Update(fElapsedTime);
    vanassistant->Render();
    return true;
  }

private:
  std::unique_ptr<VanAssistant> vanassistant; 
  std::unique_ptr<IDrzSam> sam;
  
  void ReadInputs(float fElapsedTime) {
    //TODO
  }
};

VanAssistantFB app;

int main() {
  //get app start time
  auto start = std::chrono::high_resolution_clock::now();
  auto lastupdate = start;
  float fElapsedTime = 0.0f;
  while (true) {
    //time in seconds from last update
    fElapsedTime = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - lastupdate).count();
    if(app.Loop(fElapsedTime))
      lastupdate = std::chrono::high_resolution_clock::now();
  }
}
