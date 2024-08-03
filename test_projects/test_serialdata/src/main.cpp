#include <DrzEngine_PGE.h>
#include <DrzSerial.h>

#include "../../../src/vanassistant/fonts/Solid_Mono8pt7b.h"
#include "../../../src/vanassistant/SerialProtocol.h"

#include "DrzGraphics.h"
#include "DrzSerial_Linux.h"
#include "IDrzSerial.h"

#include <IDrzEngine.h>

#include <cstring>
#include <iostream>
#include <string>

#define OLC_PGEX_QUICKGUI
#include "olcPGEX_QuickGUI.h"

#include "Counter.h"
#include "Gauge.h"

// Function to round a float up to N decimal places
std::string strRoundUpToNDecimals(float value, int N) {
    // Calculate the multiplier
    float multiplier = std::pow(10.0f, N);
    // Multiply the value by the multiplier and apply ceil
    float roundedValue = std::ceil(value * multiplier) / multiplier;
    
    // Use stringstream to format the float with fixed number of decimal places
    std::stringstream stream;
    stream << std::fixed << std::setprecision(N) << roundedValue;
    return stream.str();
}

class TestSerialData : public IDrzEngineApp, public ISerialProtocolReceiver {

  public:
    std::string serial_port="/dev/ttyUSB0";
    std::string baud_rate="115200";
    olc::QuickGUI::Manager guiManager;

    DrzEngine_PGE* drzpge;
    olc::PixelGameEngine* pge;

    TestSerialData(std::string serial_port, int baud_rate) {

      drzpge = new DrzEngine_PGE(640, 480, 1);
      pge = drzpge->pge;
    
      pge->sAppName = serial_port + "@" +  std::to_string(baud_rate);

      gfx = DrzGraphics::Get();

      //Load font
      gfx->LoadFont("solidmono8", const_cast<font*>(&Solid_Mono8pt7b));
      gfx->SetFont("solidmono8");
    }

    ~TestSerialData() {
      //SerialProtocol::Close();
    }

    void Setup() override {


      //initialize data
      data.fuelgauge = 100;           // 100/255=39%
      data.battery = 134;             // /10 = 134/10=13.4V
      data.speed = 88;                // *1   = 88 km/h
      data.rpm = 30;                  // *100 = 3000 RPM
      data.coolant_temp = 901;         // /10 = 90.1°C
      data.odometer = 123456;         // *1 = 12345 km
      data.trip = 4238;
      data.lamp_preheat = false;
      data.lamp_notcharging = false;
      data.lamp_oil = false;
      data.lamp_turnsignals = true;
      data.lamp_highbeam = false;
      data.lamp_lowbeam = false;
      data.lamp_warnings = false;

      //load sprites
      LoadSprites();

      SetupGUI();
      DataToGUI();

    }

