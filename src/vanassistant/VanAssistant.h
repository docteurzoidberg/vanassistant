#pragma once 

//Generic interfaces
#include <IDrzEngine.h>
#include <IDrzSam.h>
#include "IDrzSerial.h"

//Display pages
#include "PageAssistant.h"

//Serial protocol
#include "SerialProtocol.h"

class VanAssistant {
  public:
    VanAssistant(IDrzEngine* engine, IDrzSam* sam, IDrzSerial* serial) {
      this->engine = engine;
      this->sam = sam;
      this->serial = serial;

      displayPageAssistant = new PageAssistant(sam);
      //TODO: add other display pages
    }

    bool Say(std::string text) {
      return false;
    } 

    void Setup() {
      protocol = std::make_unique<SerialProtocol>(serial);
    
      DisplayPageManager::AddPage(displayPageAssistant);
      //TODO: add other display pages
    }

    void Update(float fElapsedTime) {
      DisplayPageManager::ReadInputs();
      DisplayPageManager::Update(fElapsedTime);
      //DisplayPageManager::Render();
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
    IDrzSerial* serial;

    std::unique_ptr<SerialProtocol> protocol;

    DisplayPage* displayPageMenu;
    DisplayPage* displayPageRoad; 
    DisplayPage* displayPageAssistant;
    DisplayPage* displayPageSys;
    DisplayPage* displayPageDebug;
    
};