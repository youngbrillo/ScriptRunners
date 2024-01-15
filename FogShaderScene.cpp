#include "Scene.h"
#include <raylib/raylib.h>
#include <raylib/raymath.h>
#include <raylib/rlgl.h>

#define PLATFORM_DESKTOP
#define RLIGHTS_IMPLEMENTATION

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

#include "rlights.h"

class FogShaderScene : public  Scene
{
public:
	FogShaderScene();
	~FogShaderScene();
	//virtual void Initialize() override;
	virtual void Update(const float& deltaTime);
	virtual void FixedUpdate(const float& timeStep);
	virtual void Draw();
	virtual void Debug() override;
	virtual void PollEvents() override;
    static Scene* Register() { return new FogShaderScene(); }

public:
    Shader shader;
    Camera camera = { 0 };
    Model modelA, modelB, modelC;
    float fogDensity = 0.15f;
    int fogDensityLoc, ambientLoc;
    Texture texture;
};
#include "TextureManager.h"
FogShaderScene::FogShaderScene()
{

    // Define the camera to look into our 3d world
    camera.position = Vector3{ 2.0f, 2.0f, 6.0f };    // Camera position
    camera.target = Vector3{ 0.0f, 0.5f, 0.0f };      // Camera looking at point
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    // Load models and texture
    modelA = LoadModelFromMesh(GenMeshTorus(0.4f, 1.0f, 16, 32));
    modelB = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
    modelC = LoadModelFromMesh(GenMeshSphere(0.5f, 32, 32));
    texture = TextureManager::Instance()->GetTexture("CHECKER");

    // Assign texture to default model material
    modelA.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    modelB.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    modelC.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    // Load shader and set up some uniforms
    shader = LoadShader(TextFormat("Assets/shaders/glsl%i/lighting.vs", GLSL_VERSION),
        TextFormat("Assets/shaders/glsl%i/fog.fs", GLSL_VERSION));
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    // Ambient light level
    ambientLoc = GetShaderLocation(shader, "ambient");
    float uniVec[4] = { 0.2f, 0.2f, 0.2f, 1.0f };   
    SetShaderValue(shader, ambientLoc, uniVec, SHADER_UNIFORM_VEC4);

    fogDensityLoc = GetShaderLocation(shader, "fogDensity");
    SetShaderValue(shader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);

    // NOTE: All models share the same shader
    modelA.materials[0].shader = shader;
    modelB.materials[0].shader = shader;
    modelC.materials[0].shader = shader;

    // Using just 1 point lights
    

    CreateLight(LIGHT_POINT, Vector3 { 0, 2, 6 }, Vector3Zero(), WHITE, shader);

    //SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

}

FogShaderScene::~FogShaderScene()
{
    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadModel(modelA);        // Unload the model A
    UnloadModel(modelB);        // Unload the model B
    UnloadModel(modelC);        // Unload the model C
    //UnloadTexture(texture);     // Unload the texture
    UnloadShader(shader);       // Unload shader

}

void FogShaderScene::Update(const float& deltaTime)
{

    // Update
    //----------------------------------------------------------------------------------
    UpdateCamera(&camera, CAMERA_ORBITAL);


    SetShaderValue(shader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);

    // Rotate the torus
    modelA.transform = MatrixMultiply(modelA.transform, MatrixRotateX(-1.0f * deltaTime));
    modelA.transform = MatrixMultiply(modelA.transform, MatrixRotateZ(1 * deltaTime));

    // Update the light shader with the camera view position
    SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], &camera.position.x, SHADER_UNIFORM_VEC3);
    //----------------------------------------------------------------------------------

}

void FogShaderScene::FixedUpdate(const float& timeStep)
{
}

void FogShaderScene::Draw()
{
    BeginMode3D(camera);

    // Draw the three models
    DrawModel(modelA, Vector3Zero(), 1.0f, WHITE);
    DrawModel(modelB, Vector3 { -2.6f, 0, 0 }, 1.0f, WHITE);
    DrawModel(modelC, Vector3 { 2.6f, 0, 0 }, 1.0f, WHITE);

    for (int i = -20; i < 20; i += 2) DrawModel(modelA, Vector3 { (float)i, 0, 2 }, 1.0f, WHITE);

    EndMode3D();

    DrawText(TextFormat("Use KEY_UP/KEY_DOWN to change fog density [%.2f]", fogDensity), 10, 30, 20, RAYWHITE);

}

void FogShaderScene::Debug()
{
}

void FogShaderScene::PollEvents()
{

    if (IsKeyDown(KEY_UP))
    {
        fogDensity += 0.001f;
        if (fogDensity > 1.0f) fogDensity = 1.0f;
    }

    if (IsKeyDown(KEY_DOWN))
    {
        fogDensity -= 0.001f;
        if (fogDensity < 0.0f) fogDensity = 0.0f;
    }
}
static int scene000 = RegisterScene("Template", "Raylib: Fog Shader", FogShaderScene::Register);