    void Update(float fElapsedTime) override {
      

      // F1: show console
      if(pge->GetKey(olc::Key::F1).bPressed) {
        pge->ConsoleShow(olc::Key::ESCAPE, false);
      }

      // F: toggle show fps
      if(pge->GetKey(olc::Key::F).bPressed) {
        drawFPS = !drawFPS;
      }

      //M: toggle mask
      if(pge->GetKey(olc::Key::M).bPressed) {
        drawMask = !drawMask;
      }

      bool bGUIDataUpdated = false;

      bool dbgGuiUpdates = false;

      if(guiSliderFuel->bHeld) {
        data.fuelgauge = guiSliderFuel->fValue;
        if(dbgGuiUpdates) std::cout << "sliderFuel changed" << std::endl;
        guiLabelFuelValue->sText = std::to_string((int)((data.fuelgauge/255.0f)*100.0f)) + "%";
        bGUIDataUpdated = true;
      }

      if(guiSliderSpeed->bHeld) {
        data.speed = guiSliderSpeed->fValue;
        if(dbgGuiUpdates) std::cout << "sliderSpeed changed" << std::endl;
        guiLabelSpeedValue->sText = std::to_string(data.speed) + "km/h";
        bGUIDataUpdated = true;
      }

      if(guiSliderBat->bHeld) {
        data.battery = guiSliderBat->fValue;
        if(dbgGuiUpdates) std::cout << "sliderBat changed" << std::endl;
        guiLabelBatValue->sText = strRoundUpToNDecimals(data.battery/10.0f,1) + "V";
        bGUIDataUpdated = true;
      }

      if(guiSliderRPM->bHeld) {
        data.rpm = guiSliderRPM->fValue;
        if(dbgGuiUpdates) std::cout << "sliderRPM changed" << std::endl;
        guiLabelRPMValue->sText = std::to_string(data.rpm*100) + "RPM";
        bGUIDataUpdated = true;
      }

      if(guiSliderCoolantTemp->bHeld) {
        data.coolant_temp = guiSliderCoolantTemp->fValue;
        if(dbgGuiUpdates) std::cout << "sliderCoolantTemp changed" << std::endl;
        guiLabelCoolantTempValue->sText = strRoundUpToNDecimals(data.coolant_temp/10.0f,1) + "C";
        bGUIDataUpdated = true;
      }

      if(guiTextBoxOdometer->sText != sGuiTextBoxOdometerOldValue) { 
        std::cerr << "txtOdometer changed. oldValue = " << sGuiTextBoxOdometerOldValue << " newValue = " << guiTextBoxOdometer->sText << std::endl;
        try {
          data.odometer = std::stoi(guiTextBoxOdometer->sText);  
          if(std::to_string(data.odometer) != guiTextBoxOdometer->sText)
            guiTextBoxOdometer->sText = std::to_string(data.odometer);
          sGuiTextBoxOdometerOldValue = guiTextBoxOdometer->sText;
          guiLabelOdemeterValue->sText = guiTextBoxOdometer->sText;
          counterTotalDistance->SetCounter(data.odometer);
          bGUIDataUpdated = true;
        } catch (std::invalid_argument& e) {
          std::cerr << "Invalid odometer value" << std::endl;
          guiTextBoxOdometer->sText = sGuiTextBoxOdometerOldValue;
          guiManager.Update(pge);
        }
      }

      if(guiTextBoxTrip->sText != sGuiTextBoxTripOldValue) {
        std::cerr << "txtTrip changed. oldValue = " << sGuiTextBoxTripOldValue << " newValue = " << guiTextBoxTrip->sText << std::endl;
        try {
          data.trip = (uint32_t) (std::stof(guiTextBoxTrip->sText)*10);
          //if data trip to string is different from gui trip, update gui trip
          if(std::to_string(data.trip) != guiTextBoxTrip->sText)
            guiTextBoxTrip->sText = strRoundUpToNDecimals(data.trip/10.0f, 1);
          sGuiTextBoxTripOldValue = guiTextBoxTrip->sText;
          guiLabelTripValue->sText = guiTextBoxTrip->sText;
          counterDistance->SetCounter(data.trip);
          bGUIDataUpdated = true;
        } catch(std::invalid_argument& e) {
          std::cerr << "Invalid trip value" << std::endl;
          guiTextBoxTrip->sText = sGuiTextBoxTripOldValue;
          guiManager.Update(pge);
        }
      }

      if(guiCheckBoxPreheat->bPressed) {
        data.lamp_preheat = guiCheckBoxPreheat->bChecked;
        if(dbgGuiUpdates) std::cout << "chkPreheat pressed" << std::endl;
        bGUIDataUpdated = true;
      }

      if(guiCheckBoxNotCharging->bPressed) {
        data.lamp_notcharging = guiCheckBoxNotCharging->bChecked;
        if(dbgGuiUpdates) std::cout << "chkNotCharging pressed" << std::endl;
        bGUIDataUpdated = true;
      }

      if(guiCheckBoxOil->bPressed) {
        data.lamp_oil = guiCheckBoxOil->bChecked;
        if(dbgGuiUpdates) std::cout << "chkOil pressed" << std::endl;
        bGUIDataUpdated = true;
      }

      if(guiCheckBoxTurnSignals->bPressed) {
        data.lamp_turnsignals = guiCheckBoxTurnSignals->bChecked;
        fLampTurnSignalBlinkEvery = 0.0f; //reset blink timer
        if(dbgGuiUpdates) std::cout << "chkTurnSignals pressed" << std::endl;
        bGUIDataUpdated = true;
      }

      if(guiCheckBoxHighBeam->bPressed) {
        data.lamp_highbeam = guiCheckBoxHighBeam->bChecked;
        if(dbgGuiUpdates) std::cout << "chkHighBeam pressed" << std::endl;
        bGUIDataUpdated = true;
      }

      if(guiCheckBoxLowBeam->bPressed) {
        data.lamp_lowbeam = guiCheckBoxLowBeam->bChecked;
        if(dbgGuiUpdates) std::cout << "chkLowBeam pressed" << std::endl;
        bGUIDataUpdated = true;
      }

      if(guiCheckBoxWarnings->bPressed) {
        data.lamp_warnings = guiCheckBoxWarnings->bChecked;
        fLampWarningsBlinkEvery = 0.0f; //reset blink timer
        if(dbgGuiUpdates) std::cout << "chkWarnings pressed" << std::endl;
        bGUIDataUpdated = true;
      }

      if(guiCheckBoxProblem->bPressed) {
        data.lamp_problem = guiCheckBoxProblem->bChecked;
        if(dbgGuiUpdates) std::cout << "chkProblem pressed" << std::endl;
        bGUIDataUpdated = true;
      }


      if(SerialProtocol::ReadSerial() && !bGUIDataUpdated) {
        //update gui with new data
        DataToGUI();
        bGUIDataUpdated = false;
      }
      // We must update the manager at some point each frame. Values of controls
		  // are only valid AFTER this call to update()
      guiManager.Update(pge);

      if(bGUIDataUpdated) {
        bGUIDataUpdated = false;
        SendJ7DashboardData();
      }

      //update blinkers
      if(data.lamp_warnings) {
        fLampWarningsBlinkEvery -= fElapsedTime;
        if(fLampWarningsBlinkEvery <= 0) {
          fLampWarningsBlinkEvery = 0.3f+fLampWarningsBlinkEvery;
          bLampWarningsBlink = !bLampWarningsBlink;
        }
      }
      if(data.lamp_turnsignals) {
        fLampTurnSignalBlinkEvery -= fElapsedTime;
        if(fLampTurnSignalBlinkEvery <= 0) {
          fLampTurnSignalBlinkEvery = 0.3f+fLampTurnSignalBlinkEvery;
          bTurnSignalBlink = !bTurnSignalBlink;
        }
      }

      //black background
      pge->Clear(olc::BLACK);

      //Draw GUI//draw gui elements
      guiManager.DrawDecal(pge);
      counterTotalDistance->Render(185,164);
      counterDistance->Render(193, 192);

      //Draw lamp indicators
      DrawLamps();

      //Draw dashboard mask
      pge->SetPixelMode(olc::Pixel::MASK);
      if(drawMask)
        pge->DrawSprite(30, gfx->GetScreenHeight()-110, sprInstrumentMask);
      pge->SetPixelMode(olc::Pixel::NORMAL);

      //Draw FPS
      if(drawFPS) {
        //text size:
        auto text = std::to_string(pge->GetFPS());
        auto size = gfx->GetTextBounds(text, 0, 0);
        gfx->DrawText(text, gfx->GetScreenWidth()-size.w-4, 14, YELLOW);
      }
      
    }

