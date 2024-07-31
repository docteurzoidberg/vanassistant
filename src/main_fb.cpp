#include <Drz_Engine_FB.h>
#include <Drz_Sam_Miniaudio.h>
#include <Drz_Serial_Termios.h>

#include "fonts/Solid_Mono8pt7b.h"
#include "fonts/Solid_Mono4pt7b.h"

#include "vanassistant/VanAssistant.h"

class VanAssistantFB : public Drz_FB_Engine {

public:
	VanAssistantFB(Drz_Serial_Linux* serial) : serial(serial), Drz_FB_Engine() {
    std::cout << "VanAssistantFB constructor" << std::endl;
  }

  bool Setup() {
    
    std::cout << "VanAssistantFB Setup" << std::endl;

    //Load fonts
    const font* solidmono8 = LoadFont("solidmono8", &Solid_Mono8pt7b);
    const font* solidmono4 = LoadFont("solidmono4", &Solid_Mono4pt7b);
    //Load sprites when any...
    //Load sounds when any...
    sam = std::make_unique<Drz_Miniaudio_Sam>();
    vanassistant = std::make_unique<VanAssistant>(this, sam.get(), serial.get());
    vanassistant->Setup();
    return true;
  }
    
  bool Loop(float fElapsedTime) {
    ReadInputs(fElapsedTime);
    ReadSerial();
    vanassistant->Update(fElapsedTime);
    vanassistant->Render();
    return true;
  }

  void Say(std::string text) {
    vanassistant->Say(text);
  }

  VanAssistant* GetVanAssistant() {
    return vanassistant.get();
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

  void ReadInputs(float fElapsedTime) {
    //TODO
  }
    
  void ProcessSayTextPacket(J7SayTextPacketData* data) {
    std::cout << "Received J7SayTextPacketData: " << data->text << std::endl;
    //Make the assistant say the text
    Say(data->text);
  }

  void ProcessDashboardPacket(J7DashboardPacketData* data) {
    std::cout << "Received J7DashboardPacketData:" << std::endl;
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

bool exec = true;
VanAssistantFB* app;
Drz_Serial_Linux* serialInput;

int serial_baudrate = 115200;
std::string serial_port = "/dev/ttyAMA0";
std::string fbdev = "/dev/fb0";

// Alias for convenience
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;


int main(int argc, char* argv[]) {
 
  //sigint handler
  signal(SIGINT, [](int signum) {
    exec = false;
  });

  if(argc > 1) {
    serial_port = argv[1];
  }

  if(argc > 2) {
    try {
      serial_baudrate = std::stoi(argv[2]);
    } catch (const std::invalid_argument& ia) {
      std::cerr << "Invalid baud rate. Defaulting to 115200." << std::endl; 
      exit(1);
    }
  }

  if(argc > 3) {
    fbdev = argv[3];
  }

  // open "/dev/fb0" by default, use fbg_fbdevSetup("/dev/fb1", 0) if you want to use another framebuffer
  // note : fbg_fbdevInit is the linux framebuffer backend, you can use a different backend easily by including the proper header and compiling with the appropriate backend file found in ../custom_backend/backend_name
  fbg_fbdevSetup((char *)fbdev.c_str(), 0);

  struct _fbg *fbg = fbg_fbdevInit();
  if (fbg == NULL) {
      return 0;
  }

  serialInput = new Drz_Serial_Linux(serial_port, serial_baudrate);
  bool serialOpened = serialInput->Setup();
  if(!serialOpened) {
    std::cerr << "Error opening serial port" << std::endl;
    return 1;
  }

  app = new VanAssistantFB(serialInput);
  app->SetFBG(fbg);
  app->Setup();

  double fTotalTime = 0.0f; // since start, in seconds !
  TimePoint previousFrameTime = Clock::now();
  TimePoint currentFrameTime;

  do {
    currentFrameTime = Clock::now();

    // Calculate elapsed time in microseconds
    auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentFrameTime - previousFrameTime);
    float fElapsedTime = elapsedTime.count() / 1000000.0f;
    //std::cout << "fElapsedTime: " << fElapsedTime << std::endl;
    fTotalTime+=fElapsedTime;
  
    //app loop
    app->Loop(fElapsedTime);

    //fb swap
    fbg_draw(fbg);
    fbg_flip(fbg);

    // Update previous frame time
    previousFrameTime = currentFrameTime;
  } while(exec);

  fbg_close(fbg);
  serialInput->Close();
  return 0;
}
