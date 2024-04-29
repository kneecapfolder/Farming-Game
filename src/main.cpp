#include "raylib.h"
#include <string>
#include <list>
#include <map>

#define CELL_SIZE 40
#define SCREEN_WIDTH 15
#define SCREEN_HEIGHT 15

using std::string;
using std::list;
using std::map;
using std::to_string;

enum Tools {hoe, bucket};

struct Tile {
    Vector2 pos;
    string type;
};

Texture2D spriteSheet;
map<string, Vector2> sprites;

string vecToStr(Vector2);
void drawTile(Tile);
float mapf(float, float, float, float, float);

int main() {
    InitWindow(CELL_SIZE*SCREEN_WIDTH, CELL_SIZE*SCREEN_HEIGHT, "Farming Game");
    SetTargetFPS(60);

    // Define sprites
    spriteSheet = LoadTexture("resources/spritesheet.png");
    sprites["dirt"] = (Vector2){0, 1};
    sprites["soil"] = (Vector2){1, 1};
    sprites["wetsoil"] = (Vector2){2, 1};
    sprites["water"] = (Vector2){3, 1};
    sprites["frame"] = (Vector2){4, 1};

    map<string, Tile> ground;
    bool debug = false;

    // Create ground
    for(float y = 0; y < SCREEN_HEIGHT; y++)
        for(float x = 0; x < SCREEN_WIDTH; x++)
            ground[vecToStr((Vector2){x, y})] = (Tile){
                (Vector2){x, y},
                "dirt",
            };

    while(!WindowShouldClose()) {
        // Input
        Vector2 mousePos = {
            mapf(GetMousePosition().x, 0, CELL_SIZE*SCREEN_WIDTH, 0, SCREEN_WIDTH),
            mapf(GetMousePosition().y, 0, CELL_SIZE*SCREEN_HEIGHT, 0, SCREEN_HEIGHT),
        };

        if (IsKeyPressed(KEY_F3)) debug = !debug;
        else (IsKeyPressed(KEY_ONE)) ;

        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) ground[vecToStr(mousePos)].type = "soil";

        BeginDrawing();
        ClearBackground(YELLOW);
        
        // Draw Ground
        for(auto [key, val] : ground)
            drawTile(val);

        // Mouse hover effect
        drawTile((Tile){
            (Vector2){mousePos.x, mousePos.y},
            "frame",
        });

        if(debug)
            DrawText(vecToStr(mousePos).c_str(), 0, 0, 20, BLUE);

        EndDrawing();
    }

    UnloadTexture(spriteSheet);

    CloseWindow();    
    return 0;
}

string vecToStr(Vector2 vec) {
    return to_string((int)vec.x)+','+to_string((int)vec.y).c_str();
}

void drawTile(Tile tile) {
    Rectangle source = {
        sprites[tile.type].x*25,
        sprites[tile.type].y*25,
        25, 25,
    };
    Rectangle dest = {
        (int)tile.pos.x*CELL_SIZE,
        (int)tile.pos.y*CELL_SIZE,
        CELL_SIZE, CELL_SIZE,
    };
    DrawTexturePro(spriteSheet, source, dest, (Vector2){0, 0}, 0, WHITE);
}

float mapf(float n, float begin1, float end1, float begin2, float end2) {
    return (n-begin1) / (end1-begin1) * (end2-begin2) + begin2;
}