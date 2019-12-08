# Self Drawing Program

This is a program that will eventually take an image, break it down into lines, and draw those lines. As of right now, I think this will use OpenGL with SDL.

## Setup
Make sure you have both OpenGL and SDL.
### OSX
You can find the SDL framework here: https://www.libsdl.org/download-2.0.php

You can find the SDL-image framework here: https://www.libsdl.org/projects/SDL_image/

Make sure to get both of the binaries. From there, open the .dmgs, and move the frameworks into /Library/Frameworks.

(If you don't have admin access, move them to ~/Library/Frameworks)

### Linux (Mostly)
NOTE: I built and made this on OS X, so I don't know if this works on Linux.

This probably works, and it seems to have SDL along with it: https://en.wikibooks.org/wiki/OpenGL_Programming/Installation/Linux

If that doesn't work for SDL, follow https://wiki.libsdl.org/Installation.
You may have to build SDL_Image from https://www.libsdl.org/projects/SDL_image/

### Compilation after Steps
Run make to compile the program.
Run `./drawing (image) (displayscale) (imagescale)`, where `displayscale` is the scale of the window relative to the original size of the image and `imagescale` is the size of the image relative to the original size of the image.
