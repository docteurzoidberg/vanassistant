#include <DrzEngine.h>
#include <DrzEngine_PGE.h>
#include <DrzGraphics.h>

#include "../../src/vanassistant/pages/common/widgets/Starfield.h"
#include "../../src/vanassistant/fonts/Solid_Mono8pt7b.h"
#include "IDrzEngine.h"

#include <cstdint>
#include <string>
#include <iostream>

using namespace drz;

class TestApp: public IDrzEngineApp {
  public:
    TestApp() {
      std::cout << "TestApp constructor called" << std::endl;
      gfx = DrzGraphics::Get();  
      engine = DrzEngine::Get();
    }

    ~TestApp(){
      std::cout << "TestApp destructor called" << std::endl;
    } 

    void Setup() {
      std::cout << "TestApp::Setup called" << std::endl;
      engine->SetTitle("TestStarfield");
      //Load font
      gfx->LoadFont("solidmono8", const_cast<font*>(&Solid_Mono8pt7b));
      gfx->SetFont("solidmono8");

      startfield = new Starfield(0,0, gfx->GetScreenWidth(),gfx->GetScreenHeight(),2000);
    }

    void Update(float elapsedTime) {
      //std::cout << "TestApp::Update called with elapsed time: " << elapsedTime << std::endl;
      gfx->Clear(BLACK);
      startfield->Update(elapsedTime);
      startfield->Render();
    }

    bool Command(const std::string& command) {
      std::cout << "Command: " << command << std::endl;
      return false; // Command not handled
    }

    bool Data(const uint8_t* data, size_t size) {
      std::cout << "TestApp::Data called with size: " << size << std::endl;
      // Process data here
      return false; // Data not handled
    }

    IDrzGraphics* gfx;
    IDrzEngine* engine;
    Starfield* startfield;
};

int main(){
  IDrzEngine* drzenginepge = new DrzEngine_PGE(320, 240, 2);
  TestApp* testapp = new TestApp();
  DrzEngine::UseApp(testapp);
  DrzEngine::Setup();  
  DrzEngine::Start();
  return 0;
}
