#pragma once

#include <IDrzSam.h>
#include <miniaudio.h>

#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

#include "sam/reciter.h"
#include "sam/sam.h"

extern "C" {

  int pos = 0;
  int debug = 0;
  extern unsigned char input[256];

  void sleep_ms(int milliseconds) {
    usleep(milliseconds * 1000); // usleep takes sleep time in microseconds
  }

  void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
  {
    int bufferpos = GetBufferLength();
    char *buffer = GetBuffer();
    if (pos >= bufferpos) return;
    if ((bufferpos - pos) < frameCount) frameCount = (bufferpos - pos);
    for (ma_uint32 i = 0; i < frameCount; i++) {
      ((unsigned char*)pOutput)[i] = buffer[pos];
      pos++;
    }
    (void)pInput;
  }

  void OutputSound()
  {
    ma_result result;
    ma_device_config deviceConfig;
    ma_device device;

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = ma_format_u8;
    deviceConfig.playback.channels = 1;
    deviceConfig.sampleRate = 22050;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = NULL;

    result = ma_device_init(NULL, &deviceConfig, &device);
    if (result != MA_SUCCESS) {
      std::cerr << "Failed to initialize playback device." << std::endl;
      return;
    }

    result = ma_device_start(&device);
    if (result != MA_SUCCESS) {
      std::cerr << "Failed to start playback device." << std::endl;
      ma_device_uninit(&device);
      return;
    }

    int bufferpos = GetBufferLength();
    bufferpos /= 50;
    while (pos < bufferpos) {
      sleep_ms(100);  // Sleep in milliseconds
    }
    ma_device_uninit(&device);
  }
}


class Drz_Miniaudio_Sam : public IDrzSam
{
public:
    Drz_Miniaudio_Sam() {

    };
    
    ~Drz_Miniaudio_Sam() {

    };

    bool Say(std::string text) {
      std::cout << "Saying: " << text << std::endl;

      strcpy((char*)input, text.c_str());
      strcat((char*)input, "[");
      if (!TextToPhonemes(input)) {
        std::cout << "Failed to convert text to phonemes" << std::endl;
        return false;
      }
      SetInput(input);
      if (!SAMMain()) {
        std::cout << "Failed to generate speech" << std::endl;
        return false;
      }

      std::thread audioThread(&Drz_Miniaudio_Sam::OutputSound, this);
      audioThread.detach(); // Detach the thread to allow independent execution

      return true;
    };
  private:
    void OutputSound() {
      ::OutputSound();
    }
};

