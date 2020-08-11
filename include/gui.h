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
    float light_direction1[2] = { 55.0, 55.0 }; // sun. compute shadows for it
    float light_color1[3] = { 1.0, 1.0, 0.70 }; 
    float light_intensity1 = 3.5; 

    //float light_direction2[3] = { -0.707, 0.000, 0.707 }; // light. compute occlusion for it
    float light_direction2[3] = { 135.0, 90.0 }; // light. compute occlusion for it
    float light_color2[3] = { 0.25, 0.20, 0.15 };
    float light_intensity2 = 4.0;

    float ambient_fractal_light_color[3] = { 0.35, 0.30, 0.25 }; // ambient light
    float ambient_light_intensity = 2.5;

    // background parameters
    const char* background_types[4] = { "Solid color", "Solid with sun", "Texture", "HDRTexture" };
    int current_background_type = 3; // If the selection isn't within 0..count, Combo won't display a preview
    float background_color[3] = { 0.30, 0.36, 0.60 };
    float sun_color[3] = { 0.8, 0.7, 0.5 };
    const char* skybox_texture[8] = { "Orbital", "Night", "PalmTrees", "CoitTower", "MountainPath", "NightPath", "Vasa", "Other" };
    int current_skybox_texture = 0;
    const char* skybox_texture_hdr[8] = { "WinterForest", "Milkyway", "GrandCanyon", "IceLake", "Factory", "TopangaForest", "TropicalBeach", "OtherHDR" };
    int current_skybox_texture_hdr = 0;
    bool irradiance_cubemap = true;

    // fractal type
    FractalType currentFractalType = FractalType::Test;
    const char* fractal_type[7] = { "Test", "Mandelbulb", "Monster", "Julia1", "Julia2", "Julia3", "Julia4" };
    int current_fractal_type = 0; 

    // general fractal parameters
    bool soft_shadows = false;
    float shadow_strength = 32.0;
    float shininess = 32.0;
    float reflection = 0.2;

    // coloring types
    ColoringType currentColoringType = ColoringType::Type1;
    const char* coloring_type[5] = { "Type1", "Type2", "Type3", "Type4", "Type5" };
    int current_coloring_type = 0; 
    float type1_color[3] = { 1.0, 0.8, 0.7 };
    float type2_color1[3] = { 0.10, 0.20, 0.30 };
    float type2_color2[3] = { 0.02, 0.10, 0.30 };
    float type2_color3[3] = { 0.30, 0.10, 0.02 };
    float type3_color[3] = { 1.0, 0.8, 0.7 };
    float type4_color[3] = { 1.0, 0.8, 0.7 };
    float type5_color[3] = { 1.0, 0.8, 0.7 };


    // test fractal parameters

    // mandelbulb fractal parameters
    int mandelbulb_iterations = 8;
    float mandelbulb_bailout = 10.0f;
    float mandelbulb_power = 9.0f;

    // julia1 fractal parameters
    float julia1_offset = 1.0;
    float julia1_smoothness = 4.0;

    // julia2 fractal parameters
    float julia2_offset[3] = { 0.1498, 0.4479, 0.4495 };
    float julia2_w =  0.4491;
    float julia2_smoothness = 4.0;

    // julia3 fractal parameters
    float julia3_offset = 1.0;
    float julia3_smoothness = 9.0;

    // julia4 fractal parameters
    float julia4_offset[3] = { -0.05, -0.052, 0.85 };
    float julia4_w =  -0.151;
    float julia4_smoothness = 8.7;
    

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
    void Julia1();
    void Julia2();
    void Julia3();
    void Julia4();

};

#endif /* End of __GUI__ */