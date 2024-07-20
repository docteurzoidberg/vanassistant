#include "Drz_Engine_PGE.h"
#include "Drz_Sam_Miniaudio.h"
#include "Drz_Serial_Termios.h"

#include "IDrzSerial.h"
#include "fonts/Solid_Mono8pt7b.h"
#include "fonts/Solid_Mono4pt7b.h"
 
#include "vanassistant/VanAssistant.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#endif


class VanAssistantPGE : public Drz_PGE_Engine {
   
public:

  std::string serial_port = "/dev/ttyUSB0";
  int serial_baudrate = 115200;

	VanAssistantPGE(int arc, char* argv[]) : Drz_PGE_Engine(this) {

    //Get serial port and baud rate from command line arguments
    if(arc > 1) {
      serial_port = argv[1];
    }

    std::cout << "Serial port: " << serial_port << std::endl;

    if(arc > 2) {
      try {
        serial_baudrate = std::stoi(argv[2]);
      } catch(const std::exception& e) {
        std::cerr << "Invalid baud rate. Defaulting to 115200." << std::endl;
      }
    }

    std::cout << "Serial baud rate: " << serial_baudrate << std::endl;

    //Set window title
		sAppName = "VanAssistant";

    //Load fonts
    const font* solidmono8 = LoadFont("solidmono8", &Solid_Mono8pt7b);
    const font* solidmono4 = LoadFont("solidmono4", &Solid_Mono4pt7b);
    //Load sprites when any...

    //Load sounds when any...

    //Load 3d models when any...
	}

	bool OnUserCreate() override {

    //Load serial port
    serial = std::make_unique<Drz_Serial>(serial_port, serial_baudrate);
    if(!serial->Setup()) { 
      std::cerr << "Failed to setup serial port" << std::endl;
      exit(1);
    }

    //protocol = std::make_unique<SerialProtocol>(serial.get());
    
    DisplayPageManager::SetEngine(this);
    
    sam = std::make_unique<Drz_Miniaudio_Sam>();
    vanassistant = std::make_unique<VanAssistant>(this, sam.get(), serial.get());
    vanassistant->Setup();

    DisplayPageManager::Load();
    
    ConsoleCaptureStdOut(true);
		return true;
	}

  bool OnUserUpdate(float fElapsedTime) override {

    // Q: exit the app
    if(GetKey(olc::Key::Q).bPressed) {
      exit(0);
    }

    // F1: show console
    if(GetKey(olc::Key::F1).bPressed) {
      ConsoleShow(olc::Key::ESCAPE, false);
    }
    
    // B: toggle debug mode
    //if(GetKey(drz::Key::B).bPressed) {
    //  ToggleDebug();
    //}

    // F: toggle fps
    //if(GetKey(drz::Key::F).bPressed) {
    //  ToggleFps();
   // }


    ReadSerial();
   
    //vanassistant->ReadInputs(fElapsedTime);
    vanassistant->Update(fElapsedTime);
    vanassistant->Render();
    return true;
  }

  bool OnConsoleCommand(const std::string& text) override {
    vanassistant->Say(text);
    return true;
  }

  bool Say(std::string text) {
    return vanassistant->Say(text);
  }

private:
  std::unique_ptr<VanAssistant> vanassistant; 
  std::unique_ptr<IDrzSam> sam;
  std::unique_ptr<IDrzSerial> serial;

  bool ReadSerial() {

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

    if(!vanassistant->ReadHeader(serial->GetReadBuffer(), &header)) {
      std::cerr << "Failed to read header" << std::endl;
      return false;
    }

    switch (header.type) {
      case J7_SAY_TEXT: {
        J7SayTextPacketData data;
        if(vanassistant->ReadJ7SayTextPacketData(serial->GetReadBuffer(), &data)) {
          ProcessSayTextPacket(&data);
        } else {
          std::cerr << "Failed to read J7SayTextPacketData" << std::endl;
        }
        break;
      }
      case J7_DASHBOARD: { 
        J7DashboardPacketData data;
        if(vanassistant->ReadJ7DashboardPacketData(serial->GetReadBuffer(), &data)) {
          ProcessDashboardPacket(&data);
        } else {
          std::cerr << "Failed to read J7DashboardPacketData" << std::endl;
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

  void ProcessSayTextPacket(J7SayTextPacketData* data) {
    std::string text(data->text);
    std::cout << "Say text packet received: " << text << std::endl;
    vanassistant->Say(text);
  }

  void ProcessDashboardPacket(J7DashboardPacketData* data) {
    //nothing for now except print the data
    std::cout << "Dashboard packet received:" << std::endl;
    std::cout << "--------------------------" << std::endl;
    std::cout << "Fuel gauge: " << static_cast<int>(data->fuelgauge) << "%" << std::endl;
    std::cout << "Speed: " << static_cast<int>(data->speed) << " km/h" << std::endl;
    std::cout << "Battery: " << static_cast<int>(data->battery) / 10.0 << " V" << std::endl;
    std::cout << "RPM: " << static_cast<int>(data->rpm) * 100 << std::endl;
    std::cout << "Coolant Temp: " << data->coolant_temp / 10.0 << " °C" << std::endl;
    std::cout << "Odometer: " << data->odometer << " km" << std::endl;
    std::cout << "Trip: " << data->trip / 10.0 << " km" << std::endl;
    std::cout << "Lamps:" << std::endl;
    std::cout << "  Preheat: " << data->lamp_preheat << std::endl;
    std::cout << "  Not charging: " << data->lamp_notcharging << std::endl;
    std::cout << "  Oil: " << data->lamp_oil << std::endl;
    std::cout << "  Turn signals: " << data->lamp_turnsignals << std::endl;
    std::cout << "  High beam: " << data->lamp_highbeam << std::endl;
    std::cout << "  Low beam: " << data->lamp_lowbeam << std::endl;
    std::cout << "  Warnings: " << data->lamp_warnings << std::endl;
    std::cout << "  Problem: " << data->lamp_problem << std::endl;
  }
};

std::unique_ptr<VanAssistantPGE> app;

int main(int argc, char* argv[]) {
  app = std::make_unique<VanAssistantPGE>(argc, argv);
  if (app->Construct(SCREEN_W, SCREEN_H, 2, 2, false, true))
    app->Start();
  return 0;
}

//emscripten exposes the Say function to the javascript
bool Say(std::string text) {
  return app->Say(text);
}

//if emscripten is defined, we need to bind the functions to the javascript
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_BINDINGS(my_module) {
  emscripten::function("Say", &Say);
}
#endif