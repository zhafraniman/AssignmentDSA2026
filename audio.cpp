#include "audio.h"
#include <raylib.h>

Music AudioManager::mainTheme;
Music AudioManager::battleTheme;
Sound AudioManager::hitSound;
Sound AudioManager::chestOpen;
bool  AudioManager::overworldActive = false;
bool  AudioManager::battleActive    = false;

// CONSTRUCTOR & DESTRUCTOR
void AudioManager::Init() {
}

void AudioManager::CleanUp() {
    // When the game closes, this safely deletes the audio from your RAM
    UnloadMusicStream(mainTheme);
    UnloadMusicStream(battleTheme);
    UnloadSound(hitSound);
    UnloadSound(chestOpen);
}

// LOADING
void AudioManager::LoadFiles() {
    // Load the files from your hard drive
    mainTheme = LoadMusicStream("src/audio/mainTheme.mp3");
    battleTheme = LoadMusicStream("src/audio/battleTheme.mp3");
    hitSound = LoadSound("src/audio/hitSound.wav");
    chestOpen = LoadSound("src/audio/chest.wav");

    mainTheme.looping   = true;
    battleTheme.looping = true;
}

// MUSIC STREAMING (Must run every frame)
void AudioManager::UpdateMusic() {
    UpdateMusicStream(mainTheme);
    UpdateMusicStream(battleTheme);
}

// PLAYBACK COMMANDS
void AudioManager::PlayOverworldMusic() {
    if (overworldActive) return; // already playing -> don't restart from 0
    PlayMusicStream(mainTheme);
    overworldActive = true;
}
 
void AudioManager::PauseOverworldMusic() {
    PauseMusicStream(mainTheme); // Freezes the song in place
}
 
void AudioManager::ResumeOverworldMusic() {
    ResumeMusicStream(mainTheme); // Unfreezes the song
}
 
void AudioManager::PlayBattleMusic() {
    if (battleActive) return; // already mid-battle -> don't restart from 0
    PlayMusicStream(battleTheme);
    battleActive = true;
}
 
void AudioManager::StopBattleMusic() {
    if (!battleActive) return;
    StopMusicStream(battleTheme);
    battleActive = false;
}
 
void AudioManager::PlayHitSound() {
    PlaySound(hitSound);
}
 
void AudioManager::PlayChestOpen() {
    PlaySound(chestOpen);
}