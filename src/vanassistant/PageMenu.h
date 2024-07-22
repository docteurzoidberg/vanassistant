#pragma once

#include <IDrzEngine.h>

#include <iostream>

using namespace drz;

class PageMenu : public DisplayPage {
  public:
    PageMenu() {}

    void ReadInputs() override {
    
    }

    ///Load assets, initialize variables, etc
    void Load() override {
      engine = DisplayPageManager::GetEngine();
      if(engine == nullptr) {
        std::cerr << "Engine is null" << std::endl;
        exit(1);
      }
    }

    ///Update the page
    void Update(float fElapsedTime) override {
     
    }

    ///Render the page
    void Render() override {
      
    }

    
  protected:
    IDrzEngine* engine;
};