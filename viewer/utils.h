#ifndef UTILS_H_
#define UTILS_H_

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_ttf.h>

#include "plane.h"

typedef struct {
    int x;
    int y;
} Point;


SDL_Texture *create_texture(const char *filename, SDL_Renderer *ren, SDL_Window *win);
void draw_circle(SDL_Renderer *ren, Point center, int radius);
void draw_moving_line(SDL_Renderer *ren, Point center, int radius);
void update_planes(Plane *planes);
int mouse_inside(Point mouse, Plane plane);
void create_text(SDL_Renderer *ren, int x, int y, char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect);
void destroy_texture(SDL_Texture *texture);
void quit_img();

#endif