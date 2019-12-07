#include "image.h"

SDPImage::SDPImage(char* imageTitle){
    // Load an image.
    img = IMG_Load(imageTitle);
    if (!img){
        fprintf(stderr, "ERROR: Image %s doesn't exist.", imageTitle);
        exit(1);
    }

    // Get the pixel data.
    unsigned int* pix = (unsigned int*)(img->pixels);
    for (int i = 0; i < img->h; i++){
        std::vector<cols> colors ;
        SDL_PixelFormat* fmt = img->format;
        for (int j = 0; j < img->w; j++){
            // Credit to https://wiki.libsdl.org/SDL_PixelFormat
            float r = (((pix[i*img->w+j]&fmt->Rmask)>>fmt->Rshift)<<fmt->Rloss)*1.0/255.0;
            float g = (((pix[i*img->w+j]&fmt->Gmask)>>fmt->Gshift)<<fmt->Gloss)*1.0/255.0;
            float b = (((pix[i*img->w+j]&fmt->Bmask)>>fmt->Bshift)<<fmt->Bloss)*1.0/255.0;
            float a = (((pix[i*img->w+j]&fmt->Amask)>>fmt->Ashift)<<fmt->Aloss)*1.0/255.0;
            colors.push_back({r, g, b, a});
        }
        pixels.push_back(colors);
    }
    edgeDetection();
}

SDPImage::~SDPImage(){
    SDL_FreeSurface(img);
}

void SDPImage::draw(float scale){
    drawImage(scale);
}

void SDPImage::drawImage(float scale){
    // Draw the image through pixel values. (Proof of concept. Obviously, this is super slow.)
    float* colorPoints = new float[img->w*img->h*3];
    float* vertexPoints = new float[img->w*img->h*3];
    glPointSize(scale);
    for (int i = 0; i < img->h; i++){
        for (int j = 0; j < img->w; j++){
            colorPoints[(i*img->w+j)*3] = pixels[i][j].r;
            colorPoints[(i*img->w+j)*3+1] = pixels[i][j].g;
            colorPoints[(i*img->w+j)*3+2] = pixels[i][j].b;
            // Vertex Translation Stuff
            vertexPoints[(i*img->w+j)*3] = j*2.0/(img->w*1.0)-1;
            vertexPoints[(i*img->w+j)*3+1] = 1-i*2.0/(img->h*1.0);
            vertexPoints[(i*img->w+j)*3+2] = 0;
        }
    }
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertexPoints);
    glColorPointer(3, GL_FLOAT, 0, colorPoints);
    glDrawArrays(GL_POINTS, 0, img->w*img->h);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

int SDPImage::getWidth(){
    return img->w;
}

int SDPImage::getHeight(){
    return img->h;
}

void SDPImage::edgeDetection(){
    std::vector<std::vector<cols>> newPixels;
    // For now, let's actually replace the pixels with a very simple edge detection algorithm.
    for (int y = 0; y < pixels.size(); y++){
        std::vector<cols> newRow;
        for (int x = 0; x < pixels[y].size(); x++){
            float weightEdge = -1;
            float weightPixel = 8;
            float avgR = pixels[y][x].r*weightPixel;
            float avgG = pixels[y][x].g*weightPixel;
            float avgB = pixels[y][x].b*weightPixel;
            // KERNEL
            for (int i = -1; i < 2; i++){
                for (int j = -1; j < 2; j++){
                    float addR = pixels[y][x].r;
                    float addG = pixels[y][x].g;
                    float addB = pixels[y][x].b;
                    if (y+i >= 0 && y+i < pixels.size() && x+j >= 0 && x+j < pixels[0].size()){
                        addR = pixels[y+i][x+j].r;
                        addG = pixels[y+i][x+j].g;
                        addB = pixels[y+i][x+j].b;
                    }
                    if (i != 0 || j != 0){
                        avgR += weightEdge*addR;
                        avgG += weightEdge*addG;
                        avgB += weightEdge*addB;
                    }
                }
            }
            float trueAvg = (abs(avgR)+abs(avgG)+abs(avgB))/3;
            trueAvg = 1-trueAvg;
            //trueAvg = trueAvg > 0.9?1:trueAvg/1.5;
            newRow.push_back({trueAvg, trueAvg, trueAvg, 1});
        }
        newPixels.push_back(newRow);
    }
    pixels.swap(newPixels);
    newPixels.clear();
}
