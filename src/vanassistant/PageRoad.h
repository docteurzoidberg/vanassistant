#include <DrzGraphics.h>
#include <DrzInputs.h>

#include "DisplayPageManager.h"

#include "DrzSerial.h"
#include "pages/road/widgets/DistanceWidget.h"
#include "pages/road/widgets/SimpleGaugeWidget.h"
#include "pages/road/widgets/RoadWidget.h"
#include "pages/road/widgets/CompassWidget.h"

#include "pages/road/sprites/fuelgaugemask.h"
#include "pages/road/sprites/tempgaugemask.h"

#include <iostream>

using namespace drz::graphics;

class PageRoad : public DisplayPage {
  public :

    PageRoad() {
      
      //set semo mode if no serial
      isDemo = DrzSerial::Get() == nullptr;

      gaugeFuel = new SimpleVerticalGaugeWidget(0, 80, 80, 80, 0, 100, 0);
      gaugeFuel->SetBarRect({36, 19, 24, 48});
      
      gaugeTemp = new SimpleVerticalGaugeWidget(0, 160, 80, 80, 0, 110, 0);
      gaugeTemp->SetBarRect({36, 19, 24, 48});

      road = new RoadWidget(81, 81, 160, 160);
      compass = new CompassWidget(121, 0, 80, 80); 
      distance = new DistanceWidget(240, 0, 80, 80);

      if(isDemo) {
        gaugeFuel->SetValue(50);
        gaugeTemp->SetValue(84);
        distance->SetTotalDistance(12345);
        distance->SetTripDistance(123.4);
        road->SetSpeed(0);
        compass->SetHeading(0);
      }

      //engine = DrzEngine::Get();
      gfx = DrzGraphics::Get();
      inputs = DrzInputs::Get();
    }

    void ReadInputs() override {
      //TODO
      if(inputs->GetKey(Key::E).isPressed) {
        std::cout << "E pressed" << std::endl;
      }
    }

    void Load() override {
      gaugeFuel->SetMask(new Sprite(FUELGAUGEMASK_SPRITE_WIDTH, FUELGAUGEMASK_SPRITE_HEIGHT, FUELGAUGEMASK_SPRITE_DATA));
      gaugeTemp->SetMask(new Sprite(TEMPGAUGEMASK_SPRITE_WIDTH, TEMPGAUGEMASK_SPRITE_HEIGHT, TEMPGAUGEMASK_SPRITE_DATA));
      AddWidget(road);
      AddWidget(gaugeFuel);
      AddWidget(gaugeTemp);
      AddWidget(compass);
      AddWidget(distance);
    }

    void Update(float fElapsedTime) override {

      gfx->Clear(BLACK);
      
      if(!isDemo) {
        return;
      }

      auto currentHeading = compass->GetHeading(); 
      auto currentSpeed = road->GetSpeed();
      auto currentTotalDistance = distance->GetTotalDistance();
      auto currentTripDistance = distance->GetTripDistance();

      //compass increment heading forever
      demoIncrementHeadingCurrent += fElapsedTime;
      if(demoIncrementHeadingCurrent>=demoIncrementHeadingEvery) {
        demoIncrementHeadingCurrent = 0.0f;
        currentHeading+=1;
        if(currentHeading>=360) {
          currentHeading = 0;
        }
        compass->SetHeading(currentHeading);
      }

      //speed increase until max
      if(currentSpeed < demoMaxSpeed) {
        demoIncrementSpeedCurrent += fElapsedTime;
        if(demoIncrementSpeedCurrent>=demoIncrementSpeedEvery) { 
          demoIncrementSpeedCurrent = 0.0f;
          currentSpeed+=1;
          if(currentSpeed>demoMaxSpeed) {
            std::cout << "Demo Max speed reached" << std::endl;
            currentSpeed = demoMaxSpeed;
          }
          road->SetSpeed(currentSpeed);
        }
      }

      //distance trip increment
      demoIncrementTripDistanceCurrent += fElapsedTime;
      if(demoIncrementTripDistanceCurrent >= demoIncrementTripDistanceEvery) {
        demoIncrementTripDistanceCurrent = 0.0f;
        currentTripDistance += 0.1f;
        distance->SetTripDistance(currentTripDistance);
      }

      //distance total increment
      demoIncrementTotalDistanceCurrent += fElapsedTime;
      if(demoIncrementTotalDistanceCurrent >= demoIncrementTotalDistanceEvery) {
        demoIncrementTotalDistanceCurrent = 0.0f;
        currentTotalDistance += 1;
        distance->SetTotalDistance(currentTotalDistance);
      }
    }

    void Render() override {
    }   

  private:
    SimpleVerticalGaugeWidget* gaugeFuel;
    SimpleVerticalGaugeWidget* gaugeTemp;
    RoadWidget* road;
    CompassWidget* compass;
    DistanceWidget* distance;

    IDrzGraphics* gfx;
    IDrzInputs* inputs; 

    bool isDemo = true;

    float demoIncrementHeadingCurrent = 0.0f;
    float demoIncrementHeadingEvery = 0.1f;

    float demoIncrementSpeedCurrent = 0.0f;
    float demoIncrementSpeedEvery = 0.1f;

    float demoMaxSpeed = 100.0f;

    float demoIncrementTotalDistanceCurrent = 0.0f;
    float demoIncrementTotalDistanceEvery = 10.0;

    float demoIncrementTripDistanceCurrent = 0.0f;
    float demoIncrementTripDistanceEvery = demoIncrementTotalDistanceEvery / 10.0f;



};