#include <iostream>
#include <raylib.h>
#define GRAY_VALUE(c) ((float)(c.r + c.g + c.b)/3.0f)

float mapf(float n, float begin1, float end1, float begin2, float end2) {
    return (n-begin1) / (end1-begin1) * (end2-begin2) + begin2;
}

int **genHeightMap(Image heightmap, int range) {
    Color *pixels = LoadImageColors(heightmap);
    int **valuemap = new int*[heightmap.height];
    for(int y = 0; y < heightmap.height; y++) {
        valuemap[y] = new int[heightmap.width];
        for(int x = 0; x < heightmap.width; x++) {
            int gray = GRAY_VALUE(pixels[x+y*50]);
            valuemap[y][x] = gray == 0? 0 : mapf(
                gray,
                0, 150, 1, 4
            );
        }
    }
    UnloadImageColors(pixels);
    return valuemap;
}

