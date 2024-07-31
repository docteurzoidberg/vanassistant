#pragma once

#include <string>

class IDrzSam {
  public:
    virtual ~IDrzSam() {}
    virtual bool Say(std::string text) = 0;
};


class DrzSam {
  public:
    static void Set(IDrzSam* instance) {
      DrzSam::instance = instance;
    }
    static IDrzSam* Get() {
      return instance;
    }
  private:
    inline static IDrzSam* instance = nullptr;
};