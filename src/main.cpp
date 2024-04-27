#include "raylib.h"
#include <string>
#include <map>
#define CELL_SIZE 50
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 11

using std::string;
using std::to_string;
using std::map;

struct Tile {
    Vector2 pos;
    Vector2 source;
};

Texture2D spriteSheet;
map<string, Vector2> sprites;

void drawTile(Tile, Color);
string vecToStr(Vector2);
float mapf(float, float, float, float, float);

int main() {
    InitWindow(CELL_SIZE*BOARD_WIDTH, CELL_SIZE*BOARD_HEIGHT, "Farming Game");
    SetTargetFPS(60);

    // Load textures
    spriteSheet = LoadTexture("resources/spritesheet.png");
    sprites["dirt"] = (Vector2){0, 1};

    map<string, Tile> background;

    // Create Inventory
    for(float i = 0; i < BOARD_WIDTH; i++)
        background[vecToStr((Vector2){i, 0})] = (Tile){
            (Vector2){i, 0},
            (Vector2){i, 0},
        };

    // Create Ground
    for(float y = 1; y < BOARD_HEIGHT; y++)
        for(float x = 0; x < BOARD_HEIGHT; x++)
            background[vecToStr((Vector2){x, y})] = (Tile){
                (Vector2){x, y},
                sprites["dirt"],
            };
    

    while(!WindowShouldClose())
    {
        // Input
        Vector2 mousePos = (Vector2){
            mapf(GetMousePosition().x, 0, CELL_SIZE*BOARD_WIDTH, 0, BOARD_WIDTH),
            mapf(GetMousePosition().y, 0, CELL_SIZE*BOARD_HEIGHT, 0, BOARD_HEIGHT),
        };

        // Draw
        BeginDrawing();

        ClearBackground(YELLOW);
        for(auto const& [key, val] : background)
            drawTile(val, key == vecToStr(mousePos)? RED : WHITE);

        DrawText(vecToStr(mousePos).c_str(), 0, 0, 20, RED);

        EndDrawing();
    }
    
    // Unload textures from GPU
    UnloadTexture(spriteSheet);

    CloseWindow();
    return 0;
}

void drawTile(Tile tile, Color tint) {
    // 25 - tile size in the spritesheet
    Rectangle source = (Rectangle){
        tile.source.x * 25,
        tile.source.y * 25,
        25,
        25
    };
    Rectangle dest = (Rectangle){
        tile.pos.x * CELL_SIZE,
        tile.pos.y * CELL_SIZE,
        CELL_SIZE,
        CELL_SIZE
    };
    DrawTexturePro(spriteSheet, source, dest, (Vector2){0, 0}, 0, tint);
}

string vecToStr(Vector2 vec) {
    return to_string((int)vec.x) + ',' + to_string((int)vec.y);
}

float mapf(float n, float begin1, float end1, float begin2, float end2) {
    return (n-begin1)/(end1-begin1) * (end2-begin2) + begin2;
}