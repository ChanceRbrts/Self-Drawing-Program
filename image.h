#ifndef sdp_image
#define sdp_image

#include "utils.h"
#include "lines.h"

class SDPImage{
    private:
        Lines* lines;
        SDL_Surface* img;
        std::vector<std::vector<cols>> pixels;
        void edgeDetection();
    public:
        SDPImage(char* imageTitle);
        ~SDPImage();
        int getPixelData();
        int getWidth();
        int getHeight();
        void update(double deltaTime, bool spacePress);
        void draw(float scale);
        void drawImage(float scale);
};

#endif