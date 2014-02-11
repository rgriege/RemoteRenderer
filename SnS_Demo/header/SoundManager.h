#ifndef __SoundManager_h__
#define __SoundManager_h__

#include "SDL.h"
#include "SDL_mixer.h"
#include <string>

#define NUM_SOUNDS 4

class SoundManager
{
private:
  SoundManager(void);
  SoundManager(SoundManager const&);
  SoundManager& operator=(SoundManager const&);

public:
  static SoundManager* GetInstance(void);
  void PlayBgm(const char *file);
  void PlaySoundOnce(const char *file);
  void PlaySoundLoop(const char *file);
  void PlaySound(const char *file, int loops);
  void PauseSound(const char *file);
  bool SoundPlaying(const char *file);
  int SoundLoaded(const char *file);
  void SetMusicVolume(int newVolume);
  void SetSoundVolume(int newVolume);
  float GetMusicVolume();
  float GetSoundVolume();

private:
  static SoundManager* instance;
  struct sample {
    Mix_Chunk* chunk;
    bool playing;
    bool paused;
    std::string file;
  } sounds[NUM_SOUNDS];
  int mvolume;
  int svolume;
  SDL_AudioSpec spec;
  int numChannels;
};

#endif // SoundManager.h
