#pragma once 

#include "Widget.h"

#include <vector>

using namespace drz;

class DisplayPage {
  public:
    DisplayPage(std::string name) : name(name) {}
    std::string name;
    bool isVisible = true;
    virtual void ReadInputs() = 0;
    virtual void Load() = 0;
    virtual void Update(float fElapsedTime) = 0;
    virtual void Render() = 0;
    virtual void Activate(DisplayPage* lastPage) = 0;
    void AddWidget(Widget* widget);
    std::vector<Widget*> GetWidgets();
  protected:
    std::vector<Widget*> widgets;
};

class DisplayPageManager {
  public:
    static void AddPage(DisplayPage* page);
    static void Load();
    static DisplayPage* GetPage(std::string name);
    static void GoToPage(std::string name);
    static void GoToPage(DisplayPage* page);
    static void Back();
    static void ReadInputs();
    static void Update(float fElapsedTime);
    static void Render();
  private:
    inline static DisplayPage* lastPage=nullptr;
    inline static DisplayPage* currentPage=nullptr;
    inline static std::vector<DisplayPage*> pages;

};

