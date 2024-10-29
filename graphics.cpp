#include "main.h"

class Graphics {
private:
    GAMEBITMAP* DrawingSurface;
    GAMEBITMAP* BackgroundBitMap;

public:
    // Constructor
    Graphics(GAMEBITMAP* surface) : DrawingSurface(surface) {
        
    }

    // Destructor
    ~Graphics() {
        // Cleanup resources if needed
    }

    VOID SetBackgroundBitMap(GAMEBITMAP*BackgroundBitMap){
        this->BackgroundBitMap = BackgroundBitMap;
    }

    // Other member functions as needed

    VOID LoadBlackScreen(){
    int32_t MemoryOffset = 0;
    int32_t Starting_Coordinate = GAME_WIDTH*GAME_HEIGHT - GAME_WIDTH;
    PIXEL p = {0};
    
    for(int32_t PixelY = 0; PixelY < GAME_HEIGHT; PixelY++){
        for(int32_t PixelX = 0; PixelX < GAME_WIDTH; PixelX++){

            MemoryOffset = Starting_Coordinate + PixelX - (GAME_WIDTH*PixelY);
            memcpy((PIXEL*)DrawingSurface->memory + MemoryOffset,&p,sizeof(PIXEL));

            }
        }
    }

    VOID LoadWhiteScreen(){
    int32_t MemoryOffset = 0;
    int32_t Starting_Coordinate = GAME_WIDTH*GAME_HEIGHT - GAME_WIDTH;
    PIXEL p = {0};
    p.red = 255;
    p.green = 255;
    p.blue = 255;
    p.alpha = 255;
    
    for(int32_t PixelY = 0; PixelY < GAME_HEIGHT; PixelY++){
        for(int32_t PixelX = 0; PixelX < GAME_WIDTH; PixelX++){

            MemoryOffset = Starting_Coordinate + PixelX - (GAME_WIDTH*PixelY);
            memcpy((PIXEL*)DrawingSurface->memory + MemoryOffset,&p,sizeof(PIXEL));

            }
        }
    }



VOID LoadBitMapToScreen(GAMEBITMAP GameBitMap, int16_t x, int16_t y, int16_t VerticalOffset,int16_t HorizontalOffset){
    x += HorizontalOffset;
    y += VerticalOffset;
    int32_t Starting_Coordinate = ((GAME_HEIGHT*GAME_WIDTH) - GAME_WIDTH) - (GAME_WIDTH*y) + x;
    int32_t Starting_BitMapPixel = (GameBitMap.bitMapInfo.bmiHeader.biHeight * GameBitMap.bitMapInfo.bmiHeader.biWidth) - GameBitMap.bitMapInfo.bmiHeader.biWidth;
    int32_t MemoryOffset = 0;
    int32_t BitMapOffset = 0;
    PIXEL BitmapPixels = {0};
    PIXEL BackgroundPixels = {0};

    
    int32_t startingX = x;
    
    for(int32_t PixelY = 0; PixelY < GameBitMap.bitMapInfo.bmiHeader.biHeight; PixelY++){
        
        for(int32_t PixelX = 0; PixelX < GameBitMap.bitMapInfo.bmiHeader.biWidth; PixelX++){

            MemoryOffset = Starting_Coordinate + PixelX - (GAME_WIDTH*PixelY);
            BitMapOffset = Starting_BitMapPixel + PixelX - (GameBitMap.bitMapInfo.bmiHeader.biWidth * PixelY);
            if (MemoryOffset < 0 || MemoryOffset >= GAME_HEIGHT * GAME_WIDTH) {
                continue; 
            }

            if (BitMapOffset < 0 || BitMapOffset >= GameBitMap.bitMapInfo.bmiHeader.biHeight * GameBitMap.bitMapInfo.bmiHeader.biWidth) {
                continue; 
            }

            

            if(startingX + PixelX < GAME_WIDTH && startingX + PixelX > 0){
            memcpy(&BitmapPixels,(PIXEL*)GameBitMap.memory + BitMapOffset,sizeof(PIXEL));
            if (BitmapPixels.alpha == 255){
                memcpy((PIXEL*)DrawingSurface->memory + MemoryOffset,&BitmapPixels,sizeof(PIXEL));
            }
            }
            
            

        }
    }

}

VOID LoadPixelToScreen(PIXEL pixel, int16_t x, int16_t y){

    int32_t Coordinate = ((GAME_HEIGHT*GAME_WIDTH) - GAME_WIDTH) - (GAME_WIDTH*y) + x;

    
    if (Coordinate < 0 || Coordinate >= GAME_HEIGHT * GAME_WIDTH) 
    {
        return; 
    }


    if(x < GAME_WIDTH && x > 0)
    {
        memcpy((PIXEL*)DrawingSurface->memory + Coordinate,&pixel,sizeof(PIXEL));
    }

}


VOID RenderCirclularParticleToScreen(C_PARTICLE particle) 
{
    int32_t x = particle.x;  
    int32_t y = particle.y;  
    int32_t circle_radius = particle.r;
    int32_t circle_radius_sq = circle_radius * circle_radius;

    
    for (int32_t PixelY = -circle_radius; PixelY <= circle_radius; PixelY++) {
        for (int32_t PixelX = -circle_radius; PixelX <= circle_radius; PixelX++) {
            
            // Calculate the distance from the center of the circle //
            int32_t distanceSquared = (PixelX * PixelX) + (PixelY * PixelY);

            // If the pixel is within the circle //

            if (distanceSquared < circle_radius_sq) {
    
                int32_t screenX = x + PixelX;
                int32_t screenY = y + PixelY;

                
                if (screenX >= 0 && screenX < GAME_WIDTH && screenY >= 0 && screenY < GAME_HEIGHT) {
                    
                    int32_t MemoryOffset = (screenY * GAME_WIDTH) + screenX;

                    
                    memcpy((PIXEL*)DrawingSurface->memory + MemoryOffset, &particle.p, sizeof(PIXEL));
                }
            }
        }
    }
}



VOID RenderLargeParticleToScreen(PARTICLE particle){
    int32_t x = particle.x;
    int32_t y = particle.y;

    int32_t Starting_Coordinate = ((GAME_HEIGHT*GAME_WIDTH) - GAME_WIDTH) - (GAME_WIDTH*y) + x;

    int32_t Starting_ParticlePixel = (particle.h * particle.w) - particle.w;
    
    int32_t MemoryOffset = 0;
    int32_t ParticleOffset = 0;
    PIXEL ParticlePixels = {0};
    PIXEL BackgroundPixels = {0};

    
    int32_t startingX = x;
    
    for(int32_t PixelY = 0; PixelY < particle.h; PixelY++){
        
        for(int32_t PixelX = 0; PixelX < particle.w; PixelX++){

            MemoryOffset = Starting_Coordinate + PixelX - (GAME_WIDTH*PixelY);
            ParticleOffset = Starting_ParticlePixel + PixelX - (particle.w * PixelY);
            if (MemoryOffset < 0 || MemoryOffset >= GAME_HEIGHT * GAME_WIDTH) {
                continue; 
            }

            if (ParticleOffset < 0 || ParticleOffset >= particle.h * particle.w) {
                continue; 
            }

            

            if(startingX + PixelX < GAME_WIDTH && startingX + PixelX > 0){
            // memcpy(&ParticlePixels,(PIXEL*)GameBitMap.memory + ParticleOffset,sizeof(PIXEL));
            
            if (particle.p.alpha == 255) {
                memcpy((PIXEL*)DrawingSurface->memory + MemoryOffset, &particle.p, sizeof(PIXEL));
            }
            
            }
            
            

        }
    }

}


};



