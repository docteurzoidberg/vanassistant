#include "PageRoad.h"
#include <iostream>


PageRoad::PageRoad() : DisplayPage("road") {

    //engine = DrzEngine::Get();
  gfx = DrzGraphics::Get();
  inputs = DrzInputs::Get();

  
  //set semo mode if no serial
  isDemo = DrzSerial::Get() == nullptr;

  starfield = new Starfield(0, 0, gfx->GetScreenWidth(), gfx->GetScreenHeight(), 750);

  gaugeFuel = new SimpleVerticalGaugeWidget(320-40, 33, 38, 53, 0, 100, 0);
  gaugeFuel->SetBarRect({19, 5, 16, 45});
  
  gaugeTemp = new SimpleVerticalGaugeWidget(320-40, 87, 38, 53, 0, 110, 0);
  gaugeTemp->SetBarRect({19, 5, 16, 45});

  road = new RoadWidget(81, 81, 160, 160);
  compass = new CompassWidget(81, 0, 160, 32); 

  distance = new DistanceWidget(0, 60, 88, 42);

  speedometer = new SpeedometerWidget(100, 50, 120, 70);
  iconPreheat = new IconWidget(2, 240-8-88, 22, 22, true);
  iconNotcharging = new IconWidget(2, 240-6-66, 22, 22, true);
  iconOil = new IconWidget(2, 240-4-44, 22, 22, true);
  iconTurnSignals = new IconWidget(2, 240-2-22, 22, 22, true);
  iconHighBeam = new IconWidget(295, 240-8-88, 22, 22, true);
  iconLowBeam = new IconWidget(295, 240-6-66, 22, 22, true);
  iconWarnings = new IconWidget(295, 240-4-44, 22, 22, true);
  iconProblem = new IconWidget(295, 240-2-22, 22, 22, true);

  if(isDemo) {
    gaugeFuel->SetValue(50);
    gaugeTemp->SetValue(84);
    distance->SetTotalDistance(12345);
    distance->SetTripDistance(0);
    road->SetSpeed(0);
    compass->SetHeading(0);
  }
}

void PageRoad::ReadInputs() {
  if(
    inputs->GetKey(Key::K1).isPressed || 
    inputs->GetKey(Key::K2).isPressed || 
    inputs->GetKey(Key::K3).isPressed || 
    inputs->GetKey(Key::K4).isPressed
  ) {
    DisplayPageManager::GoToPage("menu");
  }
}

void PageRoad::Load() {
  gaugeFuel->SetMask(new Sprite(FUELGAUGEMASK2_SPRITE_WIDTH, FUELGAUGEMASK2_SPRITE_HEIGHT, FUELGAUGEMASK2_SPRITE_DATA));
  gaugeTemp->SetMask(new Sprite(TEMPGAUGEMASK2_SPRITE_WIDTH, TEMPGAUGEMASK2_SPRITE_HEIGHT, TEMPGAUGEMASK2_SPRITE_DATA));

  AddWidget(starfield);
  AddWidget(road);
  AddWidget(compass);
  AddWidget(distance);
  AddWidget(gaugeFuel);
  AddWidget(gaugeTemp);
  AddWidget(speedometer);

  AddWidget(iconPreheat);
  AddWidget(iconNotcharging);
  AddWidget(iconOil);
  AddWidget(iconTurnSignals);
  AddWidget(iconHighBeam);
  AddWidget(iconLowBeam);
  AddWidget(iconWarnings);
  AddWidget(iconProblem);
}

void PageRoad::Activate(DisplayPage* lastPage) {
}

void PageRoad::Update(float fElapsedTime) {

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
      speedometer->SetSpeed(currentSpeed);
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

void PageRoad::Render() {
}   