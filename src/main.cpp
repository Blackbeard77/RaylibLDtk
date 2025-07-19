#include <cstdlib>
#include <algorithm>

#include "raylib.h"
#include "raymath.h"
#include "nlohmann/json.hpp"
#include "config.h"
#include "fstream"

int main() {
    // [RAYLIB INITIALIZATION]
    // Project name, screen size, fullscreen mode etc. can be specified in the config.h.in file
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(Game::ScreenWidth, Game::ScreenHeight, Game::PROJECT_NAME);
    SetTargetFPS(60);

#ifdef GAME_START_FULLSCREEN
    ToggleFullscreen();
#endif

    // [YOUR GAME INITIALISATION]
    Texture2D tileset = LoadTexture("assets/graphics/tilesets/pirates.png"); // Load a Tileset for the LDtk example.
    RenderTexture2D canvas = LoadRenderTexture(Game::ScreenWidth,
                                               Game::ScreenHeight); // Create a render texture to draw on.

    float renderScale{}; // this and the line below are relevant to letterbox-drawing later.
    Rectangle renderRec{};

    Camera2D playerCam = {
            {Game::ScreenWidth / 2, Game::ScreenHeight / 2},
            {},
            0,
            1
    };


    // Player Example
    Vector2 playerPosition = {620, 380}; // Set the initial player position
    const float playerSpeed = 1.0f;
    const float playerRadius = 8.0f;

    // Load the LDtk level file
    std::ifstream filestream("LDtk/MyLDtkProject/Level_0.ldtkl");

    // Create a JSON object to hold the level data
    nlohmann::json level;

    // Load the JSON content from the file
    filestream >> level; // This implementation just keeps the level data in a json object.
    // It's recommended to create a native class for levels later down the line.

    // [MAIN GAME LOOP]
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // [FULL SCREEN LOGIC]
        if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER)) {
            if (IsWindowFullscreen()) {
                ToggleFullscreen();
                SetWindowSize(Game::ScreenWidth, Game::ScreenHeight);
            } else {
                SetWindowSize(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()));
                ToggleFullscreen();
            }
        }

        // [YOUR GAME LOGIC]
        // Updates that are made by frame are coded here
        // This is where YOUR logic code should go

        // [PLAYER MOVEMENT - INPUTS]
        if (IsKeyDown(KEY_W)) playerPosition.y -= playerSpeed;
        if (IsKeyDown(KEY_S)) playerPosition.y += playerSpeed;
        if (IsKeyDown(KEY_A)) playerPosition.x -= playerSpeed;
        if (IsKeyDown(KEY_D)) playerPosition.x += playerSpeed;

        // [CAMERA LOGIC]
        // This here are some basic camera controls to play around with.
        playerCam.target = playerPosition;
        if (IsKeyDown(KEY_R)){
            playerCam.rotation = GetTime() * 10; // Example rotation based on time, you can change this to your needs.
        }else{
            playerCam.rotation = 0; // Reset rotation when R is not pressed
        }
        if (IsKeyDown(KEY_Z)) {
            playerCam.zoom = 2.0f;
        }else {
            playerCam.zoom = 1.0f;
        }

        // [DRAWING]
        BeginDrawing();
        // You can draw on the screen between BeginDrawing() and EndDrawing()
        // For the letterbox we draw the game on the canvas
        BeginTextureMode(canvas);
        { //Within this block is where we draw our app to the canvas this is where YOUR code goes.
            ClearBackground(GetColor(0x306850ff));
            // [CAM DRAWING]
            // Within the Mode2D Block is where we draw with Camera rules.
            // This is for your Game World and Objects.
            BeginMode2D(playerCam);
            {
                // [WORLD DRAWING]
                Rectangle cameraSpace = {
                        playerPosition.x - 400,
                        playerPosition.y - 250,
                        800,
                        500
                }; // This rectangle defines the area of the camera view for primitive culling.
                // It is slightly too small to ensure you can see its edges by default.
                for (int i = level["layerInstances"].size() - 1; i >= 0; --i) {
                    //This iterates through the layers backwards.
                    auto layer = level["layerInstances"][i];

                    for (auto tile: layer["autoLayerTiles"]) {
                        Vector2 dest;
                        dest.x = tile["px"][0];
                        dest.y = tile["px"][1];
                        if (!CheckCollisionPointRec(dest, cameraSpace)) continue; // Skip tiles outside the camera view
                        Rectangle src = {
                                tile["src"][0],
                                tile["src"][1],
                                16,
                                16
                        };
                        DrawTextureRec(tileset, src, dest, WHITE);
                    }

                    for (auto tile: layer["gridTiles"]) {
                        Vector2 dest;
                        dest.x = tile["px"][0];
                        dest.y = tile["px"][1];
                        Rectangle tileSpace = {
                                dest.x,
                                dest.y,
                                16,
                                16
                        }; // This is the rectangle of the tile.
                        if (!CheckCollisionRecs(tileSpace, cameraSpace)) continue;
                        Rectangle src = {
                                tile["src"][0],
                                tile["src"][1],
                                16,
                                16
                        };
                        DrawTextureRec(tileset, src, dest, WHITE);
                    }
                } // You should draw the level separately within its own class or function.

                // [DRAW PLAYER]
                DrawCircleV(playerPosition, playerRadius, RED);
            }
            EndMode2D();

            // [NON-CAM DRAWING]
            // This is probably where you want to draw the HUD
            // As an example, we draw the players position.
            DrawText(TextFormat("X: %.0f \nY: %.0f", playerPosition.x, playerPosition.y), 10, 10, 10, BLACK);
        }
        EndTextureMode(); // End of the canvas drawing block

        //The following lines put the canvas in the middle of the window and have the negative as black
        ClearBackground(BLACK); // If you want something else than a black void in the background,
        // you can add stuff HERE.

        // You likely dont want to touch the following lines unless you really know what you are doing.
        renderScale = std::min(GetScreenHeight() /
                               (float) canvas.texture.height, // Calculates how big or small the canvas has to be rendered.
                               GetScreenWidth() /
                               (float) canvas.texture.width); // Priority is given to the smaller side.
        renderScale = floorf(renderScale);
        if (renderScale < 1) renderScale = 1; // Ensure that scale is at least 1.
        renderRec.width = canvas.texture.width * renderScale;
        renderRec.height = canvas.texture.height * renderScale;
        renderRec.x = (GetScreenWidth() - renderRec.width) / 2.0f;
        renderRec.y = (GetScreenHeight() - renderRec.height) / 2.0f;
        DrawTexturePro(canvas.texture,
                       Rectangle{0, 0, (float) canvas.texture.width, (float) -canvas.texture.height},
                       renderRec,
                       {}, 0, WHITE);
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_S)) {
            DrawText(TextFormat("Render scale: %.0f", renderScale), 10, 10, 20, LIGHTGRAY);
        }
        EndDrawing();
    } // Main game loop end

// De-initialization here
// ...
// ...

// Close window and OpenGL context
    CloseWindow();

    return EXIT_SUCCESS;
}
