#include "SoundManager.h"

SoundManager* SoundManager::instance = NULL;

Mix_Music *bgm = NULL;

SoundManager::SoundManager(void)
{
  if(SDL_Init( SDL_INIT_EVERYTHING ) == -1)
    printf("SDL init failed\n");
  if(Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1)
    printf("Mix Audio failed\n");

  mvolume = MIX_MAX_VOLUME;
  svolume = MIX_MAX_VOLUME;
  Mix_VolumeMusic(mvolume);
  numChannels = Mix_AllocateChannels(NUM_SOUNDS);
}

SoundManager* SoundManager::GetInstance()
{
  if (!instance)
    instance = new SoundManager();
  return instance;
}

void SoundManager::SetMusicVolume(int newVolume)
{
  if (newVolume > MIX_MAX_VOLUME)
    mvolume = MIX_MAX_VOLUME;
  else if (newVolume < 0)
    mvolume = 0;
  else
    mvolume = newVolume;
  Mix_VolumeMusic(mvolume);
  printf("music volume set to %i on input %i\n", Mix_VolumeMusic(-1), newVolume);
}

void SoundManager::SetSoundVolume(int newVolume)
{
  if (newVolume > MIX_MAX_VOLUME)
    svolume = MIX_MAX_VOLUME;
  else if (newVolume < 0)
    svolume = 0;
  else
    svolume = newVolume;
}

float SoundManager::GetMusicVolume()
{
  return mvolume;
}

float SoundManager::GetSoundVolume()
{
  return svolume;
}

void SoundManager::PlayBgm(const char *file)
{
  bgm = Mix_LoadMUS(file);
  if(Mix_PlayMusic(bgm, -1) == -1)
    printf("SDL cannot play music file\n");
}

// assumes sound is not currently playing
void SoundManager::PlaySound(const char *file, int loops)
{
  int channel = SoundLoaded(file);
  if (channel != -1) {
    Mix_VolumeChunk(sounds[channel].chunk, svolume);
    if (sounds[channel].paused) {
      Mix_Resume(channel);
      sounds[channel].playing = true;
      sounds[channel].paused = false;
      printf("%s resumed\n", file);
    } else {
      if (Mix_PlayChannel(channel, sounds[channel].chunk, loops) == -1)
        printf("%s not played\n", file);
      else
        printf("%s played\n", file);
    }
  } else {
    Mix_Chunk* chunk = Mix_LoadWAV( file );
    Mix_VolumeChunk(chunk, svolume);
    channel = Mix_PlayChannel(-1, chunk, loops);
    if (channel == -1) {
      printf("%s not played\n", file);
    } else {
      sounds[channel].file = std::string(file);
      sounds[channel].playing = true;
      sounds[channel].chunk = chunk;
      printf("%s loaded to %i and played\n", file, channel);
    }
  }
}

void SoundManager::PlaySoundOnce(const char* file)
{
  PlaySound(file, 0);
}

void SoundManager::PlaySoundLoop(const char* file)
{
  PlaySound(file, -1);
}

// assumes sound is currently playing
void SoundManager::PauseSound(const char* file)
{
  for (int i = 0; i < NUM_SOUNDS; i++) {
    if (sounds[i].file.compare(file) == 0) {
      Mix_Pause(i);
      sounds[i].playing = false;
      sounds[i].paused = true;
      printf("%s paused\n", file);
      break;
    }
  }
}

bool SoundManager::SoundPlaying(const char* file)
{
  int channel = SoundLoaded(file);
  if (channel != -1 && sounds[channel].playing)
    return true;
  else
    return false;
}

int SoundManager::SoundLoaded(const char* file)
{
  for (int i = 0; i < NUM_SOUNDS; i++) {
    if (sounds[i].file.compare(file) == 0) {
      return i;
    }
  }
  return -1;
}
