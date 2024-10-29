#ifndef UNICODE
#define UNICODE
#endif 




#include "main.h"
#include "graphics.cpp"
#include "physics.cpp"



// -------------------- | GLOBALS | --------------------//



// Loop Stuff //
BOOL gGameIsRunning;
HWND gGameWindow;




// Graphic stuff //
GAMEBITMAP DrawingSurface;



// Rendering //
PREFORMENCE_DATA gPreformance_Data; 
int32_t gMonitorWidth;
int32_t gMonitorHeight;
GAMEBITMAP Background;
Graphics graphics(&DrawingSurface);
Physics physics;

int32_t LoadingFrameCount = 0;
LPSTR LoadingText = "Loading";



C_PARTICLE circle_particles[MAX_PARTICLE_COUNT];
int32_t cparticle_count;

PIXEL p;

POINT cursor_position;

int32_t monitorWidth;
int32_t monitorHeight;


int16_t LMouseWasDown;



// -------------------- | END OF GLOBALS | --------------------//





// ------------------ | MAIN WINDOWS FUNCTION | ------------------ // 

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)

{
    
    LRESULT result = 0;
    
    if (is_already_running())
    {
        MessageBoxA(NULL, "Another instance is already running", "Error", MB_ICONEXCLAMATION | MB_OK);
        result = GetLastError();
        return result;
    }


    if (Create_Main_Window(hInstance) != ERROR_SUCCESS)
    {
        return 0;
    }

    initBitMap();
    
    if (DrawingSurface.memory == NULL)
    {
        MessageBoxA(NULL, "Failed to allocate memory for bitmap", "Error", MB_ICONEXCLAMATION | MB_OK);
    }
    

    if (timeBeginPeriod(1) == TIMERR_NOCANDO)
    {
        MessageBoxA(NULL, "Unable to set time res", "Error", MB_ICONEXCLAMATION | MB_OK);
        result = GetLastError();
        return result;
    }

    HMODULE NtDllModuleHandle;
    if ((NtDllModuleHandle = GetModuleHandleA("ntdll.dll")) == NULL)
    {
        MessageBoxA(NULL, "Unable to load ntdll.dll", "Error", MB_ICONEXCLAMATION | MB_OK);
        result = GetLastError();
        return result;
    }

    NtQueryTimerResolution = (_NtQueryTimerResolution)(GetProcAddress(NtDllModuleHandle,"NtQueryTimerResolution"));
    if (NtQueryTimerResolution == NULL)
    {
        MessageBoxA(NULL, "Unable to find NtQueryTimerResolution", "Error", MB_ICONEXCLAMATION | MB_OK);
        result = GetLastError();
        return result;
    }

     ULONG minResolution, maxResolution, currentResolution;
     ULONG currentResolutionInMircoSeconds = 0;
     ULONG minResolutionInMircoSeconds = 0;

    if (NtQueryTimerResolution(&minResolution, &maxResolution, &currentResolution) == 0) 
    {
        
          currentResolutionInMircoSeconds = ((100*currentResolution)/1000);
          minResolutionInMircoSeconds = ((100*minResolution)/1000);
         
         printf("Timer res: %lu\n", currentResolution);
         printf("Max Timer res: %lu\n", maxResolution);
         printf("Min Timer res: %lu\n", minResolution);
         printf("Current timer res (mirco): %lu\n", currentResolutionInMircoSeconds);
        
    } 
    
    else 
    {
        MessageBoxA(NULL, "Unable to find NtQueryTimerResolution", "Error", MB_ICONEXCLAMATION | MB_OK);
        result = GetLastError();
        return result;
    }

    HWND hwnd = GetActiveWindow();
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(hwnd, &cursorPos);

    RECT windowRect;
    GetClientRect(hwnd, &windowRect); 

    
    monitorWidth = windowRect.right;
    monitorHeight = windowRect.bottom;

    printf("Mon width: %d\n", monitorWidth);
    printf("Mon height: %d\n", monitorHeight);



    cparticle_count = 0;


    


    MSG msg = { 0 };
    gGameIsRunning = TRUE;
    


    QueryPerformanceFrequency(&gPreformance_Data.Frequency);
    int64_t elapsedMsPerFrameAccumRaw = 0;
    int64_t elapsedMsPerFrameAccumCooked = 0;
    //ShowCursor(FALSE);

     
    // Main Loop // 

    while(gGameIsRunning)
    {

        QueryPerformanceCounter(&gPreformance_Data.StartingTime);
         
        
        while (PeekMessageA(&msg,gGameWindow, 0,0,PM_REMOVE))
        {    
            DispatchMessageA(&msg);
            
        }

       
        process_player_input();
        process_physics();
        render_game_frames();

        
        QueryPerformanceCounter(&gPreformance_Data.EndingTime);
        gPreformance_Data.ElapsedMicroseconds.QuadPart = gPreformance_Data.EndingTime.QuadPart - gPreformance_Data.StartingTime.QuadPart;
        gPreformance_Data.ElapsedMicroseconds.QuadPart *= 1000000;
        gPreformance_Data.ElapsedMicroseconds.QuadPart /= gPreformance_Data.Frequency.QuadPart;
        elapsedMsPerFrameAccumRaw += gPreformance_Data.ElapsedMicroseconds.QuadPart;
        
        
        while(gPreformance_Data.ElapsedMicroseconds.QuadPart < (DESIRED_MIRCOSECONDS))
        {

            
            gPreformance_Data.ElapsedMicroseconds.QuadPart = gPreformance_Data.EndingTime.QuadPart - gPreformance_Data.StartingTime.QuadPart;
            gPreformance_Data.ElapsedMicroseconds.QuadPart *= 1000000;
            gPreformance_Data.ElapsedMicroseconds.QuadPart /= gPreformance_Data.Frequency.QuadPart;
            
            /*

            Perfect 60 FPS Hack 

            if (gPreformance_Data.ElapsedMicroseconds.QuadPart < (DESIRED_MIRCOSECONDS - minResolutionInMircoSeconds)){
                Sleep(1);
            }
            
            
            if (gPreformance_Data.ElapsedMicroseconds.QuadPart < (DESIRED_MIRCOSECONDS - currentResolutionInMircoSeconds)){
                Sleep(0);
            }
            */

            if (gPreformance_Data.ElapsedMicroseconds.QuadPart < (LONGLONG)(DESIRED_MIRCOSECONDS - (currentResolutionInMircoSeconds*2)))
            {
                Sleep(1);
            }

            QueryPerformanceCounter(&gPreformance_Data.EndingTime);
              
        }
        
        
        gPreformance_Data.TotalFramesRendered++;

        elapsedMsPerFrameAccumCooked += gPreformance_Data.ElapsedMicroseconds.QuadPart;

          
        if (gPreformance_Data.TotalFramesRendered % 100 == 0){

            float AvgMicroSecPerFrameCooked = (elapsedMsPerFrameAccumCooked / 100);
            float AvgMsPerFrameCooked = AvgMicroSecPerFrameCooked / 1000; 
            gPreformance_Data.CookFPS = (1.0f / AvgMsPerFrameCooked) * 1000;

            float AvgMicroSecPerFrameRaw = (elapsedMsPerFrameAccumRaw / 100);
            float AvgMsPerFrameRaw = AvgMicroSecPerFrameRaw / 1000; 
            gPreformance_Data.RawFPS = (1.0f / AvgMsPerFrameRaw) * 1000;
            
            elapsedMsPerFrameAccumCooked = 0;
            elapsedMsPerFrameAccumRaw = 0;

        }
        
       
    }

    
    return 0;
}
// ------------------ | END OF MAIN WINDOWS FUNCTION | ------------------ // 




