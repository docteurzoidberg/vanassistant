#include "../../../src/Drz_PGE_Engine.h"
#include "../../../src/Drz_Serial.h"

#include "../../../src/fonts/Solid_Mono8pt7b.h"

#include "../../../src/vanassistant/SerialProtocol.h"
#include "IDrzEngine.h"
#include <cstdint>
#include <iostream>
#include <string>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

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

class TestSerialData : public Drz_PGE_Engine {

  public:
    std::string serial_port="/dev/ttyUSB0";
    std::string baud_rate="115200";
    olc::QuickGUI::Manager guiManager;

    TestSerialData(int argc, char* argv[]) : Drz_PGE_Engine(this) {

      //check if serial port is provided
      if(argc > 1) {
        serial_port = argv[1];
      }

      //check if baud rate is provided
      if(argc > 2) {
        baud_rate = argv[2];
      } 

      //check if baud rate is valid int value
      try {
        bauds = std::stoi(baud_rate);
      } catch (std::invalid_argument& e) {
        std::cerr << "Cannot parse baud rate" << std::endl;
        exit(1);
      }

      std::cout << "Serial port: " << serial_port << std::endl;
      std::cout << "Baud rate: " << bauds << std::endl;
      
      sAppName = serial_port + "@" + baud_rate;

      //Load font
      LoadFont("solidmono8", &Solid_Mono8pt7b);
      SetFont("solidmono8");
    }

    ~TestSerialData() {
      serial->Close();
    }

    bool OnUserCreate() override {

      //initialize serial
      serial = new Drz_Serial(serial_port, bauds);
      if(!serial->Setup()) {
        std::cerr << "Failed to setup serial port" << std::endl;
        exit(1);
      }

      //initialize serial protocol
      serialProtocol = new SerialProtocol(serial); 

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
		
      ConsoleCaptureStdOut(true);
      return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
      
      // F1: show console
      if(GetKey(olc::Key::F1).bPressed) {
        ConsoleShow(olc::Key::ESCAPE, false);
      }

      // F: toggle show fps
      if(GetKey(olc::Key::F).bPressed) {
        drawFPS = !drawFPS;
      }

      //M: toggle mask
      if(GetKey(olc::Key::M).bPressed) {
        drawMask = !drawMask;
      }

      if(guiSliderFuel->bHeld) {
        data.fuelgauge = guiSliderFuel->fValue;
        needToSendData = true;
      }

      if(guiSliderSpeed->bHeld) {
        data.speed = guiSliderSpeed->fValue;
        needToSendData = true;
      }

      if(guiSliderBat->bHeld) {
        data.battery = guiSliderBat->fValue;
        needToSendData = true;
      }

      if(guiSliderRPM->bHeld) {
        data.rpm = guiSliderRPM->fValue;
        needToSendData = true;
      }

      if(guiSliderCoolantTemp->bPressed) {
        data.coolant_temp = guiSliderCoolantTemp->fValue;
        needToSendData = true;
      }

      if(guiTextBoxOdometer->bPressed) {
        data.odometer = std::stoi(guiTextBoxOdometer->sText);
        counterTotalDistance->SetCounter(data.odometer);
        needToSendData = true;
      }

      if(guiTextBoxTrip->bPressed) {
        data.trip = (uint32_t) (std::stof(guiTextBoxTrip->sText)*10);
        counterDistance->SetCounter(data.trip);
        needToSendData = true;
      }

      if(guiCheckBoxPreheat->bPressed) {
        data.lamp_preheat = guiCheckBoxPreheat->bChecked;
        needToSendData = true;
      }

      if(guiCheckBoxNotCharging->bPressed) {
        data.lamp_notcharging = guiCheckBoxNotCharging->bChecked;
        needToSendData = true;
      }

      if(guiCheckBoxOil->bPressed) {
        data.lamp_oil = guiCheckBoxOil->bChecked;
        needToSendData = true;
      }

      if(guiCheckBoxTurnSignals->bPressed) {
        data.lamp_turnsignals = guiCheckBoxTurnSignals->bChecked;
        needToSendData = true;
      }

      if(guiCheckBoxHighBeam->bPressed) {
        data.lamp_highbeam = guiCheckBoxHighBeam->bChecked;
        needToSendData = true;
      }

      if(guiCheckBoxLowBeam->bPressed) {
        data.lamp_lowbeam = guiCheckBoxLowBeam->bChecked;
        needToSendData = true;
      }

      if(guiCheckBoxWarnings->bPressed) {
        data.lamp_warnings = guiCheckBoxWarnings->bChecked;
        needToSendData = true;
      }

      if(guiCheckBoxProblem->bPressed) {
        data.lamp_problem = guiCheckBoxProblem->bChecked;
        needToSendData = true;
      }

      bool bDataUpdated = false;
      if(needToSendData) {
        SendData();
        bDataUpdated = true;
        needToSendData = false;
      }

      if(ReadData() || bDataUpdated) {
        //update gui with new data
        DataToGui();
      }

      // We must update the manager at some point each frame. Values of controls
		  // are only valid AFTER this call to update()
		  guiManager.Update(this);

      //update blinkers
      if(data.lamp_warnings) {
        fLampWarningsBlinkEvery -= fElapsedTime;
        if(fLampWarningsBlinkEvery < 0) {
          fLampWarningsBlinkEvery = 0.3f;
          bLampWarningsBlink = !bLampWarningsBlink;
        }
      }
      if(data.lamp_turnsignals) {
        fLampTurnSignalBlinkEvery -= fElapsedTime;
        if(fLampTurnSignalBlinkEvery < 0) {
          fLampTurnSignalBlinkEvery = 0.3f;
          bTurnSignalBlink = !bTurnSignalBlink;
        }
      }

      //black background
      Clear(BLACK);
     
      //Draw GUI//draw gui elements
      guiManager.DrawDecal(this);
      counterTotalDistance->Render(185,164);
      counterDistance->Render(193, 192);

      //Draw lamp indicators
      DrawLamps();

      //Draw dashboard mask
      SetPixelMode(olc::Pixel::MASK);
      if(drawMask)
        DrawSprite(30, GetScreenHeight()-110, sprInstrumentMask);
      SetPixelMode(olc::Pixel::NORMAL);

      //Draw FPS
      if(drawFPS) {
        //text size:
        auto text = std::to_string(GetFPS());
        auto size = GetTextBounds(text, 0, 0);
        DrawText(text, GetScreenWidth()-size.w-4, 14, YELLOW);
      }
      return true;
    }

