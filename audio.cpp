#include "audio.h"
#include <raylib.h>

Music AudioManager::mainTheme;
Music AudioManager::battleTheme;
Sound AudioManager::hitSound;
Sound AudioManager::chestOpen;

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
}

// MUSIC STREAMING (Must run every frame)
void AudioManager::UpdateMusic() {
    UpdateMusicStream(mainTheme);
}

// PLAYBACK COMMANDS
void AudioManager::PlayOverworldMusic() {
    PlayMusicStream(mainTheme);
}

void AudioManager::PauseOverworldMusic() {
    PauseMusicStream(mainTheme); // Freezes the song in place
}

void AudioManager::ResumeOverworldMusic() {
    ResumeMusicStream(mainTheme); // Unfreezes the song
}

void AudioManager::PlayBattleMusic() {
    PlayMusicStream(battleTheme);
}

void AudioManager::StopBattleMusic() {
    StopMusicStream(battleTheme);
}

void AudioManager::PlayHitSound() {
    PlaySound(hitSound);
}

void AudioManager::PlayChestOpen() {
    PlaySound(chestOpen);
}