    bool Command(const std::string& command) override {
      SendJ7TextData(command);
      return true;
    }

    bool Data(const uint8_t* data, size_t size) override {
      return true;
    }

  protected:

    //Control labels
    olc::QuickGUI::Label* guiLabelFuel = nullptr;
    olc::QuickGUI::Label* guiLabelFuelMinValue = nullptr;
    olc::QuickGUI::Label* guiLabelFuelMaxValue = nullptr;
    olc::QuickGUI::Label* guiLabelFuelValue = nullptr;

    olc::QuickGUI::Label* guiLabelSpeed = nullptr;
    olc::QuickGUI::Label* guiLabelSpeedMinValue = nullptr;
    olc::QuickGUI::Label* guiLabelSpeedMaxValue = nullptr;
    olc::QuickGUI::Label* guiLabelSpeedValue = nullptr;

    olc::QuickGUI::Label* guiLabelBat = nullptr;
    olc::QuickGUI::Label* guiLabelBatMinValue = nullptr;
    olc::QuickGUI::Label* guiLabelBatMaxValue = nullptr;
    olc::QuickGUI::Label* guiLabelBatValue = nullptr;

    olc::QuickGUI::Label* guiLabelRPM = nullptr;
    olc::QuickGUI::Label* guiLabelRPMMinValue = nullptr;
    olc::QuickGUI::Label* guiLabelRPMMaxValue = nullptr;
    olc::QuickGUI::Label* guiLabelRPMValue = nullptr;

    olc::QuickGUI::Label* guiLabelCoolantTemp = nullptr;
    olc::QuickGUI::Label* guiLabelCoolantTempMinValue = nullptr;
    olc::QuickGUI::Label* guiLabelCoolantTempMaxValue = nullptr;
    olc::QuickGUI::Label* guiLabelCoolantTempValue = nullptr;
    
    olc::QuickGUI::Label* guiLabelOdometer = nullptr;
    olc::QuickGUI::Label* guiLabelOdemeterValue = nullptr;

    olc::QuickGUI::Label* guiLabelTrip = nullptr;
    olc::QuickGUI::Label* guiLabelTripValue = nullptr;

    olc::QuickGUI::Label* guiLabelLampPreheat = nullptr;
    olc::QuickGUI::Label* guiLabelLampNotCharging = nullptr;
    olc::QuickGUI::Label* guiLabelLampOil = nullptr;
    olc::QuickGUI::Label* guiLabelLampTurnSignals = nullptr;
    olc::QuickGUI::Label* guiLabelLampHighBeam = nullptr;
    olc::QuickGUI::Label* guiLabelLampLowBeam = nullptr;
    olc::QuickGUI::Label* guiLabelLampWarnings = nullptr;
    olc::QuickGUI::Label* guiLabelLampProblem = nullptr;

    //Sliders
    olc::QuickGUI::Slider* guiSliderFuel = nullptr;
    olc::QuickGUI::Slider* guiSliderSpeed = nullptr;
    olc::QuickGUI::Slider* guiSliderBat = nullptr;
    olc::QuickGUI::Slider* guiSliderRPM = nullptr;
    olc::QuickGUI::Slider* guiSliderCoolantTemp = nullptr;

    //TextBoxes
    olc::QuickGUI::TextBox* guiTextBoxOdometer = nullptr;
    olc::QuickGUI::TextBox* guiTextBoxTrip = nullptr;

