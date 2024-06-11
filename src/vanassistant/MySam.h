#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>

#include <iostream>
#include <string.h> 
#include <stdio.h>
#include <stdlib.h>

#include "../sam/reciter.h"
#include "../sam/sam.h"
#include "../sam/debug.h"

extern "C" {

  int pos = 0;
  int debug = 0;
  unsigned char input[256];

  void MixAudio(void *unused, Uint8 *stream, int len)
  {
    int bufferpos = GetBufferLength();
    char *buffer = GetBuffer();
    int i;
    if (pos >= bufferpos) return;
    if ((bufferpos-pos) < len) len = (bufferpos-pos);
    for(i=0; i<len; i++)
    {
      stream[i] = buffer[pos];
      pos++;
    }
  }

  void OutputSound()
  {
    int bufferpos = GetBufferLength();
    bufferpos /= 50;
    SDL_AudioSpec fmt;

    fmt.freq = 22050;
    fmt.format = AUDIO_U8;
    fmt.channels = 1;
    fmt.samples = 2048;
    fmt.callback = MixAudio;
    fmt.userdata = NULL;

    /* Open the audio device and start playing sound! */
    if ( SDL_OpenAudio(&fmt, NULL) < 0 ) 
    {
      std::cout << "Unable to open audio: " << SDL_GetError() << std::endl;
      exit(1);
    }
    SDL_PauseAudio(0);
    //SDL_Delay((bufferpos)/7);
    
    while (pos < bufferpos)
    {
      SDL_Delay(100);
    }
    
    SDL_CloseAudio();
  } 

}

class MySam {
  public:
  
    MySam() {
      std::cout << "MySam constructor" << std::endl;
    } 

    ~MySam() {
      std::cout << "MySam destructor" << std::endl;
    }

    bool Init() {
      std::cout << "MySam Init" << std::endl;
      if ( SDL_Init(SDL_INIT_AUDIO) < 0 ) 
      {
        std::cout << "Unable to init SDL: " << SDL_GetError() << std::endl;
        return  false;
      }
      return true;
    }

    bool Say(std::string text) {
      std::cout << "Saying: " << text << std::endl;

      // char * to unsigned char * ?
      strcpy((char*)input, text.c_str());
      strcat((char*)input, "[");
      if (!TextToPhonemes(input)) {
        std::cout << "Failed to convert text to phonemes" << std::endl;
        return false;
      }
      SetInput(input);
      if (!SAMMain())
      {
        std::cout << "Failed to generate speech" << std::endl;
        return false;
      }

      OutputSound();
      return true;
    }
};