#include "raylib.h"
#include <string>
#include <algorithm>
#include <list>
#include <map>
#include <set>

#define CELL_SIZE 40
#define SCREEN_WIDTH 15
#define SCREEN_HEIGHT 15

using std::string;
using std::to_string;
using std::list;
using std::map;
using std::set;

enum Tool {hoe, bucket, sand, wheat, berries, cactus, lilyPad, len};

struct Tile {
    Vector2 pos;
    string type;
};

struct Plant {
    Vector2 pos;
    Vector2 source;
    string groundTypes[2];
    int growth = 0;
    int dir = 0;
    double lastUpdate = -1;
};

Texture2D spriteSheet;
map<string, Vector2> sprites;
map<Tool, Plant> plantTypes;

bool updateTime(float, Plant&);
string vecToStr(Vector2);
void drawTile(Tile, Color = WHITE);
void drawSprite(Plant, float = 1, Color = WHITE);
void wetSoil(map<string, Tile>&, map<string, Plant> &);
bool boundsCheck(Vector2);
float mapf(float, float, float, float, float);

int main() {
    InitWindow(CELL_SIZE*SCREEN_WIDTH, CELL_SIZE*(SCREEN_HEIGHT+2), "Farming Game");
    SetTargetFPS(60);

    srand(GetTime());

    // Define sprites
    spriteSheet = LoadTexture("resources/spritesheet.png");
    sprites["dirt"] = (Vector2){0, 1};
    sprites["soil"] = (Vector2){1, 1};
    sprites["wet soil"] = (Vector2){2, 1};
    sprites["water"] = (Vector2){3, 1};
    sprites["frame"] = (Vector2){4, 1};
    sprites["sand"] = (Vector2){5, 1};

    plantTypes[wheat] = (Plant){
        (Vector2){0, 0},
        (Vector2){0, 2},
        {"soil", "wet soil"},
    };
    plantTypes[berries] = (Plant){
        (Vector2){0, 0},
        (Vector2){3, 2},
        {"soil", "wet soil"},
    };
    plantTypes[cactus] = (Plant){
        (Vector2){0, 0},
        (Vector2){6, 2},
        {"sand"},
    };
    plantTypes[lilyPad] = (Plant){
        (Vector2){0, 0},
        (Vector2){9, 2},
        {"water"},
        2,
    };

    map<string, Tile> ground;
    map<string, Plant> plants;
    bool debug = false;
    Tool tool = hoe;


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
        else if (IsKeyPressed(KEY_ONE)) tool = hoe;
        else if (IsKeyPressed(KEY_TWO)) tool = bucket;
        else if (IsKeyPressed(KEY_THREE)) tool = sand;
        else if (IsKeyPressed(KEY_FOUR)) tool = wheat;
        else if (IsKeyPressed(KEY_FIVE)) tool = berries;
        else if (IsKeyPressed(KEY_SIX)) tool = cactus;
        else if (IsKeyPressed(KEY_SEVEN)) tool = lilyPad;

        // Left click
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && boundsCheck(mousePos)) {
            Tile *pTile = &ground[vecToStr(mousePos)];
            switch(tool) {
                case hoe:
                    if (pTile->type == "dirt") pTile->type = "soil";
                break;
                case bucket:
                    pTile->type = "water";
                break;
                case sand:
                    pTile->type = "sand";
                break;
                // Plant seeds
                case wheat:
                case berries:
                case cactus:
                case lilyPad: {
                    Plant plant = plantTypes[tool];
                    plant.pos = pTile->pos;
                    if (tool == lilyPad)
                        if (plants.count(vecToStr(pTile->pos))) plant.dir = plants[vecToStr(pTile->pos)].dir;
                        else plant.dir = rand() % 4;
                    bool canBePlaced = false;
                    for(auto i : plant.groundTypes)
                        if (pTile->type == i) canBePlaced = true;
                    if (canBePlaced) plants[vecToStr(pTile->pos)] = plant;
                } break;
                default: ;
            };

            bool keepPlant = false;
            for(auto i : plants[vecToStr(mousePos)].groundTypes)
                if (i == pTile->type) keepPlant = true;
            if (!keepPlant) plants.erase(vecToStr(mousePos));
            wetSoil(ground, plants);
        }
        
        // Right click
        else if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && boundsCheck(mousePos)) {
            ground[vecToStr(mousePos)].type = "dirt";
            plants.erase(vecToStr(mousePos));
            wetSoil(ground, plants);
        }

        // Update
        for(auto [key, val] : plants)
            if (ground[key].type != "soil")
                if (val.growth < 2 && updateTime(1, plants[key])) plants[key].source.x++;

        BeginDrawing();
        // ClearBackground(BLACK);
        
        // Draw Ground
        for(auto [key, val] : ground)
            drawTile(val);

        // Draw Plants
        for(auto [key, val] : plants)
            drawSprite(val);

        // Draw hotbar
        for(int i = 0; i < len; i++)
            drawSprite((Plant){
                (Vector2){i*2,SCREEN_HEIGHT},
                (Vector2){i,0},
            }, 2, i == tool? GREEN: WHITE);

        // Mouse hover effect
        Color hoverColors[] = {WHITE, BLUE, YELLOW, GREEN, LIME, DARKGREEN, DARKGREEN};
        if (boundsCheck(mousePos)) drawTile((Tile){
            (Vector2){mousePos.x, mousePos.y},
            "frame",
        }, hoverColors[tool]);

        if(debug)
            DrawText(vecToStr(mousePos).c_str(), 0, 0, 20, GREEN);

        EndDrawing();
    }

    UnloadTexture(spriteSheet);

    CloseWindow();    
    return 0;
}

