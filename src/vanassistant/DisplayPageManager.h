#pragma once

#include <IDrzEngine.h>
#include "IDisplayPage.h"

#include <vector>

class DisplayPageManager {
  public:

    static void SetEngine(drz::IDrzEngine* engine) {
      DisplayPageManager::engine = engine;
    }

    static drz::IDrzEngine* GetEngine() {
      return engine;
    }

    static void AddPage(IPage* page) {
      pages.push_back(page);
    }

    static void Load() {

      for (auto page : pages) {
        page->Load();
      }

      //set default page = first page
      if(pages.size() > 0) {
        currentPage = pages[0];
      }
    }

    static void GoToPage(IPage* page) {
      //TODO: check if page is in pages
      //TODO: program full redraw
      currentPage = page;
    }

    static void ReadInputs() {
      if(currentPage != nullptr) {
        currentPage->ReadInputs();
      }
    }

    static void Update(float fElapsedTime) {
      //set isVisible to false for all pages except the current page
      for (auto page : pages) {
        page->isVisible = (page == currentPage);
        page->Update(fElapsedTime);
      }
    }

    static void Render() {
      if(currentPage != nullptr) {
        currentPage->Render();
      }
    }

  private:
    inline static drz::IDrzEngine* engine;
    inline static IPage* currentPage;
    inline static std::vector<IPage*> pages;
};