#define MAX_POSTPRO_SHADERS         12
enum PostproShader {
    FX_GRAYSCALE = 0,
    FX_POSTERIZATION,
    FX_DREAM_VISION,
    FX_PIXELIZER,
    FX_CROSS_HATCHING,
    FX_CROSS_STITCHING,
    FX_PREDATOR_VIEW,
    FX_SCANLINES,
    FX_FISHEYE,
    FX_SOBEL,
    FX_BLOOM,
    FX_BLUR,
    //FX_FXAA
} ;


static const char* postproShaderText[] = {
    "GRAYSCALE",
    "POSTERIZATION",
    "DREAM_VISION",
    "PIXELIZER",
    "CROSS_HATCHING",
    "CROSS_STITCHING",
    "PREDATOR_VIEW",
    "SCANLINES",
    "FISHEYE",
    "SOBEL",
    "BLOOM",
    "BLUR",
    //"FXAA"
};

struct modelLookup
{
    const char* model_path = "assets/models/church.obj";
    const char* texture_path = "assets/models/church_diffuse.png";
};

#define MAX_MODELS_ALLOWED 10

class PostProcessingScene : public  Scene
{
    Camera camera = { 0 };
    Model model;
    Texture2D texture;
    RenderTexture2D target;
    int currentShader;
    Vector3 position;
    Shader shaders[MAX_POSTPRO_SHADERS] = { 0 };
    modelLookup models_list[MAX_MODELS_ALLOWED] = {
        {"assets/models/church.obj", "assets/models/church_diffuse.png"}
        , {"assets/models/barracks.obj", "assets/models/barracks_diffuse.png"}
        , {"assets/models/watermill.obj", "assets/models/watermill_diffuse.png"}
        , {"assets/models/obj/bridge.obj", "assets/models/obj/watermill_diffuse.png"}
        , {"assets/models/obj/castle.obj",  "assets/models/obj/castle_diffuse.png"}
        , {"assets/models/obj/house.obj",   "assets/models/obj/house_diffuse.png"}
        , {"assets/models/obj/market.obj",  "assets/models/obj/market_diffuse.png"}
        , {"assets/models/obj/plane.obj",   "assets/models/obj/plane_diffuse.png"}
        , {"assets/models/obj/turret.obj",  "assets/models/obj/turret_diffuse.png"}
        , {"assets/models/obj/well.obj",    "assets/models/obj/well_diffuse.png"}
    };

    int current_model = 0;
    int targetWidth, targetHeight;
    float pixelWidth = 5.0f, pixelHeight = 5.0f;
public:
    PostProcessingScene()
    {

        // Define the camera to look into our 3d world
        camera.position = Vector3{ 2.0f, 3.0f, 2.0f };    // Camera position
        camera.target = Vector3{ 0.0f, 1.0f, 0.0f };      // Camera looking at point
        camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
        camera.fovy = 45.0f;                                // Camera field-of-view Y
        camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

        model = LoadModel("assets/models/church.obj");                 // Load OBJ model
        texture = LoadTexture("assets/models/church_diffuse.png"); // Load model texture (diffuse map)
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;        // Set model diffuse texture

        position = { 0.0f, 0.0f, 0.0f };            // Set model position

        // Load all postpro shaders
        // NOTE 1: All postpro shader use the base vertex shader (DEFAULT_VERTEX_SHADER)
        // NOTE 2: We load the correct shader depending on GLSL version

        // NOTE: Defining 0 (NULL) for vertex shader forces usage of internal default vertex shader
        shaders[FX_GRAYSCALE] = LoadShader(0, TextFormat("assets/shaders/glsl%i/grayscale.fs", GLSL_VERSION));
        shaders[FX_POSTERIZATION] = LoadShader(0, TextFormat("assets/shaders/glsl%i/posterization.fs", GLSL_VERSION));
        shaders[FX_DREAM_VISION] = LoadShader(0, TextFormat("assets/shaders/glsl%i/dream_vision.fs", GLSL_VERSION));
        shaders[FX_PIXELIZER] = LoadShader(0, TextFormat("assets/shaders/glsl%i/pixelizer.fs", GLSL_VERSION));
        shaders[FX_CROSS_HATCHING] = LoadShader(0, TextFormat("assets/shaders/glsl%i/cross_hatching.fs", GLSL_VERSION));
        shaders[FX_CROSS_STITCHING] = LoadShader(0, TextFormat("assets/shaders/glsl%i/cross_stitching.fs", GLSL_VERSION));
        shaders[FX_PREDATOR_VIEW] = LoadShader(0, TextFormat("assets/shaders/glsl%i/predator.fs", GLSL_VERSION));
        shaders[FX_SCANLINES] = LoadShader(0, TextFormat("assets/shaders/glsl%i/scanlines.fs", GLSL_VERSION));
        shaders[FX_FISHEYE] = LoadShader(0, TextFormat("assets/shaders/glsl%i/fisheye.fs", GLSL_VERSION));
        shaders[FX_SOBEL] = LoadShader(0, TextFormat("assets/shaders/glsl%i/sobel.fs", GLSL_VERSION));
        shaders[FX_BLOOM] = LoadShader(0, TextFormat("assets/shaders/glsl%i/bloom.fs", GLSL_VERSION));
        shaders[FX_BLUR] = LoadShader(0, TextFormat("assets/shaders/glsl%i/blur.fs", GLSL_VERSION));

        currentShader = FX_GRAYSCALE;

        // Create a RenderTexture2D to be used for render to texture
        targetWidth = GetScreenWidth();
        targetHeight = GetScreenHeight();
        target = LoadRenderTexture(targetWidth, targetHeight);

    }
    ~PostProcessingScene()
    {
        for (int i = 0; i < MAX_POSTPRO_SHADERS; i++) UnloadShader(shaders[i]);

        UnloadTexture(texture);         // Unload texture
        UnloadModel(model);             // Unload model
        UnloadRenderTexture(target);    // Unload render texture
    }