    //CheckBoxes
    olc::QuickGUI::ImageCheckBox* guiCheckBoxPreheat = nullptr;
    olc::QuickGUI::ImageCheckBox* guiCheckBoxNotCharging = nullptr;
    olc::QuickGUI::ImageCheckBox* guiCheckBoxOil = nullptr;
    olc::QuickGUI::ImageCheckBox* guiCheckBoxTurnSignals = nullptr;
    olc::QuickGUI::ImageCheckBox* guiCheckBoxHighBeam = nullptr;
    olc::QuickGUI::ImageCheckBox* guiCheckBoxLowBeam = nullptr;
    olc::QuickGUI::ImageCheckBox* guiCheckBoxWarnings = nullptr;
    olc::QuickGUI::ImageCheckBox* guiCheckBoxProblem = nullptr;

    olc::Sprite* sprCounterSpritesheet;
    olc::Sprite* sprInstrumentMask;
    olc::Renderable sprLampPreheat;
    olc::Renderable sprLampNotCharging;
    olc::Renderable sprLampOil;
    olc::Renderable sprLampTurnSignals;
    olc::Renderable sprLampHighBeam;
    olc::Renderable sprLampLowBeam;
    olc::Renderable sprLampWarnings;
    olc::Renderable sprLampProblem;

  private:
    IDrzSerial* serial;
    IDrzGraphics* gfx;
    SerialProtocol* serialProtocol; 

    Counter* counterTotalDistance;
    Counter* counterDistance;
    J7DashboardPacketData data;
    bool needToSendData = false;
    bool drawFPS = false;
    bool drawMask = true;
    bool logData = true;
    int bauds=-1;

    std::chrono::time_point<std::chrono::system_clock> lastSendTime = std::chrono::system_clock::now(); 

    bool throttleData = true;
    float fSendDataEvery = 0.1f;

    float fLampWarningsBlinkEvery = 0.3f;
    float fLampTurnSignalBlinkEvery = 0.3f;

    bool bTurnSignalBlink = false;
    bool bLampWarningsBlink = false;

    std::string sGuiTextBoxOdometerOldValue = "";
    std::string sGuiTextBoxTripOldValue = "";

    const Color lampColorOrangeOn = Color(230, 148, 25);
    const Color lampColorOrangeOff = Color(80, 50, 0);

    const Color lampColorRedOn = Color(230, 0, 0);
    const Color lampColorRedOff = Color(80, 0, 0);

    const Color lampColorGreenOn = Color(0, 230, 0);
    const Color lampColorGreenOff = Color(0, 80, 0);

    const Color lampColorBlueOn = Color(0, 0, 230);
    const Color lampColorBlueOff = Color(0, 0, 80);

    void LoadSprites() {
      sprCounterSpritesheet = new olc::Sprite("sprites/counterdigits.png");
      sprInstrumentMask = new olc::Sprite("sprites/dashboard_facemask.png");
      sprLampPreheat.Load("sprites/lamp_preheat.png");
      sprLampNotCharging.Load("sprites/lamp_notcharging.png");
      sprLampOil.Load("sprites/lamp_oil.png");
      sprLampTurnSignals.Load("sprites/lamp_turnsignals.png");
      sprLampHighBeam.Load("sprites/lamp_highbeam.png");
      sprLampLowBeam.Load("sprites/lamp_lowbeam.png");
      sprLampWarnings.Load("sprites/lamp_warnings.png");
      sprLampProblem.Load("sprites/lamp_problem.png");
    }

