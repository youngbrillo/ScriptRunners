#pragma once
#include "Node2d.h"
#include <vector>
#include <glm/glm.hpp>
#include "JsonHandler.h"
#include "Tilemap.h"


namespace ECS
{
    enum tile_mode {
        tile_mode_draw = 0,
        tile_mode_create,
        tile_mode_edit, 
        tile_mode_point
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
        void GeneratePoint();
    public:
        tile_mode mTileMode = tile_mode::tile_mode_draw;

        Vector2 fMousePosition;
        glm::ivec2 iMousePosition;
        bool firstPointChosen;
        Tilemap map;

        tilemap::Tile newTile;
    };
}


