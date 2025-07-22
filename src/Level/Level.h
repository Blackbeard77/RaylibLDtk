//
// Created by mrbau on 22.07.2025.
//

#ifndef RAYLIBSTARTER_LEVEL_H
#define RAYLIBSTARTER_LEVEL_H
#include "raylib.h"
#include <vector>
#include "nlohmann/json.hpp"

struct Tile{
    Rectangle src;
    Vector2 pos;
    Vector2 middle() const;
};

class Level {
    std::vector<Tile> tiles;
public:
    Level(const nlohmann::json& json);
    void draw(const Texture2D& tileset, const Rectangle& cameraSpace) const;
};


#endif //RAYLIBSTARTER_LEVEL_H
