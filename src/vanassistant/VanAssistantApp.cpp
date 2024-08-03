#include <DrzEngine.h>
#include <DrzGraphics.h>
#include <DrzSerial.h>

#include "VanAssistantApp.h"

#include "SerialProtocol.h"
#include "fonts/Solid_Mono8pt7b.h"
#include "fonts/Solid_Mono4pt7b.h"

#include "DisplayPageManager.h"
#include "PageRoad.h"

#include <iostream>

VanAssistantApp::VanAssistantApp() {
  std::cout << "VanAssistantApp constructor called" << std::endl;
  gfx = DrzGraphics::Get();
  if(gfx==nullptr) {
    std::cout << "DrzGraphics is null" << std::endl;
    exit(1);
  }
}

VanAssistantApp::~VanAssistantApp() {
  std::cout << "VanAssistantApp destructor called" << std::endl;

}

void VanAssistantApp::Setup() {

  std::cout << "VanAssistantApp::Setup called" << std::endl;

  //Load fonts
  gfx->LoadFont("solidmono8", const_cast<font*>(&Solid_Mono8pt7b));
  gfx->LoadFont("solidmono4", const_cast<font*>(&Solid_Mono4pt7b));
  //gfx->LoadFont("...", const_cast<font*>(&...));

  //Create display pages
  DisplayPage* roadPage = new PageRoad();
  //DisplayPage* ... = new ...();

  //Add all display pages
  DisplayPageManager::AddPage(roadPage);
  //DisplayPageManager::AddPage(...);
  //DisplayPageManager::AddPage(...);

  //Initialize all display pages and set default page to first one
  DisplayPageManager::Load();

  //Set the serial protocol receiver callbacks
  SerialProtocol::Use(this);
}

void VanAssistantApp::Update(float elapsedTime) {
  //std::cout << "VanAssistantApp::Update called" << std::endl;
  SerialProtocol::ReadSerial();
  DisplayPageManager::ReadInputs();
  DisplayPageManager::Update(elapsedTime);
  DisplayPageManager::Render();
}


bool VanAssistantApp::Command(const std::string& command) {
  std::cout << "VanAssistantApp::Command called" << std::endl;
  std::cout << "Command: " << command << std::endl;
  return false;
}

bool VanAssistantApp::Data(const uint8_t* data, size_t size) {
  std::cout << "VanAssistantApp::Data called" << std::endl;
  std::cout << "Data size: " << size << std::endl;
  return false;
}

//Data processing functions

void VanAssistantApp::ProcessJ7IMUPacketData(J7IMUPacketData* data) {
  std::cout << "IMU packet received" << std::endl;
  //TODO
}

void VanAssistantApp::ProcessJ7SayTextPacketData(J7SayTextPacketData* data) {
  std::string text(data->text);
  std::cout << "Say text packet received: " << text << std::endl;
  //TODO
}

void VanAssistantApp::ProcessJ7DashboardPacketData(J7DashboardPacketData* data) {
  std::cout << "Dashboard packet received" << std::endl;
  //TODO
}

