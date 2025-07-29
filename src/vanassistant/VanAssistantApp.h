#pragma once  

#include <DrzGraphics.h>
#include <IDrzEngineApp.h>

#include "SerialProtocol.h"

#include "PageAssistant.h"
#include "PageMenu.h"
#include "PageRoad.h"
#include "PagePark.h"

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

  protected:
    PageAssistant *assistantPage = nullptr;
    PageMenu *menuPage = nullptr;
    PageRoad *roadPage = nullptr;
    PagePark *parkPage = nullptr;
  private:
    IDrzGraphics* gfx;
};