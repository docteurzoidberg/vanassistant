#include "Drz_FB_Engine.h"
#include "Drz_Miniaudio_Sam.h"
#include "fonts/Solid_Mono8pt7b.h"
#include "fonts/Solid_Mono4pt7b.h"
#include "vanassistant/VanAssistant.h"

#include "fb/fbgraphics.h"
#include "fb/fbg_fbdev.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <csignal>

 
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
bool exec = true;

int main() {
 
  //sigint handler
  signal(SIGINT, [](int signum) {
    exec = false;
  });

  // open "/dev/fb0" by default, use fbg_fbdevSetup("/dev/fb1", 0) if you want to use another framebuffer
  // note : fbg_fbdevInit is the linux framebuffer backend, you can use a different backend easily by including the proper header and compiling with the appropriate backend file found in ../custom_backend/backend_name
  struct _fbg *fbg = fbg_fbdevInit();
  if (fbg == NULL) {
      return 0;
  }
  
  //get app start time
  auto start = std::chrono::high_resolution_clock::now();
  auto lastupdate = start;
  float fElapsedTime = 0.0f;
  do {
    //time in seconds from last update
    fElapsedTime = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - lastupdate).count();
    fbg_clear(fbg, 0); // can also be replaced by fbg_background(fbg, 0, 0, 0);
    fbg_draw(fbg);
    exec = app.Loop(fElapsedTime);
    fbg_flip(fbg);
    lastupdate = std::chrono::high_resolution_clock::now();
  } while(exec);
  fbg_close(fbg);
  return 0;
}