// ------------------ | WINDOWS CALLBACK FUNCTION | ------------------ //

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    
    switch (uMsg)
    {
    case WM_DESTROY:
        gGameIsRunning = FALSE;
        PostQuitMessage(0);
        return 0;

    

    case WM_PAINT:
        {
           
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

            EndPaint(hwnd, &ps);
        }
        return 0;
    
    
    
    case WM_CLOSE:
    ShowCursor(TRUE);
    if (MessageBox(hwnd, L"Really quit?", L"Project", MB_OKCANCEL) == IDOK)
    {
        DestroyWindow(hwnd);
    }
    // Else: User canceled. Do nothing.
    ShowCursor(FALSE);
    return 0;
    
    
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// ------------------ | END OF WINDOWS CALLBACK FUNCTION | ------------------ //




// ------------------ | MAIN WINDOW | ------------------ // 

DWORD Create_Main_Window(HINSTANCE hInstance)
{
    DWORD result = ERROR_SUCCESS;
    const wchar_t CLASS_NAME[]  = L"Sample Window Class";
    WNDCLASS windowClass = { 0 };

    windowClass.lpfnWndProc   = WindowProc;
    windowClass.hInstance     = hInstance;
    windowClass.lpszClassName = CLASS_NAME;
    
    //windowClass.hIcon = LoadIconA(NULL,(LPCSTR)IDI_APPLICATION);
       
    
    if (RegisterClassW(&windowClass) == 0) 
    {
        result = GetLastError();
        MessageBox(NULL,L"Window Reg Failed",L"ERROR",MB_ICONEXCLAMATION | MB_OK);
        return result;
    }

    
    

    gGameWindow = CreateWindowExW(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class 
        L"Project",    // Window text
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
        );

    if (gGameWindow == NULL)
    {
        result = GetLastError();
        return result;
    }

    gPreformance_Data.MonInfo = {sizeof(MONITORINFO)};
    if (GetMonitorInfoA(MonitorFromWindow(gGameWindow,MONITOR_DEFAULTTOPRIMARY),&gPreformance_Data.MonInfo) == 0){
        result = 0x80261001; // ERROR_MONITOR_NO_DESCRIPTOR 
        return result;
    }

    gMonitorWidth = gPreformance_Data.MonInfo.rcMonitor.right - gPreformance_Data.MonInfo.rcMonitor.left;
    gMonitorHeight = gPreformance_Data.MonInfo.rcMonitor.bottom - gPreformance_Data.MonInfo.rcMonitor.top;
    
    if (SetWindowPos(gGameWindow,HWND_TOP,gPreformance_Data.MonInfo.rcMonitor.left,gPreformance_Data.MonInfo.rcMonitor.top,gMonitorWidth,gMonitorHeight,SWP_SHOWWINDOW) == 0){
        result = GetLastError();
        return result;
    }

    if (SetWindowLongPtrA(gGameWindow,GWL_STYLE, (WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~WS_OVERLAPPEDWINDOW) == 0){
        result = GetLastError();
        return result;
    } 

   return result;
        
    
}

// ------------------ | END OF MAIN WINDOW | ------------------ // 





// --------------------- | PLAYER CONTROLLER | ------------------ //


VOID process_player_input(void)
{
    short espKeyDown = GetAsyncKeyState(VK_ESCAPE);
    
    int16_t LMouseDown = GetAsyncKeyState(VK_LBUTTON);
    POINT point;

   
    
    if (espKeyDown)
    {
        SendMessageA(gGameWindow,WM_CLOSE,0,0);
    }

    if(LMouseDown && !LMouseWasDown)
    {
        if (GetCursorPos(&point))
        {
            point = GetCursorPositionInGameWindow();
            
            SpawnCircleParticle(point.x,point.y);
        }
        
    }

    LMouseWasDown = LMouseDown;
    
}

// --------------------- | END OF PLAYER CONTROLLER | ------------------ //

// --------------------- | PHYSICS CONTROLLER| ------------------ //

VOID process_physics(void)

{
    
    for(int i = 0; i < cparticle_count; i++)
    
    {

    physics.HandleCircularParticleCollision(&circle_particles[i], circle_particles,cparticle_count);
    physics.HandleNoGravityMovement(&circle_particles[i]);
    
    }
    
    

    return;
}

// --------------------- | END OF PHYSICS CONTROLLER | ------------------ //



// --------------------- | LOADING FUNCTIONS | --------------------- //

DWORD Load32BppBitmapFromFile(LPCSTR fileName,GAMEBITMAP* GameBitMap){
    DWORD Error = ERROR_SUCCESS;
    HANDLE FileHandle = INVALID_HANDLE_VALUE;
    WORD BitMapHeader = 0;
    DWORD PixelOffset = 0;
    DWORD BytesToRead = 2;

    FileHandle = CreateFileA(fileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (FileHandle == INVALID_HANDLE_VALUE){
        Error = GetLastError();
        CloseHandle(FileHandle);
        return Error;
    }

    
    if(ReadFile(FileHandle,&BitMapHeader,2,&BytesToRead,NULL) == 0){
        Error = GetLastError();
        CloseHandle(FileHandle);
        return Error;
    }
    

    if (BitMapHeader != 0x4d42){
        Error = ERROR_FILE_INVALID;
        CloseHandle(FileHandle);
        return Error;
    }

    if(SetFilePointer(FileHandle,0XA,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER){
        Error = GetLastError();
        CloseHandle(FileHandle);
        return Error;
    }
    

    if(ReadFile(FileHandle,&PixelOffset,sizeof(DWORD),&BytesToRead,NULL) == 0){
        Error = GetLastError();
        CloseHandle(FileHandle);
        return Error;
    }

    if(SetFilePointer(FileHandle,0XE,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER){
        Error = GetLastError();
        CloseHandle(FileHandle);
        return Error;
    }

    if(ReadFile(FileHandle,&GameBitMap->bitMapInfo.bmiHeader,sizeof(BITMAPINFOHEADER),&BytesToRead,NULL) == 0){
        Error = GetLastError();
        CloseHandle(FileHandle);
        return Error;
    }

    GameBitMap->memory = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,GameBitMap->bitMapInfo.bmiHeader.biSizeImage);
    if (GameBitMap->memory == NULL){
        Error = ERROR_NOT_ENOUGH_MEMORY;
        CloseHandle(FileHandle);
        return Error;
    }

    if(SetFilePointer(FileHandle,PixelOffset,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER){
        Error = GetLastError();
        CloseHandle(FileHandle);
        return Error;
    }
    
    
    if(ReadFile(FileHandle,GameBitMap->memory,GameBitMap->bitMapInfo.bmiHeader.biSizeImage,&BytesToRead,NULL) == 0){
        Error = GetLastError();

        CloseHandle(FileHandle);
        return Error;
    }
    

    CloseHandle(FileHandle);
    return Error;

}

DWORD LoadSpriteFromSpriteSheet(GAMEBITMAP SpriteSheet, GAMEBITMAP *player_spite_box, int16_t SpriteCountRow,int16_t SpriteCountCol, int16_t Row, int16_t Col){
    DWORD Error = ERROR_SUCCESS;
    int32_t Starting_Address = 0;
    int32_t Offset = 0;
    
    player_spite_box->bitMapInfo.bmiHeader.biSize = sizeof(player_spite_box->bitMapInfo.bmiHeader);
    player_spite_box->bitMapInfo.bmiHeader.biWidth = 64;
    player_spite_box->bitMapInfo.bmiHeader.biHeight = 64;
    player_spite_box->bitMapInfo.bmiHeader.biSizeImage = 4096*4;
    
    player_spite_box->bitMapInfo.bmiHeader.biBitCount = GAME_BPP;
    player_spite_box->bitMapInfo.bmiHeader.biCompression = 0;
    player_spite_box->bitMapInfo.bmiHeader.biPlanes = 1;
    
    
    player_spite_box->memory = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,player_spite_box->bitMapInfo.bmiHeader.biSizeImage);
    if (player_spite_box->memory == NULL){
        Error = ERROR_NOT_ENOUGH_MEMORY;
        return Error;
    }


    Row *= SpriteSheet.bitMapInfo.bmiHeader.biHeight/SpriteCountRow;
    
    int32_t spriteSheetRowIndex = SpriteSheet.bitMapInfo.bmiHeader.biWidth * 4;  
    int32_t spriteSheetRowCol = 128; 
    int32_t spriteSheetPointer = (Row*spriteSheetRowIndex) + (spriteSheetRowCol*Col); // Width * 4 * index of height (jumps by 64)
    int32_t memoryOffset = 0;
    
    
    for (int i = 0; i < SpriteSheet.bitMapInfo.bmiHeader.biHeight/SpriteCountRow; i++) {
    memcpy((char*)player_spite_box->memory + memoryOffset, (char*)SpriteSheet.memory +spriteSheetPointer , 128);
    spriteSheetPointer += (SpriteSheet.bitMapInfo.bmiHeader.biWidth)*4;
    memoryOffset += 64*4;
    }
    return Error;
}


// --------------------- | END OF LOADING FUNCTIONS | --------------------- //



// ---------------- | MAIN RENDERING FUNCTION | ---------------- //

VOID render_game_frames(void){
    
    graphics.LoadBlackScreen();
    
    for(int i = 0; i < cparticle_count; i++){
        graphics.RenderCirclularParticleToScreen(circle_particles[i]);
    }
   
    
    
    HDC deviceContext = GetDC(gGameWindow);
    StretchDIBits(deviceContext,0,0,gMonitorWidth,gMonitorHeight,0,0,GAME_WIDTH,GAME_HEIGHT,DrawingSurface.memory,&DrawingSurface.bitMapInfo,DIB_RGB_COLORS,SRCCOPY);
    
    

    char fpsBuffer[64] = {0};
    
    sprintf(fpsBuffer, "Cooked FPS: %.01f Raw FPS: %.01f",gPreformance_Data.CookFPS,gPreformance_Data.RawFPS);

    SetTextColor(deviceContext, RGB(255, 255, 255));  

    SetBkMode(deviceContext, TRANSPARENT );

    TextOutA( deviceContext, 0, 0, fpsBuffer, strlen( fpsBuffer ) );
     
    
    ReleaseDC(gGameWindow,deviceContext);

}

// ---------------- | END OF MAIN RENDERING FUNCTION | ---------------- //




// ----------------------- | Initalizing Functions | ----------------------- //

VOID initBitMap(void)
{
     
     
     DrawingSurface.bitMapInfo.bmiHeader.biSize = sizeof(DrawingSurface.bitMapInfo.bmiHeader);
     DrawingSurface.bitMapInfo.bmiHeader.biWidth = GAME_WIDTH;
     DrawingSurface.bitMapInfo.bmiHeader.biHeight = GAME_HEIGHT;
     DrawingSurface.bitMapInfo.bmiHeader.biBitCount = GAME_BPP;
     DrawingSurface.bitMapInfo.bmiHeader.biCompression = 0;
     DrawingSurface.bitMapInfo.bmiHeader.biPlanes = 1;
     DrawingSurface.memory = VirtualAlloc(NULL,GAME_DRAWING_AREA_MEMORY_SIZE,MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

}



//  ----------------------- | MISC FUNCTIONS  | ----------------------- //  




BOOL is_already_running(void)
{
    
    HANDLE MUTEX_LOCK = NULL; 
    MUTEX_LOCK = CreateMutexA(NULL, FALSE, "PROC_MUTEX");

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        return TRUE;
    }
    else
    {
    return FALSE;    
    }
    
}




VOID SpawnCircleParticle(int32_t x, int32_t y)
{
    if(cparticle_count >= MAX_PARTICLE_COUNT)
    {
        return;
    }

    C_PARTICLE particle;

    // Randomize the radius (between 25 and 50)
    particle.r = rand() % 25 + 25;  

    // Randomize the color (each channel between 0 and 255)
    particle.p.red = rand() % 256;
    particle.p.green = rand() % 256;
    particle.p.blue = rand() % 256;
    particle.p.alpha = 255;  // Full opacity

    particle.mass = rand() % 100 + 100;
    
    // Set position
    particle.x = x;
    particle.y = GAME_HEIGHT - y;

    // Randomly set x and y velocities between 1 and 2
    particle.y_velocity = rand() % 2 + 1;  // Either 1 or 2
    particle.x_velocity = rand() % 2 + 1;  // Either 1 or 2

    // Randomly set x and y directions (either +1 or -1)
    particle.y_direction = (rand() % 2 == 0) ? 1 : -1;
    particle.x_direction = (rand() % 2 == 0) ? 1 : -1;

    particle.x_acceleration = 0;
    particle.y_acceleration = 0;

    // Add the new particle to the array
    circle_particles[cparticle_count] = particle;
    cparticle_count++;
}






POINT GetCursorPositionInGameWindow() {
    
    POINT cursorPos;
    GetCursorPos(&cursorPos);
        
    POINT gameCursorPos;
    gameCursorPos.x = (cursorPos.x * GAME_WIDTH) / monitorWidth;
    gameCursorPos.y = (cursorPos.y * GAME_HEIGHT) / monitorHeight;

    
    return gameCursorPos;
}



//  ----------------------- | END OF MISC FUNCTIONS |  ----------------------- // 

