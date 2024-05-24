#pragma once
#include <raylib.h>

struct Tool
{
    int index;
    Color color;
    Vector2 source;
};


namespace Tools
{
    Tool hoe = (Tool){
        0,
        WHITE,
        (Vector2){0, 0}
    };
}