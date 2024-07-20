#pragma once 

#include <IDrzEngine.h>
#include <IDrzSam.h>


#include "DisplayPageManager.h"

#include "IDisplayPage.h"
#include "PageAssistant.h"
#include "SerialProtocol.h"


class VanAssistant {
  public:
    VanAssistant(IDrzEngine* engine, IDrzSam* sam, Drz_Serial* serial) {
      this->engine = engine;
      this->sam = sam;
      this->serial = serial;
    }

    bool Say(std::string text) {
      return false;
    } 

    void Setup() {
      protocol = new SerialProtocol(serial);
      DisplayPageManager::SetEngine(engine);
      DisplayPageManager::AddPage(new PageAssistant(sam));
    }

    void Update(float fElapsedTime) {
      DisplayPageManager::ReadInputs();
      DisplayPageManager::Update(fElapsedTime);
      DisplayPageManager::Render();
    }

    void Render() {
      engine->Clear(BLACK);
      DisplayPageManager::Render();
    }

    bool ReadHeader(char* buffer, J7PacketHeader* header) {
      return protocol->ReadHeader(buffer, header);
    }

    bool ReadJ7DashboardPacketData(char* buffer, J7DashboardPacketData* data) {
      return protocol->ReadJ7DashboardPacketData(buffer, data);
    }

    bool ReadJ7IMUPacketData(char* buffer, J7IMUPacketData* data) {
      return protocol->ReadJ7IMUPacketData(buffer, data);
    }

    bool ReadJ7SayTextPacketData(char* buffer, J7SayTextPacketData* data) {
      return protocol->ReadJ7SayTextPacketData(buffer, data);
    }





  private:
    IDrzEngine* engine;
    IDrzSam* sam;
    Drz_Serial* serial;

    SerialProtocol* protocol;

    IPage* AssistantPage;
    IPage* DebugPage;

    
};