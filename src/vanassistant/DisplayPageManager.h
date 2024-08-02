#pragma once 

#include "Widget.h"

#include <vector>

using namespace drz;

class DisplayPage {
  public:
    bool isVisible = true;
    virtual void ReadInputs() = 0;
    virtual void Load() = 0;
    virtual void Update(float fElapsedTime) = 0;
    virtual void Render() = 0;
    void AddWidget(Widget* widget);
    std::vector<Widget*> GetWidgets();
  protected:
    std::vector<Widget*> widgets;
};

class DisplayPageManager {
  public:
    static void AddPage(DisplayPage* page);
    static void Load();
    static void GoToPage(DisplayPage* page);
    static void ReadInputs();
    static void Update(float fElapsedTime);
    static void Render();
  private:
    inline static DisplayPage* currentPage;
    inline static std::vector<DisplayPage*> pages;
};

