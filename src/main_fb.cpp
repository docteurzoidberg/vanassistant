#include <DrzEngine.h>
#include <DrzEngine_FB.h>

#include "DrzSerial.h"
#include "vanassistant/VanAssistantApp.h"

#define SCREEN_W 320
#define SCREEN_H 240

#include <memory>

using namespace drz;

std::unique_ptr<IDrzEngine> drzengine = nullptr;

VanAssistantApp* app = nullptr;

int main(int argc, char* argv[]) {

  //sigint handler
  //signal(SIGINT, [](int signum) {
  //  exec = false;
  //});

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

  std::string fbdev = "/dev/fb0";
  for(int i = 0; i < argc; i++) {
    if(std::string(argv[i]) == "--fbdev") {
      fbdev = argv[i+1];
    }
  }

  //Engine implementation
  drzengine = std::make_unique<DrzEngine_FB>(SCREEN_W, SCREEN_H);

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
    //Disable serial if no port is set
    DrzSerial::Set(nullptr);
  }
  
  app = new VanAssistantApp();
  
  DrzEngine::UseApp(app);
  
  //engine setup autodetects engine sub parts
  DrzEngine::Setup();  

  //Start engine main loop
  DrzEngine::Start();

  return 0;
}
