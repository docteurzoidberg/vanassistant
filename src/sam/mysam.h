#pragma once

#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>

#include <iostream>

#include "reciter.h"
#include "sam.h"
#include "debug.h"

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
      return true;
    }
    void Say(const char *text) {
      std::cout << "Saying: " << text << std::endl;
    }
  private:


};
extern "C" {
int pos = 0;
int debug = 0;

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
		printf("Unable to open audio: %s\n", SDL_GetError());
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