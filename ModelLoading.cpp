#pragma once
#include "Scene.h"
#include <raylib/raylib.h>
#include "RenderPipeline.h"

class ModelLoadingScene : public Scene
{
private:
    Camera camera = { 0 };
    Model model;
    Texture2D texture;
    Vector3 position;
    BoundingBox bounds;
    bool selected = false;          // Selected object flag
public:
	ModelLoadingScene()
	{

        // Initialization
        //--------------------------------------------------------------------------------------
        const int screenWidth = GetScreenWidth();
        const int screenHeight = GetScreenHeight();

        // Define the camera to look into our 3d world
        camera.position = Vector3{ 50.0f, 50.0f, 50.0f }; // Camera position
        camera.target   = Vector3{ 0.0f, 10.0f, 0.0f };     // Camera looking at point
        camera.up       = Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
        camera.fovy = 45.0f;                                // Camera field-of-view Y
        camera.projection = CAMERA_PERSPECTIVE;                   // Camera mode type

        model = LoadModel("assets/models/obj/castle.obj");                 // Load model
        texture = LoadTexture("assets/models/obj/castle_diffuse.png"); // Load model texture
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;            // Set map diffuse texture

        position = { 0.0f, 0.0f, 0.0f };                    // Set model position

        bounds = GetMeshBoundingBox(model.meshes[0]);   // Set model bounds

        // NOTE: bounds are calculated from the original size of the model,
        // if model is scaled on drawing, bounds must be also scaled


        DisableCursor();                // Limit cursor to relative movement inside the window

        //SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
	}
	~ModelLoadingScene()
	{
            UnloadTexture(texture);     // Unload texture
            UnloadModel(model);         // Unload model

	}
	virtual void Update(const float& dt){

        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);

        // Load new models/textures on drag&drop
        if (IsFileDropped())
        {
            FilePathList droppedFiles = LoadDroppedFiles();

            if (droppedFiles.count == 1) // Only support one file dropped
            {
                if (IsFileExtension(droppedFiles.paths[0], ".obj") ||
                    IsFileExtension(droppedFiles.paths[0], ".gltf") ||
                    IsFileExtension(droppedFiles.paths[0], ".glb") ||
                    IsFileExtension(droppedFiles.paths[0], ".vox") ||
                    IsFileExtension(droppedFiles.paths[0], ".iqm") ||
                    IsFileExtension(droppedFiles.paths[0], ".m3d"))       // Model file formats supported
                {
                    UnloadModel(model);                         // Unload previous model
                    model = LoadModel(droppedFiles.paths[0]);   // Load new model
                    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture; // Set current map diffuse texture

                    bounds = GetMeshBoundingBox(model.meshes[0]);

                    // TODO: Move camera position from target enough distance to visualize model properly
                }
                else if (IsFileExtension(droppedFiles.paths[0], ".png"))  // Texture file formats supported
                {
                    // Unload current model texture and load new one
                    UnloadTexture(texture);
                    texture = LoadTexture(droppedFiles.paths[0]);
                    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
                }
            }

            UnloadDroppedFiles(droppedFiles);    // Unload filepaths from memory
        }

        // Select model on mouse click
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            // Check collision between ray and box
            if (GetRayCollisionBox(GetMouseRay(GetMousePosition(), camera), bounds).hit) selected = !selected;
            else selected = false;
        }
    
    
    }

	virtual void FixedUpdate(const float& timeStep) {}
	virtual void Draw()
	{

        BeginMode3D(camera);

        DrawModel(model, position, 1.0f, WHITE);        // Draw 3d model with texture

        DrawGrid(20, 10.0f);         // Draw a grid

        if (selected) DrawBoundingBox(bounds, GREEN);   // Draw selection box

        EndMode3D();

        DrawText("Drag & drop model to load mesh/texture.", 10, GetScreenHeight() - 20, 10, DARKGRAY);
        if (selected) DrawText("MODEL SELECTED", GetScreenWidth() - 110, 10, 10, GREEN);

        DrawText("(c) Castle 3D model by Alberto Cano", GetScreenWidth() - 200, GetScreenHeight() - 20, 10, GRAY);

        DrawFPS(10, 10);

	}
	virtual void Debug() {

	};
    virtual void PollEvents() {

        if (IsKeyPressed(KEY_TAB))
        {
            if (IsCursorHidden())
                EnableCursor();
            else
                DisableCursor();
        }


    };


	static Scene* Register() { return new ModelLoadingScene(); }

};






static int scene000 = RegisterScene("Template", "Raylib: Model Loading", ModelLoadingScene::Register);



