#include <string.h>

#include "utils.h"
#include "viewer.h"
#include "plane.h"

#define WIDTH 720
#define HEIGTH 720


#ifdef WIN32
#define MAIN WinMain
#else
#define MAIN main
#endif

int MAIN(int argc, char *argv[]) {
    Viewer *viewer = viewer_create(WIDTH, HEIGTH);
    viewer_set_background(viewer, 20, 20, 20, 255);

    // Fuente
    TTF_Init();
    TTF_Font *font = TTF_OpenFont("FreeSans.ttf", 12);
    SDL_Color white_color = { 255, 255, 255, 255 };

    // Textura avion
    SDL_Texture *plane_tex = create_texture("plane.png", viewer->ren, viewer->win);
    
    int radius = 350;
    Point center = { WIDTH / 2, HEIGTH / 2 };

    const int MAX_PLANES = 3;
    Plane planes[MAX_PLANES];

    strcpy(planes[0].id, "A00000");
    planes[0].x = WIDTH - 150;
    planes[0].y = 150;

    strcpy(planes[1].id, "A00010");
    planes[1].x = 100;
    planes[1].y = 300;

    strcpy(planes[2].id, "A00020");
    planes[2].x = WIDTH-300;
    planes[2].y = HEIGTH-50;

    SDL_Event event;

    while (viewer_should_run(viewer)) {
        while(SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) viewer_stop(viewer);
        }


        viewer_clear(viewer);

        /* ------------------------------------------------------------------------------------- RENDER SECTION */
        // Circulo
        draw_circle(viewer->ren, center, radius);

        // Linea
        draw_moving_line(viewer->ren, center, radius);

        // Id del avion
        Point mouse;
        SDL_GetMouseState(&mouse.x, &mouse.y);

        for (int i = 0; i < MAX_PLANES; i++) {
            SDL_Rect plane_coords = { planes[i].x, planes[i].y, PLANE_SIZE, PLANE_SIZE };
            SDL_RenderCopyEx(viewer->ren, plane_tex, NULL, &plane_coords, planes[i].dir, NULL, SDL_FLIP_NONE);

            if (mouse_inside(mouse, planes[i])) {
                SDL_Texture *text;
                SDL_Rect rect;
                create_text(viewer->ren, planes[i].x, planes[i].y+PLANE_SIZE, planes[i].id, font, &text, &rect);
                SDL_RenderCopy(viewer->ren, text, NULL, &rect);
                SDL_DestroyTexture(text);
            }
        }
        /* ---------------------------------------------------------------------------------------------------- */


        // Mostrar lo que se ha dibujado
        viewer_render(viewer);

        // update
        update_planes(planes);

        // 60 FPS
        SDL_Delay(16);
    }

    // Liberar recursos
    viewer_destroy(viewer);
    destroy_texture(plane_tex);
    quit_img();

    return 0;
}
