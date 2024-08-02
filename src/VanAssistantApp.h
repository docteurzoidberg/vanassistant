#pragma once  

#include <DrzEngine.h>
#include <DrzGraphics.h>

using namespace drz;

class VanAssistantApp : public IDrzEngineApp {
  public:
    VanAssistantApp();
    ~VanAssistantApp(); 
    void Setup();
    void Update(float elapsedTime);
    bool Command(const std::string& command);
    bool Data(const uint8_t* data, size_t size); 

  private:
    IDrzGraphics* gfx;
    //IDrzEngine* engine;
};