    void SetModel(int index)
    {

        UnloadTexture(texture);         // Unload texture
        UnloadModel(model);             // Unload model

        model = LoadModel(models_list[index].model_path);                 // Load OBJ model
        texture = LoadTexture(models_list[index].texture_path); // Load model texture (diffuse map)
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;        // Set model diffuse texture
    }
    //virtual void Initialize() override;
    virtual void Update(const float& deltaTime) {

        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera, CAMERA_ORBITAL);


        //----------------------------------------------------------------------------------

    }
    virtual void FixedUpdate(const float& timeStep)
    {

    }
    virtual void Draw()
    {

        // Draw
        //----------------------------------------------------------------------------------
        BeginTextureMode(target);       // Enable drawing to texture
            ClearBackground(RAYWHITE);  // Clear texture background

            BeginMode3D(camera);        // Begin 3d mode drawing
                DrawModel(model, position, 0.1f, WHITE);   // Draw 3d model with texture
                DrawGrid(10, 1.0f);     // Draw a grid
            EndMode3D();                // End 3d mode drawing, returns to orthographic 2d mode
        EndTextureMode();               // End drawing to texture (now we have a texture available for next passes)



        // Render generated texture using selected postprocessing shader
        BeginShaderMode(shaders[currentShader]);
        // NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
        DrawTextureRec(target.texture, Rectangle { 0, 0, (float)target.texture.width, (float)-target.texture.height }, Vector2 { 0, 0 }, WHITE);

        EndShaderMode();

        // Draw 2d shapes and text over drawn texture
        DrawRectangle(0, 9+20, 580, 30, Fade(LIGHTGRAY, 0.7f));

        DrawText("(c) Church 3D model by Alberto Cano", GetScreenWidth() - 200, GetScreenHeight() - 20, 10, GRAY);
        DrawText("CURRENT POSTPRO SHADER:", 10, 15+20, 20, BLACK);
        DrawText(postproShaderText[currentShader], 330, 15+20, 20, RED);
        DrawText("< >", 540, 10+20, 30, DARKBLUE);
        DrawFPS(700, 15+20);

    }
    virtual void Debug() override
    {
        bool a = ImGui::SliderInt("Target width", &targetWidth, 0, GetScreenWidth());
        bool b =  ImGui::SliderInt("Target height", &targetHeight, 0, GetScreenHeight());
        if (a || b)
        {
            UnloadRenderTexture(target);    // Unload render texture
            target = LoadRenderTexture(targetWidth, targetHeight);
        }

        if(ImGui::SliderInt("current Model", &current_model, 0, MAX_MODELS_ALLOWED - 1))
        { 
            SetModel(current_model);
        }

        if (currentShader == FX_PIXELIZER)
        {
            int pixelWidthLoc = GetShaderLocation(shaders[currentShader], "pixelWidth");
            int pixelHeightLoc = GetShaderLocation(shaders[currentShader], "pixelHeight");

            bool c = ImGui::SliderFloat("pixelWidth", &pixelWidth, 0.5f, 20.0f);
            bool d = ImGui::SliderFloat("pixelHeight", &pixelHeight, 0.5f, 20.0f);

            if (c || d)
            {
                SetShaderValue(shaders[currentShader], pixelWidthLoc, &pixelWidth, SHADER_UNIFORM_FLOAT);
                SetShaderValue(shaders[currentShader], pixelHeightLoc, &pixelHeight, SHADER_UNIFORM_FLOAT);

            }

        }
    }
    virtual void PollEvents() override
    {
        if (IsKeyPressed(KEY_RIGHT)) currentShader++;
        else if (IsKeyPressed(KEY_LEFT)) currentShader--;

        if (currentShader >= MAX_POSTPRO_SHADERS) currentShader = 0;
        else if (currentShader < 0) currentShader = MAX_POSTPRO_SHADERS - 1;


        int curr_model = current_model;
        if (IsKeyPressed(KEY_UP)) curr_model++;
        else if (IsKeyPressed(KEY_DOWN)) curr_model--;

        if (curr_model >= MAX_MODELS_ALLOWED) curr_model = 0;
        else if (curr_model < 0) curr_model = MAX_MODELS_ALLOWED - 1;

        if (curr_model != current_model)
        {
            current_model = curr_model;
            SetModel(current_model);
        }
    }
    static Scene* Register() { return new PostProcessingScene(); }

public:
};

static int scene001 = RegisterScene("Template", "Raylib: Post Processing", PostProcessingScene::Register);
