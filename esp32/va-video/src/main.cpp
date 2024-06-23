#include <Arduino.h>
#include <VanAssistant.h>
#include <ESP_8_BIT_GFX.h>

#include "fonts/Computerfont18pt7b.h"
#include "fonts/Mono_Regular18pt7b.h"
#include "fonts/Mono_Regular14pt7b.h"
#include "fonts/Mono_Regular12pt7b.h"
#include "fonts/Mono_Regular8pt7b.h"
#include "fonts/Solid_Mono8pt7b.h"
#include "fonts/Solid_Mono6pt7b.h"
#include "fonts/Solid_Mono4pt7b.h"

#include <Drz_ESP_8BIT_GFX_Engine.h>

ESP_8_BIT_GFX videoOut(true /* = NTSC */, 8 /* = RGB332 color */);
IDrzEngine* engine;
VanAssistant* vanassistant;

long start = 0;
long lastUpdate = 0;
long lastRender = 0;

class Engine : public DrzGameEngine {
public:
  Engine(ESP_8_BIT_GFX* p) : DrzGameEngine(p) { }
  hwbutton GetKey(uint8_t key) override {
    hwbutton button;
    return button;
  }
};

void setup() {

  // Initial setup of graphics library
  engine = new Engine(&videoOut);

  // Load fonts
  engine->LoadFont("comp18",      &Computerfont18pt7b);
  engine->LoadFont("mono18",      &Mono_Regular18pt7b);
  engine->LoadFont("mono14",      &Mono_Regular14pt7b);
  engine->LoadFont("mono12",      &Mono_Regular12pt7b);
  engine->LoadFont("mono8",       &Mono_Regular8pt7b);
  engine->LoadFont("solidmono8",  &Solid_Mono8pt7b);
  engine->LoadFont("solidmono6",  &Solid_Mono6pt7b);
  engine->LoadFont("solidmono4",  &Solid_Mono4pt7b);
  
  //Start the video output
  videoOut.begin();

  // Create the VanAssistant app
  vanassistant = new VanAssistant(engine);
  vanassistant->Setup();
  start = millis();
}

void loop() {
  long fElapsedTime = millis() - lastUpdate;
  vanassistant->Update(fElapsedTime);
  lastUpdate = millis();
  vanassistant->Render();
  lastRender = millis();
  // Wait for the next frame to minimize chance of visible tearing
  videoOut.waitForFrame();
}
