#pragma once  

#include <DrzGraphics.h>
#include <IDrzEngineApp.h>

#include "SerialProtocol.h"

using namespace drz;

class VanAssistantApp : public IDrzEngineApp, public ISerialProtocolReceiver {
  public:
    VanAssistantApp();
    ~VanAssistantApp(); 

    //IDrzEngineApp
    void Setup() override;
    void Update(float elapsedTime) override;
    bool Command(const std::string& command) override;
    bool Data(const uint8_t* data, size_t size) override; 

    //ISerialProtocolReceiver    
    void ProcessJ7IMUPacketData(J7IMUPacketData* data) override;
    void ProcessJ7SayTextPacketData(J7SayTextPacketData* data) override;
    void ProcessJ7DashboardPacketData(J7DashboardPacketData* data) override;
    void ProcessVictronSmartShuntData(VictronSmartShuntData* data) override;
    void ProcessVictronOrionData(VictronOrionData* data) override;

  private:
    IDrzGraphics* gfx;
};