    void SetupGUI() {


      counterTotalDistance = new Counter(pge, 6, sprCounterSpritesheet, {8,10});
      counterTotalDistance->SetCounter(data.odometer);
      
      counterDistance = new Counter(pge, 4, sprCounterSpritesheet, {8,10});
      counterDistance->SetCounter(data.trip);

      float lx = 4.0f;
      float ly = 2.0f;

      auto odometerText = std::to_string(data.odometer);
      auto tripText = strRoundUpToNDecimals(data.trip/10.0f, 1);

      // Labels 

      guiLabelFuel = new olc::QuickGUI::Label(guiManager, "Fuel:", { lx, ly }, { 70.0f, 16.0f });
      guiLabelFuelMinValue = new olc::QuickGUI::Label(guiManager, "0", { lx+156,  ly }, { 10.0f, 16.0f });
      guiLabelFuelMaxValue = new olc::QuickGUI::Label(guiManager, "100%", { lx+252, ly }, { 10.0f, 16.0f });
      guiLabelFuelValue = new olc::QuickGUI::Label(guiManager, "50%", { lx+74, ly }, { 70.0f, 16.0f });

      guiLabelSpeed = new olc::QuickGUI::Label(guiManager, "Speed:", { lx, ly+18 }, { 70.0f, 16.0f });
      guiLabelSpeedMinValue = new olc::QuickGUI::Label(guiManager, "0", { lx+156, ly+18 }, { 10.0f, 16.0f });
      guiLabelSpeedMaxValue = new olc::QuickGUI::Label(guiManager, "140km/h", { lx+252, ly+18 }, { 10.0f, 16.0f });
      guiLabelSpeedValue = new olc::QuickGUI::Label(guiManager, "88km/h", { lx+74, ly+18 }, { 70.0f, 16.0f });

      guiLabelBat = new olc::QuickGUI::Label(guiManager, "Bat:", { lx, ly+36 }, { 70.0f, 16.0f });
      guiLabelBatMinValue = new olc::QuickGUI::Label(guiManager, "0", { lx+156, ly+36}, { 10.0f, 16.0f });
      guiLabelBatMaxValue = new olc::QuickGUI::Label(guiManager, "20.0 V", { lx+252, ly+36 }, { 10.0f, 16.0f });
      guiLabelBatValue = new olc::QuickGUI::Label(guiManager, "13.4V", { lx+74, ly+36 }, { 70.0f, 16.0f });

      guiLabelRPM = new olc::QuickGUI::Label(guiManager, "RPM:", { lx, ly+54 }, { 70.0f, 16.0f });
      guiLabelRPMMinValue = new olc::QuickGUI::Label(guiManager, "0", { lx+156, ly+54 }, { 10.0f, 16.0f });
      guiLabelRPMMaxValue = new olc::QuickGUI::Label(guiManager, "7000RPM", { lx+252, ly+54 }, { 10.0f, 16.0f });
      guiLabelRPMValue = new olc::QuickGUI::Label(guiManager, "3000RPM", { lx+74, ly+54 }, { 70.0f, 16.0f });

      guiLabelCoolantTemp = new olc::QuickGUI::Label(guiManager, "Water Temp:", { lx, ly+72 }, { 70.0f, 16.0f });
      guiLabelCoolantTempMinValue = new olc::QuickGUI::Label(guiManager, "0", { lx+156, ly+72 }, { 10.0f, 16.0f });
      guiLabelCoolantTempMaxValue = new olc::QuickGUI::Label(guiManager, "140C", { lx+252, ly+72  }, { 10.0f, 16.0f });
      guiLabelCoolantTempValue = new olc::QuickGUI::Label(guiManager, "90C", { lx+74, ly+72 }, { 70.0f, 16.0f });

      guiLabelOdometer = new olc::QuickGUI::Label(guiManager, "Odometer:", { lx, ly+90  }, { 70.0f, 16.0f });
      guiLabelOdemeterValue = new olc::QuickGUI::Label(guiManager, odometerText, { lx+74, ly+90 }, { 70.0f, 16.0f });

      guiLabelTrip = new olc::QuickGUI::Label(guiManager, "Trip:", { lx, ly+108 }, { 70.0f, 16.0f });
      guiLabelTripValue = new olc::QuickGUI::Label(guiManager, tripText, { lx+74, ly+108 }, { 70.0f, 16.0f });

      //Sliders

      guiSliderFuel = new olc::QuickGUI::Slider(guiManager,{ lx+180, ly+8 }, { lx+240, ly+8 }, 0, 255, 128);
      guiSliderSpeed = new olc::QuickGUI::Slider(guiManager,{ lx+180, ly+26 }, { lx+240, ly+26 }, 0, 140, 88);
      guiSliderBat = new olc::QuickGUI::Slider(guiManager, { lx+180, ly+44}, { lx+240, ly+44}, 0, 200, 123);
      guiSliderRPM = new olc::QuickGUI::Slider(guiManager, { lx+180, ly+62 }, { lx+240, ly+62 }, 0, 100, 30);
      guiSliderCoolantTemp = new olc::QuickGUI::Slider(guiManager, { lx+180, ly+80 }, { lx+240, ly+80}, 0, 1400, 55);

      //TextBoxes

      guiTextBoxOdometer = new olc::QuickGUI::TextBox(guiManager, odometerText, { lx+171, ly+90 }, { 80.0f, 16.0f } );
      guiTextBoxTrip = new olc::QuickGUI::TextBox(guiManager, tripText, { lx+171, ly+108 }, { 80.0f, 16.0f } );

      //CheckBoxes

      guiCheckBoxPreheat = new olc::QuickGUI::ImageCheckBox(guiManager, sprLampPreheat, false, { lx-2, ly+140 }, { 24.0f, 24.0f });
      guiCheckBoxNotCharging = new olc::QuickGUI::ImageCheckBox(guiManager, sprLampNotCharging, false, { lx-2, ly+164 }, { 24.0f, 24.0f });
      guiCheckBoxOil = new olc::QuickGUI::ImageCheckBox(guiManager, sprLampOil, false, { lx-2, ly+188 }, { 24.0f, 24.0f });
      guiCheckBoxWarnings= new olc::QuickGUI::ImageCheckBox(guiManager, sprLampWarnings, false, { lx-2, ly+212 }, { 24.0f, 24.0f });
      
      guiCheckBoxTurnSignals = new olc::QuickGUI::ImageCheckBox(guiManager, sprLampTurnSignals, false, { lx+290, ly+140 }, { 24.0f, 24.0f });
      guiCheckBoxHighBeam = new olc::QuickGUI::ImageCheckBox(guiManager, sprLampHighBeam, false, { lx+290, ly+164 }, { 24.0f, 24.0f });
      guiCheckBoxLowBeam = new olc::QuickGUI::ImageCheckBox(guiManager, sprLampLowBeam, false, { lx+290, ly+188 }, { 24.0f, 24.0f });
      guiCheckBoxProblem = new olc::QuickGUI::ImageCheckBox(guiManager, sprLampProblem, false, { lx+290, ly+212 }, { 24.0f, 24.0f });
    
      // Customize how the labels look

      guiLabelBat->nAlign = olc::QuickGUI::Label::Alignment::Right;		
      guiLabelBatMaxValue->nAlign = olc::QuickGUI::Label::Alignment::Left;
      guiLabelBatValue->bHasBackground = true;

      guiLabelFuel->nAlign = olc::QuickGUI::Label::Alignment::Right;
      guiLabelFuelMaxValue->nAlign = olc::QuickGUI::Label::Alignment::Left;
      guiLabelFuelValue->bHasBackground = true;

      guiLabelSpeed->nAlign = olc::QuickGUI::Label::Alignment::Right;
      guiLabelSpeedMaxValue->nAlign = olc::QuickGUI::Label::Alignment::Left;
      guiLabelSpeedValue->bHasBackground = true;

      guiLabelRPM->nAlign = olc::QuickGUI::Label::Alignment::Right;
      guiLabelRPMMaxValue->nAlign = olc::QuickGUI::Label::Alignment::Left;
      guiLabelRPMValue->bHasBackground = true;

      guiLabelCoolantTemp->nAlign = olc::QuickGUI::Label::Alignment::Right;
      guiLabelCoolantTempMaxValue->nAlign = olc::QuickGUI::Label::Alignment::Left;
      guiLabelCoolantTempValue->bHasBackground = true;

      guiLabelOdometer->nAlign = olc::QuickGUI::Label::Alignment::Right;
      guiLabelOdemeterValue->bHasBackground = true;

      guiLabelTrip->nAlign = olc::QuickGUI::Label::Alignment::Right;
      guiLabelTripValue->bHasBackground = true;

      guiManager.colNormal = olc::MAGENTA;
      guiManager.colHover = olc::WHITE;
      guiManager.colClick = olc::BLACK;

    }

