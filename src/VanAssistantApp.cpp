#include <DrzEngine.h>
#include <DrzGraphics.h>

#include "VanAssistantApp.h"


#include "vanassistant/DisplayPageManager.h"
#include "vanassistant/PageRoad.h"

#include <iostream>

VanAssistantApp::VanAssistantApp() {
  std::cout << "VanAssistantApp constructor called" << std::endl;
}

VanAssistantApp::~VanAssistantApp() {
  std::cout << "VanAssistantApp destructor called" << std::endl;
}

void VanAssistantApp::Setup() {
  std::cout << "VanAssistantApp::Setup called" << std::endl;
  //TODO: setup display pages

  //engine = DrzEngine::Get();
  gfx = DrzGraphics::Get();

  DisplayPage* roadPage = new PageRoad();
  DisplayPageManager::AddPage(roadPage);
}

void VanAssistantApp::Update(float elapsedTime) {
  std::cout << "VanAssistantApp::Update called" << std::endl;

  //Draw pixels using graphics
  auto gfx = DrzGraphics::Get();
  gfx->DrawPixel(0, 0, Color(255, 0, 0));
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