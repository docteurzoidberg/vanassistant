#pragma once

#include <IDrzEngine.h>

namespace drz {

  class IDrzInputs {
    public:
      virtual ~IDrzInputs() = 0;
      virtual bool Setup() = 0; 
      virtual void Update() = 0;
      virtual bool Close() = 0;
      virtual hwbutton GetKey(Key k) = 0;
  };
}