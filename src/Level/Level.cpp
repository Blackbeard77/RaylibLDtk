//
// Created by mrbau on 22.07.2025.
//

#include "Level.h"

Vector2 Tile::middle() const {
    return {
        pos.x + src.width / 2,
        pos.y + src.height / 2
    };
}

Level::Level(const nlohmann::json &json) {
    // Traverse layers backwards
    for (int i = json["layerInstances"].size() - 1; i >= 0; --i) {
        nlohmann::json layer = json["layerInstances"][i];
        float layerGridSize = (float)layer["__gridSize"];
        for (const auto &tile : layer["autoLayerTiles"]) {
            Tile t;
            t.src = {
                tile["src"][0],
                tile["src"][1],
                layerGridSize,
                layerGridSize
            };
            t.pos = {
                tile["px"][0],
                tile["px"][1]
            };
            tiles.push_back(t);
        }

        for (const auto &tile : layer["gridTiles"]) {
            Tile t;
            t.src = {
                tile["src"][0],
                tile["src"][1],
                layerGridSize,
                layerGridSize
            };
            t.pos = {
                tile["px"][0],
                tile["px"][1]
            };
            tiles.push_back(t);
            // As you can tell, we have practically repeated code here, it's WET
            // For the sake of clarity, we leave as is, however:
            // Try: Make this code DRY
        }
    }
}

void Level::draw(const Texture2D &tileset, const Rectangle &cameraSpace) const {
    for (const auto &tile : tiles) {
        if (CheckCollisionPointRec(tile.middle(), cameraSpace)){
            DrawTextureRec(tileset, tile.src, tile.pos, WHITE);
        }
    }
}
