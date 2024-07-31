#include <IDrzEngine.h>
#include <IDrzInputs.h>

#include "pages/road/widgets/SimpleGaugeWidget.h"
#include "pages/road/widgets/RoadWidget.h"
#include "pages/road/widgets/CompassWidget.h"

#include "pages/road/sprites/fuelgaugemask.h"
#include "pages/road/sprites/tempgaugemask.h"

using namespace drz;

class PageRoad : public DisplayPage {
  public :

    PageRoad() {
      gaugeFuel = new SimpleVerticalGaugeWidget(0, 80, 80, 80, 0, 100, 25);
      gaugeFuel->SetBarRect({36, 19, 24, 48});
      
      gaugeTemp = new SimpleVerticalGaugeWidget(0, 160, 80, 80, 0, 110, 88);
      gaugeTemp->SetBarRect({36, 19, 24, 48});

      road = new RoadWidget(81, 81, 160, 160);
      road->SetSpeed(88);

      compass = new CompassWidget(121, 0, 80, 80); 

      engine = DrzEngine::Get();
      inputs = DrzInputs::Get();
    }

    void ReadInputs() override {
      //TODO
      if(inputs->GetKey(Key::E).bPressed) {
        std::cout << "E pressed" << std::endl;
      }
    }

    void Load() override {
      fuelGaugeMask = engine->CreateSpriteFromData(FUELGAUGEMASK_SPRITE_DATA, FUELGAUGEMASK_SPRITE_WIDTH, FUELGAUGEMASK_SPRITE_HEIGHT);
      tempGaugeMask = engine->CreateSpriteFromData(TEMPGAUGEMASK_SPRITE_DATA, TEMPGAUGEMASK_SPRITE_WIDTH, TEMPGAUGEMASK_SPRITE_HEIGHT);
      gaugeFuel->SetMask(fuelGaugeMask);
      gaugeTemp->SetMask(tempGaugeMask);
      AddWidget(gaugeFuel);
      AddWidget(gaugeTemp);
      AddWidget(road);
      AddWidget(compass);
    }

    void Update(float fElapsedTime) override {
      engine->Clear(YELLOW);
    }

    void Render() override {
    }   

  private:
    SimpleVerticalGaugeWidget* gaugeFuel;
    SimpleVerticalGaugeWidget* gaugeTemp;
    RoadWidget* road;
    CompassWidget* compass;

    Sprite* fuelGaugeMask;
    Sprite* tempGaugeMask;

    IDrzEngine* engine;
    IDrzInputs* inputs;

};