#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <stdlib.h>
#include "viewer.h"

Viewer *viewer_create(int width, int heigth)
{
    Viewer *viewer = malloc(sizeof(Viewer));

    viewer->width = width;
    viewer->heigth = heigth;
    viewer->state = RUNNING;

    SDL_Color bg_color = {0, 0, 0, 255};
    viewer->background = bg_color;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        perror("SDL_Init error");
        exit(EXIT_FAILURE);
    }

    viewer->win = SDL_CreateWindow("SDL Basico", 100, 100, viewer->width, viewer->heigth, SDL_WINDOW_SHOWN);
    if (viewer->win == NULL) {
        perror("SDL_CreateWindow error");
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    viewer->ren = SDL_CreateRenderer(viewer->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (viewer->ren == NULL) {
        perror("SDL_Renderer error");
        SDL_DestroyWindow(viewer->win);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    return viewer;
}

void viewer_destroy(Viewer *viewer)
{
    SDL_DestroyRenderer(viewer->ren);
    SDL_DestroyWindow(viewer->win);
    free(viewer);
    SDL_Quit();
}

void viewer_set_background(Viewer *viewer, int r, int g, int b, int a)
{
    viewer->background.r = r;
    viewer->background.g = g;
    viewer->background.b = b;
    viewer->background.a = a;
}

void viewer_clear(Viewer *viewer)
{
    SDL_SetRenderDrawColor(viewer->ren, viewer->background.r, viewer->background.g, viewer->background.b, viewer->background.a);
    SDL_RenderClear(viewer->ren);
}

int viewer_should_run(Viewer *viewer)
{
    return viewer->state == RUNNING;
}

void viewer_stop(Viewer *viewer)
{
    viewer->state = STOP;
}

void viewer_render(Viewer *viewer)
{
    SDL_RenderPresent(viewer->ren);
}
