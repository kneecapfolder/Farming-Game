#include <iostream> // For debugging
#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include <string>
#include <map>
#include "heightmap.hpp"
#include "tiles.hpp"
#include "tools.hpp"
#define SCREEN_SIZE 700
#define CELL_SIZE 25

using std::map;
using std::string;
using std::to_string;

void moveCamera(Camera2D&);
void zoomCamera(Camera2D&);
Tile **mapToTiles(Image, int);
void drawTile(Tile, Vector2, Texture2D, Color = WHITE);

int main() {
    InitWindow(SCREEN_SIZE, SCREEN_SIZE + 100, "Farming Game");
    SetTargetFPS(60);

    Image heightMap = LoadImage("resources/heightmap.png");
    Texture2D spriteSheet = LoadTexture("resources/spritesheet.png");
    Camera2D camera = { 0 };
    camera.zoom = 1;
    Tile **board = mapToTiles(heightMap, 4);
    Tool tools[] = {
        Tools::hoe
    };
    Tool tool = tools[0];
    bool debug = true;

    const int boardSize = heightMap.width;

    while(!WindowShouldClose()) {
        // Input
        Vector2 worldPos = Vector2Scale(GetScreenToWorld2D(GetMousePosition(), camera), 1.0f/CELL_SIZE);
        Vector2 mousePos = (Vector2){
            floor(worldPos.x),
            floor(worldPos.y)
        };
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Tile *pTile = &board[(int)mousePos.y][(int)mousePos.x];
            switch(tool.index) {
                case 0:
                    if (pTile->type == "dirt") *pTile = Tiles::soil;
                break;
            }
        }
        else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            board[(int)mousePos.y][(int)mousePos.x] = Tiles::dirt;
            
        }

        if (IsKeyPressed(KEY_F3)) debug = !debug;

        // Camera control
        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
            moveCamera(camera);
        zoomCamera(camera);

        // Restrict camera movement
        while(GetScreenToWorld2D((Vector2){0,0}, camera).x < 0) camera.target.x++;
        while(GetScreenToWorld2D((Vector2){0,0}, camera).y < 0) camera.target.y++;
        while(GetScreenToWorld2D((Vector2){SCREEN_SIZE, 0}, camera).x > boardSize*CELL_SIZE) camera.target.x--;
        while(GetScreenToWorld2D((Vector2){0, SCREEN_SIZE}, camera).y > boardSize*CELL_SIZE) camera.target.y--;

        // Draw
        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode2D(camera);
                // Draw board
                for(int y = 0; y < boardSize; y++)
                    for(int x = 0; x < boardSize; x++)
                        drawTile(board[y][x], (Vector2){(float)x, (float)y}, spriteSheet);

                // Draw mouse hover effect
                Tile mouseHover = Tiles::frame;
                drawTile(mouseHover, mousePos, spriteSheet, tool.color);

            EndMode2D();
            // Draw UI
            if (debug)
                DrawText((to_string((int)mousePos.x)+','+to_string((int)mousePos.y)).c_str(), 0, 0, 32, RED);

            for(int i = 0; i < 9; i++) {
                Rectangle source = (Rectangle){
                    i*25.0f, 0,
                    25, 25
                };
                Rectangle dest = (Rectangle){
                    i*100.0f, SCREEN_SIZE,
                    100, 100
                };
                DrawTexturePro(spriteSheet, source, dest, (Vector2){0, 0}, 0, WHITE);
            }
        EndDrawing();
    }

    UnloadImage(heightMap);
    UnloadTexture(spriteSheet);
    for(int i = 0; i < boardSize; i++)
        delete[] board[i];
    delete[] board;

    CloseWindow();
    return 0;
}

// Drag the camera view
void moveCamera(Camera2D &cam) {
    Vector2 delta = GetMouseDelta();
    delta = Vector2Scale(delta, -1.0f/cam.zoom);
    cam.target = Vector2Add(cam.target, delta);
}

// Zoom the screen in and out
void zoomCamera(Camera2D &cam) {
    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        // Get the world point that is under the mouse
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), cam);

        // Set the offset to where the mouse is
        cam.offset = GetMousePosition();

        // Set the target to match, so that the camera maps the world space point 
        // under the cursor to the screen space point under the cursor at any zoom
        cam.target = mouseWorldPos;

        // Zoom increment
        float scaleFactor = 1.0f + (0.25f*fabsf(wheel));
        if (wheel < 0) scaleFactor = 1.0f/scaleFactor;
        cam.zoom = Clamp(cam.zoom*scaleFactor, 0.6f, 64.0f);
    }
}

Tile **mapToTiles(Image img, int range) {
    // Gen a height map of ints from 0 to range
    int **valMap = genHeightMap(img, range);
    Tile **tileMap = new Tile*[img.width];

    // Transform int val to Tile
    for(int y = 0; y < img.width; y++) {
        tileMap[y] = new Tile[img.width];
        for(int x = 0; x < img.width; x++) {
            Tile tile;
            switch(valMap[y][x]) {
                case 0: tile = Tiles::water; break;
                case 1: tile = Tiles::sand; break;
                case 2: tile = Tiles::dirt; break;
                case 3: tile = Tiles::grass; break;
            }
            tileMap[y][x] = tile;
        }
    }
            
    // Remove valMap from memory
    for(int i = 0; i < img.width; i++)
        delete[] valMap[i];
    delete[] valMap;

    return tileMap;
}

void drawTile(Tile tile, Vector2 pos, Texture2D spriteSheet, Color tint) {
    Rectangle source = (Rectangle){
        tile.source.x * 25,
        tile.source.y * 25,
        25, 25
    };
    Rectangle dest = (Rectangle){
        CELL_SIZE * floor(pos.x),
        CELL_SIZE * floor(pos.y),
        CELL_SIZE, CELL_SIZE
    };
    DrawTexturePro(spriteSheet, source, dest, (Vector2){0, 0}, 0, tint);
}