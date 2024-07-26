#ifndef VIEWER_H_
#define VIEWER_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>

typedef enum {
    STOP,
    RUNNING
} ViewerState;

typedef struct {
    int width;
    int heigth;
    SDL_Window *win;
    SDL_Renderer *ren;
    SDL_Color background;
    ViewerState state;
} Viewer;

Viewer *viewer_create(int width, int heigth);
void viewer_destroy(Viewer *viewer);
void viewer_set_background(Viewer *viewer, int r, int g, int b, int a);
void viewer_clear(Viewer *viewer);
int viewer_should_run(Viewer *viewer);
void viewer_stop(Viewer *viewer);
void viewer_render(Viewer *viewer);

#endif