    bool OnConsoleCommand(const std::string& text) override {
      //send text over serial (raw)
      serial->Write(text.c_str(), text.length());
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
    Drz_Serial* serial;
    SerialProtocol* serialProtocol; 

    Counter* counterTotalDistance;
    Counter* counterDistance;
    J7DashboardPacketData data;
    bool needToSendData = false;
    bool drawFPS = false;
    bool drawMask = true;
    int bauds=-1;

    float fLampWarningsBlinkEvery = 0.3f;
    float fLampTurnSignalBlinkEvery = 0.3f;

    bool bTurnSignalBlink = false;
    bool bLampWarningsBlink = false;

    color lampColorOrangeOn = color(230, 148, 25);
    color lampColorOrangeOff = color(80, 50, 0);

    color lampColorRedOn = color(230, 0, 0);
    color lampColorRedOff = color(80, 0, 0);

    color lampColorGreenOn = color(0, 230, 0);
    color lampColorGreenOff = color(0, 80, 0);

    color lampColorBlueOn = color(0, 0, 230);
    color lampColorBlueOff = color(0, 0, 80);

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

      counterTotalDistance = new Counter(this, 6, sprCounterSpritesheet, {8,10});
      counterTotalDistance->SetCounter(data.odometer);
      
      counterDistance = new Counter(this, 4, sprCounterSpritesheet, {8,10});
      counterDistance->SetCounter(data.trip);

      float lx = 4.0f;
      float ly = 2.0f;

      auto odometerText = std::to_string(data.odometer);
      auto tripText = std::to_string(data.trip/10.0f);

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
      guiLabelOdemeterValue = new olc::QuickGUI::Label(guiManager, "12345", { lx+74, ly+90 }, { 70.0f, 16.0f });

      guiLabelTrip = new olc::QuickGUI::Label(guiManager, "Trip:", { lx, ly+108 }, { 70.0f, 16.0f });
      guiLabelTripValue = new olc::QuickGUI::Label(guiManager, "0423.8", { lx+74, ly+108 }, { 70.0f, 16.0f });

      //Sliders

      guiSliderFuel = new olc::QuickGUI::Slider(guiManager,{ lx+180, ly+8 }, { lx+240, ly+8 }, 0, 255, 128);
      guiSliderSpeed = new olc::QuickGUI::Slider(guiManager,{ lx+180, ly+26 }, { lx+240, ly+26 }, 0, 140, 88);
      guiSliderBat = new olc::QuickGUI::Slider(guiManager, { lx+180, ly+44}, { lx+240, ly+44}, 0, 200, 123);
      guiSliderRPM = new olc::QuickGUI::Slider(guiManager, { lx+180, ly+62 }, { lx+240, ly+62 }, 0, 100, 30);
      guiSliderCoolantTemp = new olc::QuickGUI::Slider(guiManager, { lx+180, ly+80 }, { lx+240, ly+80}, 0, 1400, 55);

      //TextBoxes

      guiTextBoxOdometer = new olc::QuickGUI::TextBox(guiManager, "123456", { lx+171, ly+90 }, { 80.0f, 16.0f } );
      guiTextBoxTrip = new olc::QuickGUI::TextBox(guiManager, "0423.8", { lx+171, ly+108 }, { 80.0f, 16.0f } );

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

      guiTextBoxOdometer->sText = std::to_string(data.odometer);
      guiTextBoxTrip->sText = strRoundUpToNDecimals(data.trip/10.0f, 1);

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
   
    bool ReadData() {

      //read serial
      auto bc = serial->Read();
      if(bc <= 0) return false; 

      std::string str(serial->read_buf, bc);
      std::cout << "<" << str << std::endl;

      //TODO: parse data using SerialProtocol
      J7DashboardPacketData* data = serialProtocol->ReceiveData(serial->read_buf, bc);
      if(data != nullptr) {
        std::cout << "Received data" << std::endl;
        std::cout << "--------------------" << std::endl;
        std::cout << "Fuel gauge: " << data->fuelgauge << std::endl;
        std::cout << "Battery: " << data->battery << std::endl;
        std::cout << "Speed: " << data->speed << std::endl;
        std::cout << "RPM: " << data->rpm << std::endl;
        std::cout << "Coolant temp: " << data->coolant_temp << std::endl;
        std::cout << "Odometer: " << data->odometer << std::endl;
        std::cout << "Lamp preheat: " << data->lamp_preheat << std::endl;
        std::cout << "Lamp not charging: " << data->lamp_notcharging << std::endl;
        std::cout << "Lamp oil: " << data->lamp_oil << std::endl;
        std::cout << "Lamp turn signals: " << data->lamp_turnsignals << std::endl;
        std::cout << "Lamp high beam: " << data->lamp_highbeam << std::endl;
        std::cout << "Lamp low beam: " << data->lamp_lowbeam << std::endl;
        std::cout << "Lamp warnings: " << data->lamp_warnings << std::endl;
      }
      return true;
    }

    void SendData() { 
      serialProtocol->SendData(&data);
      needToSendData = false;

      std::cout << "Sent data" << std::endl;
      std::cout << "--------------------" << std::endl;
      std::cout << "Fuel gauge: " << std::to_string(data.fuelgauge) << std::endl;
      std::cout << "Battery: " << std::to_string(data.battery) << std::endl;
      std::cout << "Speed: " << std::to_string(data.speed) << std::endl;
      std::cout << "RPM: " << std::to_string(data.rpm) << std::endl;
      std::cout << "Coolant temp: " << std::to_string(data.coolant_temp) << std::endl;
      std::cout << "Odometer: " << std::to_string(data.odometer) << std::endl;
      std::cout << "Lamp preheat: " << data.lamp_preheat << std::endl;
      std::cout << "Lamp not charging: " << data.lamp_notcharging << std::endl;
      std::cout << "Lamp oil: " << data.lamp_oil << std::endl;
      std::cout << "Lamp turn signals: " << data.lamp_turnsignals << std::endl;
      std::cout << "Lamp high beam: " << data.lamp_highbeam << std::endl;
      std::cout << "Lamp low beam: " << data.lamp_lowbeam << std::endl;
      std::cout << "Lamp warnings: " << data.lamp_warnings << std::endl;
    } 
    
    void DrawLamps() {
      //pre heating
      if(data.lamp_preheat) {
        FillRect(250, 130, 24, 24, lampColorOrangeOn);
      } else {
        FillRect(250, 130, 24, 24, lampColorOrangeOff);
      }

      //alarm
      if(data.lamp_problem) {
        FillRect(150, 130, 24, 24, lampColorRedOn);
      } else {
        FillRect(150, 130, 24, 24, lampColorRedOff);
      }

      //warnings
      if(data.lamp_warnings && bLampWarningsBlink) {
        FillRect(250, 200, 40, 40, lampColorRedOn);
      } else {
        FillRect(250, 200, 40, 40, lampColorRedOff);
      }
 
      //turn signals
      if(data.lamp_turnsignals && bTurnSignalBlink) {
        FillRect(98, 200, 28, 12, lampColorGreenOn);
      } else {
        FillRect(98, 200, 28, 12, lampColorGreenOff);
      } 

      //not charging
      if(data.lamp_notcharging) {
        FillRect(65, 186, 20, 20, lampColorRedOn);
      } else {
        FillRect(65, 186, 20, 20, lampColorRedOff);
      }

      //oil
      if(data.lamp_oil) {
        FillRect(135, 186, 20, 20, lampColorRedOn);
      } else {
        FillRect(135, 186, 20, 20, lampColorRedOff);
      }

      //high beam
      if(data.lamp_highbeam) {
        FillRect(80, 204, 18, 20, lampColorBlueOn);
      } else {
        FillRect(80, 204, 18, 20, lampColorBlueOff);
      }

      //low beam
      if(data.lamp_lowbeam) {
        FillRect(122, 204, 18, 20, lampColorGreenOn);
      } else {
        FillRect(122, 204, 18, 20, lampColorGreenOff);
      }
    }
};

int main(int argc, char* argv[]) {
  TestSerialData* app;
  app = new TestSerialData(argc, argv);
  if (app->Construct(320, 240, 2, 2)) {
    app->Start();
  }
  return 0;
}