    void DataToGUI() {
      guiCheckBoxHighBeam->bChecked = data.lamp_highbeam;
      guiCheckBoxLowBeam->bChecked = data.lamp_lowbeam;
      guiCheckBoxNotCharging->bChecked = data.lamp_notcharging;
      guiCheckBoxOil->bChecked = data.lamp_oil;
      guiCheckBoxPreheat->bChecked = data.lamp_preheat;
      guiCheckBoxProblem->bChecked = data.lamp_problem;
      guiCheckBoxTurnSignals->bChecked = data.lamp_turnsignals;
      guiCheckBoxWarnings->bChecked = data.lamp_warnings;

      auto odometerText = std::to_string(data.odometer);
      auto tripText = strRoundUpToNDecimals(data.trip/10.0f, 1);

      //to avoid trigger a text change when initializing data first time
      sGuiTextBoxOdometerOldValue = odometerText;
      sGuiTextBoxTripOldValue = tripText;

      guiTextBoxOdometer->sText = odometerText;
      guiTextBoxTrip->sText = tripText;

      guiSliderBat->fValue = data.battery;
      guiSliderCoolantTemp->fValue = data.coolant_temp;
      guiSliderFuel->fValue = data.fuelgauge;
      guiSliderRPM->fValue = data.rpm;
      guiSliderSpeed->fValue = data.speed;

      guiLabelSpeedValue->sText = std::to_string(data.speed) + "km/h";
      guiLabelFuelValue->sText = std::to_string((int)((data.fuelgauge/255.0f)*100.0f)) + "%";
      guiLabelBatValue->sText = strRoundUpToNDecimals(data.battery/10.0f,1) + "V";
      guiLabelRPMValue->sText = std::to_string(data.rpm*100) + "RPM";
      guiLabelCoolantTempValue->sText = strRoundUpToNDecimals(data.coolant_temp/10.0f,1) + "C";
      guiLabelOdemeterValue->sText = std::to_string(data.odometer);
      guiLabelTripValue->sText = strRoundUpToNDecimals(data.trip/10.0f,1);
    }
/*
    bool ReadData() {

      //parse data
      J7PacketHeader header;
      int bytesRead = serial->Read();
      if(bytesRead<=0) {
        //no data
        return false;
      }

      if (bytesRead < sizeof(J7PacketHeader)) {
        std::cerr << "Incorrect packet len" << std::endl;
        return false;
      }

      if(!serialProtocol->ReadHeader(serial->read_buf, &header)) {
        std::cerr << "Failed to read header" << std::endl;
        return false;
      }

      switch (header.type) {
        case J7_SAY_TEXT: {
          J7SayTextPacketData data;
          if(serialProtocol->ReadJ7SayTextPacketData(serial->read_buf, &data)) {
            ProcessSayTextPacket(&data);
          } else {
            std::cerr << "Failed to read J7SayTextPacketData" << std::endl;
          }
          break;
        }
        case J7_DASHBOARD: { 
          J7DashboardPacketData data;
          if(serialProtocol->ReadJ7DashboardPacketData(serial->read_buf, &data)) {
            ProcessDashboardPacket(&data);
          } else {
            std::cerr << "Failed to read J7DashboardPacketData. header.len=" << header.len << " bytesRead=" << bytesRead << std::endl;
          }
          break;
        }
        case J7_IMU: {
          J7IMUPacketData data;
          if(serialProtocol->ReadJ7IMUPacketData(serial->read_buf, &data)) {
            ProcessIMUPacket(&data);
          } else {
            std::cerr << "Failed to read J7IMUPacketData" << std::endl;
          }
          break;
        }
        default:
          std::cerr << "Unknown packet type: " << header.type << std::endl;
          return false;
          break;
      }
      return true;
    }
*/
    void ProcessJ7DashboardPacketData(J7DashboardPacketData* data) override {
      this->data = *data;
      needToSendData = true;
      if(logData)
        LogJ7DashboardPacketData(*data, 0);
    }

