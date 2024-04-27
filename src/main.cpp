#include "raylib.h"
#include <string>
#include <map>
#define CELL_SIZE 50
#define BOARD_SIZE 10

using std::string;
using std::to_string;
using std::map;

struct Tile {
    Vector2 pos;
    Texture2D sprite;
};

map<string, Texture2D> sprites;

void drawTile(Tile, Color);
string vecToStr(Vector2);
float mapf(float, float, float, float, float);

int main() {
    InitWindow(CELL_SIZE*BOARD_SIZE, CELL_SIZE*BOARD_SIZE, "Farming Game");
    SetTargetFPS(60);

    // Load textures
    sprites["dirt"] = LoadTexture("resources/dirtpxl.png");

    // Create Ground
    map<string, Tile> ground;
    for(float y = 0; y < BOARD_SIZE; y++)
        for(float x = 0; x < BOARD_SIZE; x++)
            ground[vecToStr((Vector2){x, y})] = (Tile){
                (Vector2){x, y},
                sprites["dirt"]
            };
    

    while(!WindowShouldClose())
    {
        // Input
        Vector2 mousePos = (Vector2){
            mapf(GetMousePosition().x, 0, CELL_SIZE*BOARD_SIZE, 0, BOARD_SIZE),
            mapf(GetMousePosition().y, 0, CELL_SIZE*BOARD_SIZE, 0, BOARD_SIZE),
        };

        // Draw
        BeginDrawing();

        ClearBackground(YELLOW);
        for(auto const& [key, val] : ground)
            drawTile(val, key == vecToStr(mousePos)? RED : WHITE);

        DrawText(vecToStr(mousePos).c_str(), 0, 0, 20, RED);

        EndDrawing();
    }
    
    // Unload textures from GPU
    for(auto const& pair : sprites)
        UnloadTexture(pair.second);

    CloseWindow();
    return 0;
}

void drawTile(Tile tile, Color tint) {
    Rectangle source = (Rectangle){0, 0, tile.sprite.width, tile.sprite.height};
    Rectangle dest = (Rectangle){
        tile.pos.x * CELL_SIZE,
        tile.pos.y * CELL_SIZE,
        CELL_SIZE,
        CELL_SIZE
    };
    DrawTexturePro(tile.sprite, source, dest, (Vector2){0, 0}, 0, tint);
}

string vecToStr(Vector2 vec) {
    return to_string((int)vec.x) + ',' + to_string((int)vec.y);
}

float mapf(float n, float begin1, float end1, float begin2, float end2) {
    return (n-begin1)/(end1-begin1) * (end2-begin2) + begin2;
}