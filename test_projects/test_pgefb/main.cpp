#include "FramebufferPlatform.h"

#undef OLC_PLATFORM_CUSTOM_EX

#define OLC_PLATFORM_CUSTOM_EX LinuxFBGPlatform
#define OLC_RENDERER_CUSTOM_EX LinuxFBGRenderer

#define OLC_PGE_APPLICATION

#include "olcPixelGameEngine.h"

class Example : public olc::PixelGameEngine {
public:
	Example() {
		sAppName = "Linux Framebuffer with FBG Example";
	}

	bool OnUserCreate() override {
		// Initialize resources here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {
		// Update and draw code
		Clear(olc::CYAN);
		Draw(1, 1, olc::RED);
		Draw(10, 10, olc::WHITE);
		return true;
	}
};

int main() {
	Example demo;
  if (demo.Construct(320,240, 1, 1)) {
    demo.Start();
  }
	return 0;
}
