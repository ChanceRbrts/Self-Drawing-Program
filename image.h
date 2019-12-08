#ifndef sdp_image
#define sdp_image

#include "utils.h"
#include "lines.h"
#include "math.h"

class SDPImage{
    private:
        Lines* lines;
        SDL_Surface* img;
        float iScale;
        std::vector<std::vector<cols>> pixels;
        void scaleImage(float imgScale);
        void edgeDetection();
    public:
        SDPImage(char* imageTitle, float imgScale);
        ~SDPImage();
        int getPixelData();
        int getWidth();
        int getHeight();
        void update(double deltaTime, bool spacePress);
        void draw(float scale);
        void drawImage(float scale);
};

#endif