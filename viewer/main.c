#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "plane.h"

#define WIDTH 1920
#define HEIGTH 1080

#define PLANE_SIZE 32


typedef struct {
    int x;
    int y;
} Point;


typedef enum {
    STOP,
    RUNNING
} State;


void sdl_init();
SDL_Window *create_window();
SDL_Renderer *create_renderer(SDL_Window *win);
SDL_Texture *create_texture(const char *filename, SDL_Renderer *ren, SDL_Window *win);
void draw_circle(SDL_Renderer *ren, Point center, int radius);
void draw_moving_line(SDL_Renderer *ren, Point center, Point line_p);
void calc_line_movement(Point *line_p, Point center, int radius);
void update_planes(Plane *planes);

int shouldRunning(State state);
void stopRunning(State *state);


int main()
{
    // Iniciar SDL
    sdl_init();

    // Crear una ventana
    SDL_Window *win = create_window();

    // Crear renderizador
    SDL_Renderer *ren = create_renderer(win);

    // Textura avion
    SDL_Texture *plane_tex = create_texture("plane.png", ren, win);
    
    int radius = 500;
    Point center = { WIDTH / 2, HEIGTH / 2 };

    Point line_p = { 0, 0 };

    const int MAX_PLANES = 3;
    Plane planes[MAX_PLANES];

    planes[0].x = WIDTH - 100;
    planes[0].y = 0;
    planes[0].dir = 135;

    planes[1].x = 0;
    planes[1].y = 300;
    planes[1].dir = 0;

    planes[2].x = WIDTH-300;
    planes[2].y = HEIGTH-75;
    planes[2].dir = 225;

    State state = RUNNING;
    SDL_Event event;

    while (shouldRunning(state)) {
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) stopRunning(&state);
        }

        // Limpiar pantalla con color
        SDL_SetRenderDrawColor(ren, 40, 40, 40, 255);
        SDL_RenderClear(ren);

        // Circulo
        draw_circle(ren, center, radius);

        // Linea
        draw_moving_line(ren, center, line_p);

        // Aviones
        for (int i = 0; i < MAX_PLANES; i++) {
            SDL_Rect plane_coords = { planes[i].x, planes[i].y, PLANE_SIZE, PLANE_SIZE };
            SDL_RenderCopyEx(ren, plane_tex, NULL, &plane_coords, planes[i].dir, NULL, SDL_FLIP_NONE);
        }

        // Mostrar lo que se ha dibujado
        SDL_RenderPresent(ren);

        // calcular movimiento linea
        calc_line_movement(&line_p, center, radius);

        // update
        update_planes(planes);

        // 60 FPS
        SDL_Delay(16);
    }

    // Liberar recursos
    SDL_DestroyTexture(plane_tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

void sdl_init()
{
    int r = SDL_Init(SDL_INIT_VIDEO);
    if (r != 0) {
        perror("SDL_Init error");
        exit(EXIT_FAILURE);
    }
}

SDL_Window *create_window()
{
    SDL_Window *win = SDL_CreateWindow("SDL Basico", 100, 100, WIDTH, HEIGTH, SDL_WINDOW_SHOWN);
    if (win == NULL) {
        perror("SDL_CreateWindow error");
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    return win;
}

SDL_Renderer *create_renderer(SDL_Window *win)
{
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL) {
        perror("SDL_Renderer error");
        SDL_DestroyWindow(win);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    return ren;
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

void draw_moving_line(SDL_Renderer *ren, Point center, Point line_p)
{
    // Linea circulo
    SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
    SDL_RenderDrawLine(ren, center.x, center.y, line_p.x, line_p.y);
}

void calc_line_movement(Point *line_p, Point center, int radius)
{
    const int cycle = 10 * 1000;
    Uint32 ticks = SDL_GetTicks();
    double angle = (double)(ticks % cycle) / cycle * 2.0 * M_PI;

    line_p->x = center.x + radius * cos(angle);
    line_p->y = center.y + radius * sin(angle);
}

void update_planes(Plane *planes)
{
    // read ADS-B
    // update planes

    planes[0].x -= 1;
    planes[0].y += 1;

    planes[1].x += 1;

    planes[2].x -= 1;
    planes[2].y -= 1;
}

int shouldRunning(State state)
{
    return state == RUNNING;
}

void stopRunning(State *state)
{
    *state = STOP;
}