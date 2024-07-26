#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define WIDTH 800
#define HEIGTH 600

typedef struct {
    float x;
    float y;
    float ang;
} Plane;

int main()
{
    // Iniciar SDL
    int r = SDL_Init(SDL_INIT_VIDEO);
    if (r != 0) {
        perror("SDL_Init error");
        exit(EXIT_FAILURE);
    }

    // Crear una ventana
    SDL_Window *win = SDL_CreateWindow("SDL Basico", 100, 100, WIDTH, HEIGTH, SDL_WINDOW_SHOWN);
    if (win == NULL) {
        perror("SDL_CreateWindow error");
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // Crear renderizador
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL) {
        perror("SDL_Renderer error");
        SDL_DestroyWindow(win);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_Surface *plane_bmp = IMG_Load("plane.png");
    if (plane_bmp == NULL) {
        perror("IMG_Load error");
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        IMG_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_Texture *plane_tex = SDL_CreateTextureFromSurface(ren, plane_bmp);
    SDL_FreeSurface(plane_bmp);
    if (plane_tex == NULL) {
        perror("SDL_CreateTextureFromSurface error");
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        IMG_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    int radius = 280;
    int x_center = WIDTH/2;
    int y_center = HEIGTH/2;

    int x_line = x_center+r;
    int y_line = y_center;
    float ang = 0;

    Plane plane;
    plane.x = WIDTH-150;
    plane.y = 0;
    plane.ang = 135;

    Plane planes[3];
    planes[0].x = WIDTH-150;
    planes[0].y = 0;
    planes[0].ang = 135;

    planes[1].x = 50;
    planes[1].y = 200;
    planes[1].ang = 0;

    planes[2].x = WIDTH-370;
    planes[2].y = HEIGTH-75;
    planes[2].ang = 225;

    int running = 1;
    SDL_Event event;
    while (running) {
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
        }

        // Limpiar pantalla con color
        SDL_SetRenderDrawColor(ren, 40, 40, 40, 255);
        SDL_RenderClear(ren);

        // Punto
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        // SDL_RenderDrawPoint(ren, 10, 10);

        // Linea
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        // SDL_RenderDrawLine(ren, 50, 50, WIDTH-50, 600-50);

        // Rectangulo
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_Rect rect = {WIDTH-220, 10, 200, 75};
        // SDL_RenderDrawRect(ren, &rect);

        // Circulo
        SDL_SetRenderDrawColor(ren, 255, 255, 0, 255);
        

        int x0 = WIDTH/2 - radius;
        int y0 = HEIGTH/2 - radius;

        SDL_Rect rect2 = { x0, y0, 2*radius, 2*radius };
        // SDL_RenderDrawRect(ren, &rect2);

        int x = radius - 1;
        int y = 0;
        int dx = 1;
        int dy = 1;
        int err = dx - (radius << 1);

        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        while (x >= y) {
            SDL_RenderDrawPoint(ren, x_center + x, y_center + y);
            SDL_RenderDrawPoint(ren, x_center + y, y_center + x);
            SDL_RenderDrawPoint(ren, x_center - y, y_center + x);
            SDL_RenderDrawPoint(ren, x_center - x, y_center + y);
            SDL_RenderDrawPoint(ren, x_center - x, y_center - y);
            SDL_RenderDrawPoint(ren, x_center - y, y_center - x);
            SDL_RenderDrawPoint(ren, x_center + y, y_center - x);
            SDL_RenderDrawPoint(ren, x_center + x, y_center - y);

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

        // Linea circulo
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        SDL_RenderDrawLine(ren, x_center, y_center, x_line, y_line);

        // Imagen
        
        // SDL_RenderCopy(ren, plane_tex, NULL, &plane_coords);
        for (int i = 0; i < 3; i++) {
            SDL_Rect plane_coords = { planes[i].x, planes[i].y, 32, 32};
            SDL_RenderCopyEx(ren, plane_tex, NULL, &plane_coords, planes[i].ang, NULL, SDL_FLIP_NONE);
        }

        // Mostrar lo que se ha dibujado
        SDL_RenderPresent(ren);

        // calcular movimiento linea
        const int cycle = 10 * 1000;
        Uint32 ticks = SDL_GetTicks();
        double angle = (double)(ticks % cycle) / cycle * 2.0 * M_PI;

        x_line = x_center + radius * cos(angle);
        y_line = y_center + radius * sin(angle);

        // calcular posicion avions
        planes[0].x -= 0.1;
        planes[0].y += 0.1;

        planes[1].x += 0.1;

        planes[2].x -= 0.1;
        planes[2].y -= 0.1;


        SDL_Delay(16); // Approximately 60 frames per second
    }

    // Liberar recursos
    SDL_DestroyTexture(plane_tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
