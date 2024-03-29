#include "image.h"
#include <string>
#include <thread>

void initialize(){
    // Initialize SDL and SDL_image
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0){
        fprintf(stderr, "ERROR: Couldn't initialize SDL.");
        SDL_Quit();
        exit(1);
    }
    int flag = IMG_INIT_JPG | IMG_INIT_PNG;
    if (IMG_Init(flag) != flag){
        fprintf(stderr, "ERROR: Couldn't initialize SDL Image");
        IMG_Quit();
        SDL_Quit();
        exit(1);
    }
    // Initialize SDL Parameters
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}

void draw(SDPImage* img, SDL_Window* win, float scale){
    // Start the Drawing.
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glPushAttrib(GL_TRANSFORM_BIT|GL_ENABLE_BIT);
    // Draw the image.
    img->draw(scale);
    // Finish the Drawing.
    glPopAttrib();
    glFlush();
    SDL_GL_SwapWindow(win);
}

int main(int argc, char** argv){
    // Make sure we actually have an image in the second argument.
    if (argc < 2){
        fprintf(stderr, "USAGE: ./self-drawing image (scale of display) (scale of image)");
        exit(1);
    }
    
    initialize();
    // Make a window for the image.
    float scale = 1;
    float imgScale = 1;
    if (argc >= 3){
        sscanf(argv[2], "%f", &scale);
    }
    if (argc >= 4){
        sscanf(argv[3], "%f", &imgScale);
    }
    if (imgScale > 1){
        fprintf(stderr, "ERROR: Scale of image can't be more than 1.");
        exit(1);
    }
    // If we have an imgScale of 0.25, our scale should be 4*scale.
    scale = scale/imgScale;
    SDPImage* img = new SDPImage(argv[1], imgScale);
    std::string title = "Drawing " + std::string(argv[1]);
    SDL_Window* win = SDL_CreateWindow(title.c_str(), 0, 0, img->getWidth()*scale, img->getHeight()*scale, SDL_WINDOW_OPENGL);
    SDL_GL_CreateContext(win);
    double curTime = 0;
    bool spacebarPress = false;
    bool spacebarHold = false;
    // Draw Loop
    while (1){
        SDL_Event e;
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_KEYDOWN){
                if (e.key.keysym.sym == SDLK_SPACE){
                    if (!spacebarHold){
                        spacebarPress = true;
                    }
                    spacebarHold = true;
                } else if (e.key.keysym.sym == SDLK_ESCAPE){
                    exit(0);
                }
            } else if (e.type == SDL_KEYUP){
                if (e.key.keysym.sym == SDLK_SPACE){
                    spacebarHold = false;
                }
            } else if (e.type == SDL_QUIT){
                exit(0);
            }
        }
        double newTime = SDL_GetTicks()/1000.0;
        if (curTime != 0){
            double deltaTime = newTime-curTime;
            img->update(deltaTime, spacebarPress);
        }
        curTime = newTime;
        draw(img, win, scale);
        spacebarPress = false;
    }
}
