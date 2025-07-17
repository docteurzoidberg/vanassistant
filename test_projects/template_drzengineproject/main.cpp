#include <DrzEngine.h>
#include <DrzEngine_PGE.h>
#include <DrzGraphics.h>

#include <cstdint>
#include <string>
#include <iostream>

using namespace drz;

class TestApp: public IDrzEngineApp {
  public:
    TestApp() {
      std::cout << "TestApp constructor called" << std::endl;
      gfx = DrzGraphics::Get();
    }
    ~TestApp(){
      std::cout << "TestApp destructor called" << std::endl;
    } 
    void Setup() {
      std::cout << "TestApp::Setup called" << std::endl;
    }
    void Update(float elapsedTime) {
      std::cout << "TestApp::Update called with elapsed time: " << elapsedTime << std::endl;
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
};


int main(){

  //Engine implementation
  IDrzEngine* drzenginepge = new DrzEngine_PGE(320, 240, 2);

  TestApp* testapp = new TestApp();

  DrzEngine::UseApp(testapp);
  
  //engine setup autodetects engine sub parts
  DrzEngine::Setup();  

  //Start engine main loop
  DrzEngine::Start();

  return 0;
}