    void ProcessJ7SayTextPacketData(J7SayTextPacketData* data) override {
      if(logData)
        LogJ7SayTextPacketData(*data, 0);
    }

    void ProcessJ7IMUPacketData(J7IMUPacketData* data) override {
      if(logData)
        LogJ7IMUPacketData(*data, 0);
    }

    std::string to_hex(char c) {
      std::stringstream ss;
      ss << std::hex << std::setw(2) << std::setfill('0') << (int)c;
      return ss.str();
    }

    void LogJ7DashboardPacketData(J7DashboardPacketData data, int way=0) {

      std::string wayPrefix = way ? "->" : "<-";
      std::cout << wayPrefix << (way ? "Sent: " : "Reveived: ") << "J7DashboardPacketData" << std::endl;

      //display each data byte as hex
      std::string hexData = "";
      auto rawData = (char*)&data;
      for(int i=0; i<sizeof(J7DashboardPacketData); i++) {
        hexData += to_hex(rawData[i]) + " ";
      }


      std::cout << wayPrefix << hexData << std::endl;


      std::string strFuel = "Fuel: " + std::to_string((int)((data.fuelgauge/255.0f)*100.0f)) + "%";
      std::string strBat = "Bat: " + strRoundUpToNDecimals(data.battery/10.0f,1) + "V";
      std::string strSpeed = "Speed: " + std::to_string(data.speed) + "km/h";
      std::string strRPM = "RPM: " + std::to_string(data.rpm*100) + "RPM";
      std::string strCoolantTemp = "Coolant temp: " + strRoundUpToNDecimals(data.coolant_temp/10.0f,1) + "C";
      std::string strOdometer = "Odometer: " + std::to_string(data.odometer);
      std::string strTrip = "Trip: " + strRoundUpToNDecimals(data.trip/10.0f,1);
      std::string strLampPreheat = "Lamp preheat: " + std::to_string(data.lamp_preheat);
      std::string strLampNotCharging = "Lamp not charging: " + std::to_string(data.lamp_notcharging);
      std::string strLampOil = "Lamp oil: " + std::to_string(data.lamp_oil);
      std::string strLampTurnSignals = "Lamp turn signals: " + std::to_string(data.lamp_turnsignals);
      std::string strLampHighBeam = "Lamp high beam: " + std::to_string(data.lamp_highbeam);
      std::string strLampLowBeam = "Lamp low beam: " + std::to_string(data.lamp_lowbeam);
      std::string strLampWarnings = "Lamp warnings: " + std::to_string(data.lamp_warnings);
      std::string strLampProblem = "Lamp problem: " + std::to_string(data.lamp_problem);

      std::cout << wayPrefix << strFuel << std::endl;
      std::cout << wayPrefix << strBat << std::endl;
      std::cout << wayPrefix << strSpeed << std::endl;
      std::cout << wayPrefix << strRPM << std::endl;
      std::cout << wayPrefix << strCoolantTemp << std::endl;
      std::cout << wayPrefix << strOdometer << std::endl;
      std::cout << wayPrefix << strTrip << std::endl;
      std::cout << wayPrefix << strLampPreheat << std::endl;
      std::cout << wayPrefix << strLampNotCharging << std::endl;
      std::cout << wayPrefix << strLampOil << std::endl;
      std::cout << wayPrefix << strLampTurnSignals << std::endl;
      std::cout << wayPrefix << strLampHighBeam << std::endl;
      std::cout << wayPrefix << strLampLowBeam << std::endl;
      std::cout << wayPrefix << strLampWarnings << std::endl;
      std::cout << wayPrefix << strLampProblem << std::endl;
    }

    void LogJ7SayTextPacketData(J7SayTextPacketData data, int way=0) {
      std::string wayPrefix = way ? "->" : "<-";
      std::cout << wayPrefix << (way ? "Sent: " : "Reveived: ") << "J7SayTextPacketData" << std::endl;
      std::cout << wayPrefix << "Text: " << data.text << std::endl;
    }

