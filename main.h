#ifndef MAIN_H
#define MAIN_H

#include <string>
#include <windows.h>
#include <winerror.h>
#include <stdio.h>
#include <stdint.h>
#include <algorithm>
#include <random>
#include <stdlib.h> 
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstdlib> 

#include <future>
#include <chrono>
#include <vector>




#define GAME_WIDTH 1920
#define GAME_HEIGHT 1080

#define GAME_BPP 32
#define GAME_DRAWING_AREA_MEMORY_SIZE (GAME_WIDTH * GAME_HEIGHT * (GAME_BPP / 8))
#define DESIRED_MIRCOSECONDS 16667/2
#define MAX_PARTICLE_COUNT 200
#define GRAVITY 10
#define PI 3.14
#define MAX_VELOCITY 3



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD Create_Main_Window(HINSTANCE hInstance);

VOID process_player_input(void);
VOID initBitMap(void);
VOID render_game_frames(void);

BOOL is_already_running(void);


typedef LONG(NTAPI* _NtQueryTimerResolution) (
  OUT PULONG MinimumResolution,
  OUT PULONG MaximumResolution,
  OUT PULONG CurrentResolution );

_NtQueryTimerResolution NtQueryTimerResolution;

typedef struct GAMEBITMAP{
    BITMAPINFO bitMapInfo;
    void* memory;
}GAMEBITMAP;


typedef struct PIXEL32 {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
} PIXEL;




typedef struct PREFORMENCE_DATA{
    uint64_t TotalFramesRendered;
    float RawFPS;
    float CookFPS;
    MONITORINFO MonInfo;
    uint32_t MonWidth;
    uint32_t Monheight;
    LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
    LARGE_INTEGER Frequency;



}PREFORMENCE_DATA;


typedef struct {
    PIXEL p;         
    int32_t x;       
    int32_t y;
    int32_t x_direction; 
    int32_t y_direction; 
    FLOAT y_velocity;
    FLOAT x_velocity;
    int32_t w;
    int32_t h;


} PARTICLE;


typedef struct {
    PIXEL p;         
    
    int32_t x;       
    int32_t y;
    int32_t r;

    int32_t x_direction; 
    int32_t y_direction; 
    
    FLOAT y_velocity;
    FLOAT x_velocity;

    FLOAT x_acceleration;
    FLOAT y_acceleration;

    FLOAT mass;
    

} C_PARTICLE;


VOID LoadBlackScreen();
VOID LoadWhiteScreen();
VOID LoadBitMapToScreen(GAMEBITMAP, int16_t, int16_t,int16_t,int16_t);
VOID process_physics(VOID);
VOID RenderParticleToScreen(PARTICLE);

POINT GetCursorPositionInGameWindow();
VOID HandleParticleCollision(PARTICLE*, PARTICLE*, int32_t);
VOID RenderLargeParticleToScreen(PARTICLE);
VOID HandleGravity(PARTICLE*);
VOID HandleMovement(PARTICLE*);
VOID RenderCirclularParticleToScreen(C_PARTICLE);
VOID SpawnCircleParticle(int32_t,int32_t);
VOID HandleCircularParticleCollision(C_PARTICLE*, C_PARTICLE*, int32_t);
VOID HandleNoGravityMovement(C_PARTICLE*);

#endif 
