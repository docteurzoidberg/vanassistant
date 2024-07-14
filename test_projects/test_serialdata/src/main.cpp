#include "../../../src/Drz_PGE_Engine.h"
#include "../../../src/Drz_Serial.h"

#include "../../../src/fonts/Solid_Mono8pt7b.h"

#include "../../../src/vanassistant/SerialProtocol.h"
#include <iostream>
#include <string>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class TestSerialData : public Drz_PGE_Engine {
  public:
    std::string serial_port="/dev/ttyUSB0";
    std::string baud_rate="115200";
    int bauds=-1;

    TestSerialData(int argc, char* argv[]) : Drz_PGE_Engine(this) {

      if(argc > 1) {
        serial_port = argv[1];
      }

      if(argc > 2) {
        baud_rate = argv[2];
      } 

      std::cout << "Serial port: " << serial_port << std::endl;
      
      //check if baud rate is valid int value

      try {
        bauds = std::stoi(baud_rate);
        std::cout << "Baud rate: " << bauds << std::endl;
      } catch (std::invalid_argument& e) {
        std::cerr << "Cannot parse baud rate" << std::endl;
        exit(1);
      }
      
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
      data.fuelgauge = 100;     // 100/255=39%
      data.battery = 134;       // /10 = 134/10=13.4V
      data.speed = 88;          // *1   = 88 km/h
      data.rpm = 30;            // *100 = 3000 RPM
      data.coolant_temp = 90;   // *1 = 90°C
      data.odometer = 12345;    // *1 = 12345 km
      data.lamp_preheat = false;
      data.lamp_notcharging = false;
      data.lamp_oil = false;
      data.lamp_turnsignals = false;
      data.lamp_highbeam = false;
      data.lamp_lowbeam = false;
      data.lamp_warnings = false;

      ConsoleCaptureStdOut(true);
      return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
      
      // F1: show console
      if(GetKey(olc::Key::F1).bPressed) {
        ConsoleShow(olc::Key::ESCAPE, false);
      }

      ReadData();

      if(needToSendData) {
        SendData();
      }

      //startfield->Update(fElapsedTime);
      Clear(BLACK);
      //startfield->Render();
      DrawText(std::to_string(GetFPS()), 4, 14, YELLOW);
      return true;
    }

    bool OnConsoleCommand(const std::string& text) override {
      //send text over serial (raw)
      serial->Write(text.c_str(), text.length());
      return true;
    }

  private:
    Drz_Serial* serial;
    SerialProtocol* serialProtocol;
    J7DashboardPacketData data;
    bool needToSendData = false;

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
      std::cout << "Fuel gauge: " << data.fuelgauge << std::endl;
      std::cout << "Battery: " << data.battery << std::endl;
      std::cout << "Speed: " << data.speed << std::endl;
      std::cout << "RPM: " << data.rpm << std::endl;
      std::cout << "Coolant temp: " << data.coolant_temp << std::endl;
      std::cout << "Odometer: " << data.odometer << std::endl;
      std::cout << "Lamp preheat: " << data.lamp_preheat << std::endl;
      std::cout << "Lamp not charging: " << data.lamp_notcharging << std::endl;
      std::cout << "Lamp oil: " << data.lamp_oil << std::endl;
      std::cout << "Lamp turn signals: " << data.lamp_turnsignals << std::endl;
      std::cout << "Lamp high beam: " << data.lamp_highbeam << std::endl;
      std::cout << "Lamp low beam: " << data.lamp_lowbeam << std::endl;
      std::cout << "Lamp warnings: " << data.lamp_warnings << std::endl;
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