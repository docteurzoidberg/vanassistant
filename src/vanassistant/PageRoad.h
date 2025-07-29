#pragma once

#include <DrzGraphics.h>
#include <DrzInputs.h>

#include "DisplayPageManager.h"

#include "DrzSerial.h"

#include "pages/common/widgets/Starfield.h"

#include "pages/road/widgets/DistanceWidget.h"
#include "pages/road/widgets/IconWidget.h"
#include "pages/road/widgets/SimpleGaugeWidget.h"
#include "pages/road/widgets/RoadWidget.h"
#include "pages/road/widgets/CompassWidget.h"
#include "pages/road/widgets/IconWidget.h"
#include "pages/road/widgets/SpeedometerWidget.h"

#include "pages/road/sprites/fuelgaugemask2.h"
#include "pages/road/sprites/tempgaugemask2.h"

#include <iostream>

using namespace drz::graphics;

class PageRoad : public DisplayPage {
  public :

    PageRoad();


    void ReadInputs() override;

    void Load() override;
    void Activate(DisplayPage* lastPage) override;
    void Update(float fElapsedTime) override;
    void Render() override;

  private:

    Starfield* starfield;
    RoadWidget* road;  

    SimpleVerticalGaugeWidget* gaugeFuel;
    SimpleVerticalGaugeWidget* gaugeTemp;
    CompassWidget* compass;
    DistanceWidget* distance;
    SpeedometerWidget* speedometer;

    IconWidget* iconPreheat;
    IconWidget* iconNotcharging;
    IconWidget* iconOil;
    IconWidget* iconTurnSignals;
    IconWidget* iconHighBeam;
    IconWidget* iconLowBeam;
    IconWidget* iconWarnings;
    IconWidget* iconProblem;

    IDrzGraphics* gfx;
    IDrzInputs* inputs; 

    bool isDemo = true;

    float demoIncrementHeadingCurrent = 0.0f;
    float demoIncrementHeadingEvery = 0.1f;

    float demoIncrementSpeedCurrent = 0.0f;
    float demoIncrementSpeedEvery = 0.1f;

    float demoMaxSpeed = 88.0f;

    float demoIncrementTotalDistanceCurrent = 0.0f;
    float demoIncrementTotalDistanceEvery = 10.0;

    float demoIncrementTripDistanceCurrent = 0.0f;
    float demoIncrementTripDistanceEvery = demoIncrementTotalDistanceEvery / 10.0f;
};