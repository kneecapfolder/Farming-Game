#include <raylib.h>
#include <string>

struct Tile {
    std::string type;    // Type name
    Vector2 source;      // Texture source in spriteSheet
    Vector2 pos;         // World position
};


namespace Tiles {
    Tile dirt = (Tile){
        "dirt",
        (Vector2){0,1}
    };
    Tile soil = (Tile){
        "soil",
        (Vector2){1,1}
    };
    Tile wetSoil = (Tile){
        "wet soil",
        (Vector2){2,1}
    };
    Tile water = (Tile){
        "water",
        (Vector2){3,1}
    };
    Tile frame = (Tile){
        "frame",
        (Vector2){4,1}
    };
    Tile sand = (Tile){
        "sand",
        (Vector2){5,1}
    };
    Tile grass = (Tile){
        "grass",
        (Vector2){6,1}
    };
}