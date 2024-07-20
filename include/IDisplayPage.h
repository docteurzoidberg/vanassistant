#pragma once

class IPage {
  public:
    bool isVisible = true;
    virtual void ReadInputs() = 0;
    virtual void Load() = 0;
    virtual void Update(float fElapsedTime) = 0;
    virtual void Render() = 0;
};