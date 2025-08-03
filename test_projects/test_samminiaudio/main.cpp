
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "Drz_Miniaudio_Sam.h"

#include <atomic>
#include <string>

extern unsigned char SampleBuffer[];
extern unsigned int bufferpos;

drz::IDrzSam* sam = nullptr;

class SAMDemo : public olc::PixelGameEngine {
public:
    SAMDemo() {
        sAppName = "SAM + miniaudio + PGE Test";
    }

    std::atomic<bool> speaking = false;

    bool OnUserCreate() override {
        sam = new Drz_Miniaudio_Sam();
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        Clear(olc::BLACK);
        DrawString(10, 10, "Press SPACE to speak", olc::WHITE);

        static float jaw = 0.0f;
        if (speaking)
            jaw = 20.0f;
        else if (jaw > 0.0f)
            jaw -= 40.0f * fElapsedTime;

        FillRect(50, 50, 40, 40);              // head
        FillRect(60, 90, 20, (int)jaw);        // jaw

        if (GetKey(olc::Key::SPACE).bPressed && !speaking) {
            sam->Say("Hello");
        }
        return true;
    }


};

int main() {
    SAMDemo demo;
    if (demo.Construct(160, 120, 4, 4))
        demo.Start();
    return 0;
}
