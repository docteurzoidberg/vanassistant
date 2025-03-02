#include "DisplayPageManager.h"
#include <iostream>
#include <string>

using namespace drz;

void DisplayPage::AddWidget(Widget* widget) {
  widgets.push_back(widget);
}

std::vector<Widget*> DisplayPage::GetWidgets() {
  return widgets;
}

void DisplayPageManager::AddPage(DisplayPage* page) {
  pages.push_back(page);
}

void DisplayPageManager::Load() {

  for (auto page : pages) {
    page->Load();
    for(auto widget : page->GetWidgets()) {
      widget->Load();
    }
  }

  //set default page = first page
  if(pages.size() > 0) {
    currentPage = pages[0];
  }
  lastPage = nullptr;
}

DisplayPage* DisplayPageManager::GetPage(std::string name) {
  for(auto p : pages) {
    if(p->name == name) {
      return p;
    }
  }
  return nullptr;
}

void DisplayPageManager::GoToPage(std::string name) {

  DisplayPage* pageToGo = GetPage(name);
  if(pageToGo != nullptr) {
    GoToPage(pageToGo);
    return;
  }
  std::cout << "Page not found: " << name << std::endl;
}

void DisplayPageManager::GoToPage(DisplayPage* page) {
  //TODO: check if page is in pages
  //TODO: program full redraw

  lastPage = currentPage;
  if(currentPage != nullptr) {
    currentPage->isVisible = false;
  }
  currentPage = page;
  currentPage->isVisible = true;
  currentPage->Activate(lastPage);
}

void DisplayPageManager::Back() {
  if(lastPage != nullptr) {
    GoToPage(lastPage);
  }
}

void DisplayPageManager::ReadInputs() {
  if(currentPage == nullptr) {
    return;  
  }
  currentPage->ReadInputs();
}

void DisplayPageManager::Update(float fElapsedTime) {
  if(currentPage == nullptr) {
    return;
  }
  for(auto widget : currentPage->GetWidgets()) {
    widget->Update(fElapsedTime);
  }
  currentPage->Update(fElapsedTime);
}

void DisplayPageManager::Render() {
  if(currentPage == nullptr) {
    return;
  }
  for(auto widget : currentPage->GetWidgets()) {
    if(widget->isVisible) {
      widget->Render();
    }
  }
  currentPage->Render();
}
