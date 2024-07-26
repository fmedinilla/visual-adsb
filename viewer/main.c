#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "viewer.h"
#include "plane.h"

#define WIDTH 1920
#define HEIGTH 1080

#define PLANE_SIZE 32


typedef struct {
    int x;
    int y;
} Point;


SDL_Texture *create_texture(const char *filename, SDL_Renderer *ren, SDL_Window *win);
void draw_circle(SDL_Renderer *ren, Point center, int radius);
void draw_moving_line(SDL_Renderer *ren, Point center, int radius);
void update_planes(Plane *planes);

int main()
{
    Viewer *viewer = viewer_create(WIDTH, HEIGTH);
    viewer_set_background(viewer, 20, 20, 20, 255);

    // Textura avion
    SDL_Texture *plane_tex = create_texture("plane.png", viewer->ren, viewer->win);
    
    int radius = 500;
    Point center = { WIDTH / 2, HEIGTH / 2 };

    const int MAX_PLANES = 3;
    Plane planes[MAX_PLANES];

    planes[0].x = WIDTH - 100;
    planes[0].y = 0;

    planes[1].x = 0;
    planes[1].y = 300;

    planes[2].x = WIDTH-700;
    planes[2].y = HEIGTH-75;

    SDL_Event event;

    while (viewer_should_run(viewer)) {
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) viewer_stop(viewer);
        }

        viewer_clear(viewer);

        // Circulo
        draw_circle(viewer->ren, center, radius);

        // Linea
        draw_moving_line(viewer->ren, center, radius);

        // Aviones
        for (int i = 0; i < MAX_PLANES; i++) {
            SDL_Rect plane_coords = { planes[i].x, planes[i].y, PLANE_SIZE, PLANE_SIZE };
            SDL_RenderCopyEx(viewer->ren, plane_tex, NULL, &plane_coords, planes[i].dir, NULL, SDL_FLIP_NONE);
        }

        // Mostrar lo que se ha dibujado
        viewer_render(viewer);

        // update
        update_planes(planes);

        // 60 FPS
        SDL_Delay(16);
    }

    // Liberar recursos
    viewer_destroy(viewer);
    SDL_DestroyTexture(plane_tex);
    IMG_Quit();

    return 0;
}

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
    // read ADS-B
    // update planes

    double theta_radians = 0;
    double theta_degrees = 0;

    planes[0].x += -1;
    planes[0].y += 1;
    theta_radians = atan2(1, -1);
    theta_degrees = theta_radians * (180.0 / M_PI);
    planes[0].dir = theta_degrees;

    planes[1].x += 0.5;
    theta_radians = atan2(0, 0.5);
    theta_degrees = theta_radians * (180.0 / M_PI);
    planes[1].dir = theta_degrees;

    planes[2].x += -0.3;
    planes[2].y += -1;
    theta_radians = atan2(-1, -0.3);
    theta_degrees = theta_radians * (180.0 / M_PI);
    planes[2].dir = theta_degrees;
}
