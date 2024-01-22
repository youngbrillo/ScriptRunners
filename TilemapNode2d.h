#pragma once
#include "Node2d.h"
#include <vector>
#include <glm/glm.hpp>
namespace ECS
{
    struct Tile
    {
        glm::ivec2 position = glm::ivec2(0, 0);
        glm::ivec2 size = glm::ivec2(1);
        Rectangle frame = { 0.0f, 0.0f, 16.0f, 16.0f };
        Color tint = RAYWHITE;
        int layer = 0;
    };
    struct Grid
    {
        int cell_width = 1;
        int cell_height = 1;
    };
    enum tile_mode {
        tile_mode_draw = 0,
        tile_mode_create,
        tile_mode_edit
    };

    class TilemapNode2d : public ECS::Node2d
    {
    public:
        TilemapNode2d(const char* name);
        ~TilemapNode2d();
        
        void Import(const char* texture_alias, const char* dataFile_path);
        void LoadData(const char* dataFile_path);
        void saveData();


        virtual void Update(const float& deltaTime);
        virtual void FixedUpdate(const float& timestep);
        virtual void Draw();
        virtual void UIDraw();
        virtual void Poll();
        virtual void inspect();
        static void Extend(lua_State* L);
    protected:
        void GenerateFixtures();

    public:
        tile_mode mTileMode = tile_mode::tile_mode_draw;
        std::vector<ECS::Tile> mTiles;
        ECS::Tile newTile;
        ECS::Grid grid;

        Vector2 fMousePosition;
        glm::ivec2 iMousePosition;
    };
}


