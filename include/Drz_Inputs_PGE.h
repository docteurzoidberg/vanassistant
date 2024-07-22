#pragma once

#include "IDrzEngine.h"
#include "IDrz_Inputs.h"

#include <olcPixelGameEngine.h>

namespace drz {

class Drz_Inputs_PGE: public IDrzInputs {
  public:

    Drz_Inputs_PGE(olc::PixelGameEngine* engine) : engine(engine) {}

    //Called once at the beginning of the program
    bool Setup() override {
      //nothing to do with PGE
      return true;
    }

    //Called in update loop by the engine before scan hardware
    void Update() override {
      //nothing to do with PGE
    }

    //Called once at the end of the program
    bool Close() override {
      //nothing to do with PGE
      return true;
    }

    hwbutton GetKey(drz::Key k) override {
      return toHwKey(engine->GetKey(toPGE(k)));
    }

  private:
    olc::PixelGameEngine* engine;

    olc::Key toPGE(drz::Key k) {
      //check if key exists
      if(olc2drzMap.find(k) == olc2drzMap.end()) {
        return olc::Key::NONE;
      }
      return olc2drzMap[k];
    }

    hwbutton toHwKey(olc::HWButton b) {
      hwbutton key;
      key.bHeld = b.bHeld;
      key.bPressed = b.bPressed;
      key.bReleased = b.bReleased;
      return key;
    }

    std::map<drz::Key, olc::Key> olc2drzMap = {
      {drz::Key::A, olc::Key::A},
      {drz::Key::B, olc::Key::B},
      {drz::Key::C, olc::Key::C},
      {drz::Key::D, olc::Key::D},
      {drz::Key::E, olc::Key::E},
      {drz::Key::F, olc::Key::F},
      {drz::Key::G, olc::Key::G},
      {drz::Key::H, olc::Key::H},
      {drz::Key::I, olc::Key::I},
      {drz::Key::J, olc::Key::J},
      {drz::Key::K, olc::Key::K},
      {drz::Key::L, olc::Key::L},
      {drz::Key::M, olc::Key::M},
      {drz::Key::N, olc::Key::N},
      {drz::Key::O, olc::Key::O},
      {drz::Key::P, olc::Key::P},
      {drz::Key::Q, olc::Key::Q},
      {drz::Key::R, olc::Key::R},
      {drz::Key::S, olc::Key::S},
      {drz::Key::T, olc::Key::T},
      {drz::Key::U, olc::Key::U},
      {drz::Key::V, olc::Key::V},
      {drz::Key::W, olc::Key::W},
      {drz::Key::X, olc::Key::X},
      {drz::Key::Y, olc::Key::Y},
      {drz::Key::Z, olc::Key::Z},
      {drz::Key::K0, olc::Key::K0},
      {drz::Key::K1, olc::Key::K1},
      {drz::Key::K2, olc::Key::K2},
      {drz::Key::K3, olc::Key::K3},
      {drz::Key::K4, olc::Key::K4},
      {drz::Key::K5, olc::Key::K5},
      {drz::Key::K6, olc::Key::K6},
      {drz::Key::K7, olc::Key::K7},
      {drz::Key::K8, olc::Key::K8},
      {drz::Key::K9, olc::Key::K9},
      {drz::Key::F1, olc::Key::F1},
      {drz::Key::F2, olc::Key::F2},
      {drz::Key::F3, olc::Key::F3},
      {drz::Key::F4, olc::Key::F4},
      {drz::Key::F5, olc::Key::F5},
      {drz::Key::F6, olc::Key::F6},
      {drz::Key::F7, olc::Key::F7},
      {drz::Key::F8, olc::Key::F8},
      {drz::Key::F9, olc::Key::F9},
      {drz::Key::F10, olc::Key::F10},
      {drz::Key::F11, olc::Key::F11},
      {drz::Key::F12, olc::Key::F12},
      {drz::Key::UP, olc::Key::UP},
      {drz::Key::DOWN, olc::Key::DOWN},
      {drz::Key::LEFT, olc::Key::LEFT},
      {drz::Key::RIGHT, olc::Key::RIGHT},
      {drz::Key::SPACE, olc::Key::SPACE},
      {drz::Key::ENTER, olc::Key::ENTER},
      {drz::Key::ESCAPE, olc::Key::ESCAPE},
      {drz::Key::BACK, olc::Key::BACK},
      {drz::Key::TAB, olc::Key::TAB},
      {drz::Key::SHIFT, olc::Key::SHIFT},
      {drz::Key::CTRL, olc::Key::CTRL},
      //{drz::Key::ALT, olc::Key::ALT},
      {drz::Key::INS, olc::Key::INS},
      {drz::Key::DEL, olc::Key::DEL},
      {drz::Key::HOME, olc::Key::HOME},
      {drz::Key::END, olc::Key::END},
      {drz::Key::PGUP, olc::Key::PGUP},
      {drz::Key::PGDN, olc::Key::PGDN},
      {drz::Key::PAUSE, olc::Key::PAUSE},
      {drz::Key::SCROLL, olc::Key::SCROLL}
    };
  
};

}