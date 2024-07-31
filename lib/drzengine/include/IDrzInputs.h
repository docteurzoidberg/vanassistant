#pragma once

#include <IDrzEngine.h>

namespace drz {

  class IDrzInputs {
    public:
      //virtual ~IDrzInputs() = 0;
      virtual bool Setup() = 0; 
      virtual void Update() = 0;
      virtual bool Close() = 0;
      virtual hwbutton GetKey(Key k) = 0;
  };

  class DrzInputs {
    public:
      static void Set(IDrzInputs* inputs) {
        DrzInputs::inputs = inputs;
      }

      static IDrzInputs* Get() {
        return DrzInputs::inputs;
      }
    protected:
      inline static IDrzInputs* inputs = nullptr;
  };
}

