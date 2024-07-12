#include "Drz_FB_Engine.h"
#include "Drz_Miniaudio_Sam.h"
#include "Drz_SerialInput.h"

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
    std::cout << "VanAssistantFB constructor" << std::endl;
  }

  bool Setup() {
    
    std::cout << "VanAssistantFB Setup" << std::endl;

    //Load fonts
    const font* solidmono8 = LoadFont("solidmono8", &Solid_Mono8pt7b);
    const font* solidmono4 = LoadFont("solidmono4", &Solid_Mono4pt7b);
    //Load sprites when any...
    //Load sounds when any...
    sam = std::make_unique<Drz_Miniaudio_Sam>();
    vanassistant = std::make_unique<VanAssistant>(this, sam.get());
    vanassistant->Setup();
    return true;
  }
    
  bool Loop(float fElapsedTime) {
    ReadInputs(fElapsedTime);
    vanassistant->Update(fElapsedTime);
    vanassistant->Render();
    return true;
  }

  void Say(std::string text) {
    vanassistant->Say(text);
  }

private:
  std::unique_ptr<VanAssistant> vanassistant; 
  std::unique_ptr<IDrzSam> sam;
  
  void ReadInputs(float fElapsedTime) {
    //TODO
  }
};

bool exec = true;
VanAssistantFB* app;
Drz_SerialInput* serialInput;

// Alias for convenience
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

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

  serialInput = new Drz_SerialInput();
  bool serialOpened = serialInput->Setup();
  if(!serialOpened) {
    std::cerr << "Error opening serial port" << std::endl;
    return 1;
  }

  app = new VanAssistantFB();
  app->SetFBG(fbg);
  app->Setup();

  double fTotalTime = 0.0f; // since start, in seconds !
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;

  TimePoint previousFrameTime = Clock::now();
  TimePoint currentFrameTime;

  do {
    currentFrameTime = Clock::now();
   
    // Calculate elapsed time in microseconds
    auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentFrameTime - previousFrameTime);
    float fElapsedTime = elapsedTime.count() / 1000000.0f;

    //std::cout << "fElapsedTime: " << fElapsedTime << std::endl;
    fTotalTime+=fElapsedTime;
    
    //rainbow effect, just for fun
    //r = (uint8_t)(128.0f + 128.0f * sinf(fTotalTime));
    //g = (uint8_t)(128.0f + 128.0f * sinf(fTotalTime + 2.0f));
    //b = (uint8_t)(128.0f + 128.0f * sinf(fTotalTime + 4.0f));
    //fbg_background(fbg,r,g,b); //replace with app loop
    
    int bytesRead = serialInput->Read();
    if(bytesRead > 0) {
      //std::cout << "Bytes read: " << bytesRead << std::endl;
      //std::cout << "Data: " << serialInput->read_buf << std::endl;
      //copy to string
      std::string data(serialInput->read_buf);
      app->Say(data);
    } 

    app->Loop(fElapsedTime);

    fbg_draw(fbg);
    fbg_flip(fbg);

    // Update previous frame time
    previousFrameTime = currentFrameTime;

  } while(exec);
  fbg_close(fbg);
  serialInput->Close();
  return 0;
}
