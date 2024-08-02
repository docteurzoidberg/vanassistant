#include <DrzEngine.h>
#include <DrzEngine_PGE.h>

#ifdef _WIN32
#include <windows.h>
#undef DrawText
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

#include "VanAssistantApp.h"

#define SCREEN_W 320
#define SCREEN_H 240

#include <memory>

using namespace drz;

std::unique_ptr<IDrzEngine> drzenginepge = nullptr;

VanAssistantApp* app = nullptr;

#ifdef _WIN32
//TODO: include win serial
#elif __linux__
//TODO: include linux serial
#endif

//emscripten exposes the Say function to the javascript
bool Say(std::string text) {
  //TODO
  return false;
}

int main(int argc, char* argv[]) {

  //TODO: use serial only if serial port is provided
  //TODO: do not use serial if emscripten

  //Engine implementation
  drzenginepge = std::make_unique<DrzEngine_PGE>(SCREEN_W, SCREEN_H, 2);

  #ifdef _WIN32
  //TODO: include win serial
  #elif __linux__
  //TODO: include linux serial
  #endif


  app = new VanAssistantApp(/* graphics, serial, input */);
  
  DrzEngine::UseApp(app);
  
  //engine setup autodetects engine sub parts
  DrzEngine::Setup();  

  //Start engine main loop
  DrzEngine::Start();

  return 0;
}

#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
  return main(__argc, __argv);
}
#endif

//if emscripten is defined, we need to bind the functions to the javascript
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_BINDINGS(my_module) {
  emscripten::function("Say", &Say);
}
#endif
