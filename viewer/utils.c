#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "utils.h"
#include "plane.h"


SDL_Texture *create_texture(const char *filename, SDL_Renderer *ren, SDL_Window *win)
{
    SDL_Surface *bmp = IMG_Load(filename);
    if (bmp == NULL) {
        perror("IMG_Load error");
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        IMG_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, bmp);
    SDL_FreeSurface(bmp);
    if (tex == NULL) {
        perror("SDL_CreateTextureFromSurface error");
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        IMG_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    return tex;
}

void draw_circle(SDL_Renderer *ren, Point center, int radius)
{
    // Circulo
    int x = radius - 1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    while (x >= y) {
        SDL_RenderDrawPoint(ren, center.x + x, center.y + y);
        SDL_RenderDrawPoint(ren, center.x + y, center.y + x);
        SDL_RenderDrawPoint(ren, center.x - y, center.y + x);
        SDL_RenderDrawPoint(ren, center.x - x, center.y + y);
        SDL_RenderDrawPoint(ren, center.x - x, center.y - y);
        SDL_RenderDrawPoint(ren, center.x - y, center.y - x);
        SDL_RenderDrawPoint(ren, center.x + y, center.y - x);
        SDL_RenderDrawPoint(ren, center.x + x, center.y - y);

        if (err <= 0) {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0) {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}

void draw_moving_line(SDL_Renderer *ren, Point center, int radius)
{
    const int cycle = 10 * 1000;
    Uint32 ticks = SDL_GetTicks();
    double angle = (double)(ticks % cycle) / cycle * 2.0 * M_PI;

    Point line_p;
    line_p.x = center.x + radius * cos(angle);
    line_p.y = center.y + radius * sin(angle);

    SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
    SDL_RenderDrawLine(ren, center.x, center.y, line_p.x, line_p.y);
}

void update_planes(Plane *planes)
{
    double theta_radians = 0;
    double theta_degrees = 0;

    planes[0].x += -PLANE_SPEED;
    planes[0].y += PLANE_SPEED;
    theta_radians = atan2(PLANE_SPEED, -PLANE_SPEED);
    theta_degrees = theta_radians * (180.0 / M_PI);
    planes[0].dir = theta_degrees;

    planes[1].x += PLANE_SPEED;
    theta_radians = atan2(0, PLANE_SPEED);
    theta_degrees = theta_radians * (180.0 / M_PI);
    planes[1].dir = theta_degrees;

    planes[2].x += -PLANE_SPEED;
    planes[2].y += -PLANE_SPEED;
    theta_radians = atan2(-PLANE_SPEED, -PLANE_SPEED);
    theta_degrees = theta_radians * (180.0 / M_PI);
    planes[2].dir = theta_degrees;
}

int mouse_inside(Point mouse, Plane plane)
{
    int x_axis = mouse.x >= plane.x && mouse.x <= (plane.x + PLANE_SIZE);
    int y_axis = mouse.y >= plane.y && mouse.y <= (plane.y + PLANE_SIZE);

    int inside = x_axis && y_axis;
    // if (inside) printf("inside\n");
    return inside;
}

void create_text(SDL_Renderer *ren, int x, int y, char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect)
{
    int text_w, text_h;
    SDL_Surface *surface;
    SDL_Color text_color = {255, 255, 255, 255};

    surface = TTF_RenderText_Solid(font, text, text_color);
    *texture = SDL_CreateTextureFromSurface(ren, surface);
    text_w = surface->w;
    text_h = surface->h;
    SDL_FreeSurface(surface);

    rect->x = x;
    rect->y = y;
    rect->w = text_w;
    rect->h = text_h;
}

void destroy_texture(SDL_Texture *texture)
{
    SDL_DestroyTexture(texture);
}

void quit_img()
{
    IMG_Quit();
}