    void LogJ7IMUPacketData(J7IMUPacketData data, int way=0) {
      std::string wayPrefix = way ? "->" : "<-";
      std::cout << wayPrefix << (way ? "Sent: " : "Reveived: ") << "J7IMUPacketData" << std::endl;
      std::cout << wayPrefix << "Accel X: " << data.acc_x << std::endl;
      std::cout << wayPrefix << "Accel Y: " << data.acc_y << std::endl;
      std::cout << wayPrefix << "Accel Z: " << data.acc_z << std::endl;
      std::cout << wayPrefix << "Gyro X: " << data.gyro_x << std::endl;
      std::cout << wayPrefix << "Gyro Y: " << data.gyro_y << std::endl;
      std::cout << wayPrefix << "Gyro Z: " << data.gyro_z << std::endl;
    }

    void SendJ7DashboardData() {
      auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastSendTime).count();
      if(throttleData && elapsedMs < (int)(fSendDataEvery*1000)) {
        //std::cout << "data already sent " << elapsedMs << "ms ago, not sending" << std::endl;
        return;
      }

      SerialProtocol::WriteJ7DashboardPacketData(&data);
      needToSendData = false;
      lastSendTime = std::chrono::system_clock::now();
      if(logData)
        LogJ7DashboardPacketData(data, 1);
    } 

    void SendJ7TextData(std::string text) {
      J7SayTextPacketData data;
      strncpy(data.text, text.c_str(), sizeof(data.text));
      SerialProtocol::WriteJ7SayTextPacketData(&data);
      if(logData)
        LogJ7SayTextPacketData(data, 1);
    }
    
    void DrawLamps() {
      //pre heating
      if(data.lamp_preheat) {
        gfx->FillRect(250, 130, 24, 24, lampColorOrangeOn);
      } else {
        gfx->FillRect(250, 130, 24, 24, lampColorOrangeOff);
      }

      //alarm
      if(data.lamp_problem) {
        gfx->FillRect(150, 130, 24, 24, lampColorRedOn);
      } else {
        gfx->FillRect(150, 130, 24, 24, lampColorRedOff);
      }

      //warnings
      if(data.lamp_warnings && bLampWarningsBlink) {
        gfx->FillRect(250, 200, 40, 40, lampColorRedOn);
      } else {
        gfx->FillRect(250, 200, 40, 40, lampColorRedOff);
      }  

      //turn signals
      if(data.lamp_turnsignals && bTurnSignalBlink) {
        gfx->FillRect(98, 200, 28, 12, lampColorGreenOn);
      } else {
        gfx->FillRect(98, 200, 28, 12, lampColorGreenOff);
      } 

      //not charging
      if(data.lamp_notcharging) {
        gfx->FillRect(65, 186, 20, 20, lampColorRedOn);
      } else {
        gfx->FillRect(65, 186, 20, 20, lampColorRedOff);
      }

      //oil
      if(data.lamp_oil) {
        gfx->FillRect(135, 186, 20, 20, lampColorRedOn);
      } else {
        gfx->FillRect(135, 186, 20, 20, lampColorRedOff);
      }

      //high beam
      if(data.lamp_highbeam) {
        gfx->FillRect(80, 204, 18, 20, lampColorBlueOn);
      } else {
        gfx->FillRect(80, 204, 18, 20, lampColorBlueOff);
      }

      //low beam
      if(data.lamp_lowbeam) {
        gfx->FillRect(122, 204, 18, 20, lampColorGreenOn);
      } else {
        gfx->FillRect(122, 204, 18, 20, lampColorGreenOff);
      }
    }
};

int main(int argc, char* argv[]) {

  std::string serialPort = "";
  for(int i = 0; i < argc; i++) {
    if(std::string(argv[i]) == "--serial") {
      serialPort = argv[i+1];
    }
  }
  int serialBauds = 115200;
  for(int i = 0; i < argc; i++) {
    if(std::string(argv[i]) == "--bauds") {
      //check if baud rate is valid int value
      try {
        serialBauds = std::stoi(argv[i+1]);
      } catch (std::invalid_argument& e) {
        std::cerr << "Cannot parse baud rate" << std::endl;
        exit(1);
      }
    }
  }

  std::cout << "Serial port: " << serialPort << std::endl;
  std::cout << "Baud rate: " << serialBauds << std::endl;

  IDrzSerial* serial = new DrzSerial_Linux();
  serial->SetPort(serialPort.c_str());
  serial->SetBaudRate(serialBauds);
  if(!serial->Setup()) {
    std::cerr << "Failed to setup serial port" << std::endl;
    exit(1);
  }

  TestSerialData* app;
  app = new TestSerialData(serialPort, serialBauds);

  SerialProtocol::Use(app);

  DrzEngine::UseApp(app);
  
  //engine setup autodetects engine sub parts
  DrzEngine::Setup();  

  //Start engine main loop
  DrzEngine::Start();

  //if (app->Construct(320, 240, 2, 2)) {
  //  app->Start();
  //}
  return 0;
}