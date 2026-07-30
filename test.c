#include <stdlib.h>
#include <stdio.h>

typedef struct Object {
    double pos[2];
    double vel[2];
} Object;

Object** objects = NULL;
size_t objects_length = 0;

Object* create(){
    return calloc(1,sizeof(Object));
}

double* getPosFromObject(void* obj){
    return ((Object*)obj)->pos;
}
double* getVelFromObject(void* obj){
    return ((Object*)obj)->vel;
}

void insert(Object* obj) {
    Object** tmp = realloc(objects,
        (objects_length + 1) * sizeof(Object*));

    if (tmp) {
        objects = tmp;
        objects[objects_length++] = obj;
    }
}

void UpdateAll(double dt) {
    for (size_t i = 0; i < objects_length; i++) {
        objects[i]->pos[0] += objects[i]->vel[0];
        objects[i]->pos[1] += objects[i]->vel[1];
    }
}