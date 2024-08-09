#ifndef PLANE_H_
#define PLANE_H_

#define PLANE_SIZE 32
#define PLANE_SPEED 0.05

typedef struct {
    char id[10];
    float x;
    float y;
    float dir;
} Plane;

#endif