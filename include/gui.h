#ifndef __GUI__
#define __GUI__

#include "imgui.h"
#include "imfilebrowser.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <array>

#include "renderer.h"
#include "framebuffer.h"
#include "window.h"
#include "camera.h"
#include "types.h"
#include "fractalController.h"


class Gui {
public:
    void Init(Window* window, FractalController* fr);
    void Update();
    void Destroy();

private:
    // light parameters
    float light_direction[3] = { 0.0f, -1.0f, 0.0f };
    float ambient_light_color[3] = { 1.0, 1.0, 1.0 };
    float diffuse_light_color[3] = { 1.0, 1.0, 1.0 };
    float specular_light_color[3] = { 1.0, 1.0, 1.0 };

    // background parameters
    const char* background_types[3] = { "Solid color", "Texture", "HDRTexture" };
    int current_background_type = 2; // If the selection isn't within 0..count, Combo won't display a preview
    float background_color[3] = { 0.30, 0.36, 0.60 };
    const char* skybox_texture[8] = { "Orbital", "Night", "PalmTrees", "CoitTower", "MountainPath", "NightPath", "Vasa", "Other" };
    int current_skybox_texture = 0;
    const char* skybox_texture_hdr[8] = { "WinterForest", "Milkyway", "GrandCanyon", "IceLake", "Factory", "TopangaForest", "TropicalBeach", "OtherHDR" };
    int current_skybox_texture_hdr = 0;
    bool irradiance_cubemap = true;

    // general fractal parameters
    bool hard_shadows = false;
    bool soft_shadows = false;
    bool ambient_occlusion = false;

    // fractal type
    FractalType currentFractalType = FractalType::Test;

    // test fractal parameters
    float ambient_fractal_color[3] = { 0.19225, 0.19225, 0.19225 };
    float diffuse_fractal_color[3] = { 0.50754, 0.50754, 0.50754 };
    float specular_fractal_color[3] = { 0.50827, 0.50827, 0.50827 };
    float shininess = 32.0;
    float reflection = 0.2;

    // mandelbulb fractal parameters
    float light_direction1[3] = { 0.577, 0.577, -0.577 }; // sun. compute shadows for it
    float light_color1[3] = { 1.0, 1.0, 0.70 }; 
    float light_intensity1 = 12.0; 

    float light_direction2[3] = { -0.707, 0.000, 0.707 }; // light. compute occlusion for it
    float light_color2[3] = { 0.25, 0.20, 0.15 };
    float light_intensity2 = 4.0;

    float ambient_fractal_light_color[3] = { 0.35, 0.30, 0.25 }; // ambient light
    float ambient_light_intensity = 2.5;

    float color1[3] = { 0.10, 0.20, 0.30 };
    float color2[3] = { 0.02, 0.10, 0.30 };
    float color3[3] = { 0.30, 0.10, 0.02 };
    int mandelbulb_iterations = 8;
    float mandelbulb_bailout = 10.0f;
    float mandelbulb_power = 9.0f;

    // export
    bool exportWindowFlag = false;
    ExportType currentExportType = ExportType::Png;
    ImGui::FileBrowser fileBrowserSaveImage = ImGui::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CreateNewDir);
    std::string output_path = std::filesystem::current_path().u8string() + "/";
    std::string output_name = "image";
    std::string output_name_obj = "image.obj";
    int output_width = 1920;
    int output_height = 1080;
    int output_quality = 100;

    // setup new HDR skybox
    ImGui::FileBrowser fileBrowserSetupSkyboxHDR = ImGui::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CreateNewDir);
    std::string skybox_hdr_path = "/" + std::filesystem::current_path().u8string() + "/textures/HDR/Milkyway/Milkyway.hdr";
    std::string skybox_hdr_root = "";
    std::string skybox_hdr_name = "";

    // setup new skybox (6 textures)
    CubemapSide cubemapSide;
    ImGui::FileBrowser fileBrowserSetupSkybox = ImGui::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CreateNewDir);
    std::string path = "/" + std::filesystem::current_path().u8string();
    std::array<std::string, 6> skybox_paths = { path + "/textures/error.jpg",
                                                path + "/textures/error.jpg",
                                                path + "/textures/error.jpg",
                                                path + "/textures/error.jpg",
                                                path + "/textures/error.jpg",
                                                path + "/textures/error.jpg"    
    };
    std::array<std::string, 6> skybox_roots;
    std::array<std::string, 6> skybox_names;

    // Other
    ImGuiWindowFlags parametersWindowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse ;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool show_demo_window = true; // потом надо удалить
    bool show_another_window = false; // потом надо удалить

    FractalController* fractalController;
    Window* window;
    FrameBuffer* fbo;
    Camera* camera;

    void MenuBar();
    void Preview();
    void Stats();
    void MainParameters();
    void FractalColor();
    void FractalParameters();
    void ExportAs();
    void FileBrowserExport();
    void FileBrowserSetupSkybox();
    void FileBrowserSetupSkyboxHDR();

    void Test();
    void Mandelbulb();

};

#endif /* End of __GUI__ */