bool updateTime(float t, Plant &plant) {
    double curr = GetTime();
    if (plant.lastUpdate == -1) {
        plant.lastUpdate = curr;
        return false;
    }
    if (curr - plant.lastUpdate >= t) {
        plant.lastUpdate = curr;
        plant.growth++;
        return true;
    }
    return false;
}

string vecToStr(Vector2 vec) {
    return to_string((int)vec.x)+','+to_string((int)vec.y).c_str();
}

void drawTile(Tile tile, Color tint) {
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
    DrawTexturePro(spriteSheet, source, dest, (Vector2){0, 0}, 0, tint);
}

void drawSprite(Plant plant, float scale, Color tint) {
    Rectangle source = {
        plant.source.x*25,
        plant.source.y*25,
        25, 25,
    };
    Rectangle dest = {
        (int)plant.pos.x*CELL_SIZE+CELL_SIZE*.5*scale,
        (int)plant.pos.y*CELL_SIZE+CELL_SIZE*.5*scale,
        CELL_SIZE*scale, CELL_SIZE*scale,
    };
    DrawTexturePro(spriteSheet, source, dest, (Vector2){dest.width/2, dest.height/2}, 90*plant.dir, tint);
}

void wetSoil(map<string, Tile> &ground, map<string, Plant> &plants) {
    set<string> wetSpots;

    for(auto [key, val] : ground) {
        if (val.type == "wet soil") ground[key].type = "soil";
        else if (val.type == "water") {
            if (val.pos.x+1 < SCREEN_WIDTH) wetSpots.insert(vecToStr((Vector2){val.pos.x+1, val.pos.y}));
            if (val.pos.x-1 >= 0) wetSpots.insert(vecToStr((Vector2){val.pos.x-1, val.pos.y}));
            if (val.pos.y+1 < SCREEN_HEIGHT) wetSpots.insert(vecToStr((Vector2){val.pos.x, val.pos.y+1}));
            if (val.pos.y-1 >= 0) wetSpots.insert(vecToStr((Vector2){val.pos.x, val.pos.y-1}));
        }
    }

    for(auto i : wetSpots)
        if (ground[i].type == "soil") ground[i].type = "wet soil";
}

bool boundsCheck(Vector2 mousePos) {
    return !(mousePos.x < 0 || mousePos.x >= SCREEN_WIDTH || mousePos.y < 0 || mousePos.y >= SCREEN_HEIGHT);
}

float mapf(float n, float begin1, float end1, float begin2, float end2) {
    return (n-begin1) / (end1-begin1) * (end2-begin2) + begin2;
}