#include "DisplayPageManager.h"

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
}

void DisplayPageManager::GoToPage(DisplayPage* page) {
  //TODO: check if page is in pages
  //TODO: program full redraw
  currentPage = page;
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
