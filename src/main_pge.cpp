#ifdef _WIN32
#include <windows.h>
#undef DrawText
#undef SetPort
#endif

#include <DrzSerial.h>
#include <DrzEngine.h>
#include <DrzEngine_PGE.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif

#include "vanassistant/VanAssistantApp.h"

#define SCREEN_W 320
#define SCREEN_H 240

using namespace drz;

std::unique_ptr<IDrzEngine> drzenginepge = nullptr;

VanAssistantApp* app = nullptr;

//emscripten exposes the Say function to the javascript
bool Say(std::string text) {
  //TODO
  return false;
}

int main(int argc, char* argv[]) {

  //NOTE: emscripten will not use command line arguments, so no serial port will be used

  std::string serialPort = "";
  for(int i = 0; i < argc; i++) {
    if(std::string(argv[i]) == "--serial") {
      serialPort = argv[i+1];
    }
  }
  int serialBauds = 115200;
  for(int i = 0; i < argc; i++) {
    if(std::string(argv[i]) == "--bauds") {
      serialBauds = std::stoi(argv[i+1]);
    }
  }

  //Engine implementation
  drzenginepge = std::make_unique<DrzEngine_PGE>(SCREEN_W, SCREEN_H, 2);

  auto serial = DrzSerial::Get();

  if(serial==nullptr) {
    std::cout << "DrzSerial is null" << std::endl;
    exit(1);
  }

  if(!serialPort.empty()) {
    serial->SetPort(serialPort.c_str());
    serial->SetBaudRate(serialBauds);
    if(!serial->Setup()) {
      std::cout << "Serial port setup failed" << std::endl;
      exit(1);
    }
  } else {
    DrzSerial::Set(nullptr);
  }

  app = new VanAssistantApp();
  
  DrzEngine::UseApp(app);
  
  //engine setup autodetects engine sub parts
  DrzEngine::Setup();  

  drzenginepge->SetTitle("VanAssistant");

  //Start engine main loop
  DrzEngine::Start();

  return 0;
}

#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
  // Convert command line to argc and argv
  int __argc;
  char** __argv;
  {
    LPWSTR* szArglist;
    int nArgs;
    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    __argc = nArgs;
    __argv = (char**)malloc(nArgs * sizeof(char*));
    for (int i = 0; i < nArgs; i++) {
      int size = wcslen(szArglist[i]) + 1;
      __argv[i] = (char*)malloc(size);
      wcstombs(__argv[i], szArglist[i], size);
    }
    LocalFree(szArglist);
  }
  return main(__argc, __argv);
}
#endif

//if emscripten is defined, we need to bind the functions to the javascript
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_BINDINGS(my_module) {
  emscripten::function("Say", &Say);
}
#endif
