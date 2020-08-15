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
    float background_brightness = 1.0f;
    const char* background_types[4] = { "Solid color", "Solid with sun", "Texture", "HDRTexture" };
    int current_background_type = 3; // If the selection isn't within 0..count, Combo won't display a preview
    float background_color[3] = { 0.30, 0.36, 0.60 };
    float room_background[3] = { 0.0, 0.0, 0.0 };
    float sun_color[3] = { 0.8, 0.7, 0.5 };
    const char* skybox_texture[8] = { "Orbital", "Night", "PalmTrees", "CoitTower", "MountainPath", "NightPath", "Vasa", "Other" };
    int current_skybox_texture = 0;
    const char* skybox_texture_hdr[8] = { "WinterForest", "Milkyway", "GrandCanyon", "IceLake", "Factory", "TopangaForest", "TropicalBeach", "OtherHDR" };
    int current_skybox_texture_hdr = 0;
    bool irradiance_cubemap = true;

    // fractal type
    FractalType currentFractalType = FractalType::Test;
    const char* fractal_type[15] = { "Test", "Mandelbulb", "Juliabulb1", "Monster", "Julia1", "Julia2", "Julia3", "Julia4", "Sierpinski1",
                                     "Sierpinski2", "MengerSponge1", "MengerSponge2", "Apollonian1", "Apollonian2", "Apollonian3" };
    int current_fractal_type = 0; 

    // general fractal parameters
    bool soft_shadows = false;
    float shadow_strength = 32.0;
    float shininess = 32.0;
    float reflection = 0.2;

    // coloring types
    ColoringType currentColoringType = ColoringType::Type1;
    const char* coloring_type[7] = { "Type1", "Type2", "Type3", "Type4", "Type5", "Type6", "Type7" };
    int current_coloring_type = 0; 
    float type1_color[3] = { 1.0, 0.8, 0.7 };
    float type2_color1[3] = { 0.10, 0.20, 0.30 };
    float type2_color2[3] = { 0.02, 0.10, 0.30 };
    float type2_color3[3] = { 0.30, 0.10, 0.02 };
    float type3_color[3] = { 1.0, 0.8, 0.7 };
    float type3_coef = 2.0;
    float type4_color[3] = { 1.0, 0.8, 0.7 };
    float type5_color1[3] = { 1.0, 0.0, 0.0 };
    float type5_color2[3] = { 0.0, 1.0, 0.0 };
    float type5_color3[3] = { 0.0, 0.0, 1.0 };
    float type6_color[3] = { 1.0, 0.8, 0.7 };
    float type7_color1[3] = { 1.0,  1.0, 1.0 };
    float type7_color2[3] = { 1.0, 0.80, 0.2 };
    float type7_color3[3] = { 1.0, 0.55, 0.0 };

    // test fractal parameters

    // mandelbulb fractal parameters
    int mandelbulb_iterations = 8;
    float mandelbulb_bailout = 10.0f;
    float mandelbulb_power = 9.0f;

    // Juliabulb1 fractal parameters
    float juliabulb1_offset = 1.6;
    float juliabulb1_smoothness = 8.0;
    int juliabulb1_iterations = 4;

    // julia1 fractal parameters
    float julia1_offset = 1.0;
    float julia1_smoothness = 4.0;
    int julia1_iterations = 11;


    // julia2 fractal parameters
    float julia2_offset[3] = { 0.1498, 0.4479, 0.4495 };
    float julia2_w =  0.4491;
    float julia2_smoothness = 4.0;
    int julia2_iterations = 11;

    // julia3 fractal parameters
    float julia3_offset = 1.0;
    float julia3_smoothness = 9.0;
    int julia3_iterations = 11;

    // julia4 fractal parameters
    float julia4_offset[3] = { -0.05, -0.052, 0.85 };
    float julia4_w =  -0.151;
    float julia4_smoothness = 8.7;
    int julia4_iterations = 11;
    
    // sierpinski1 fractal parameters
    float sierpinski1_va[3] = { 0.0, 0.57735,   0.0 };
    float sierpinski1_vb[3] = { 0.0, -1.0,  1.15470 };
    float sierpinski1_vc[3] = { 1.0, -1.0, -0.57735 };
    float sierpinski1_vd[3] = {-1.0, -1.0, -0.57735 };
    int sierpinski1_iterations = 8;

    // Sierpinski2 fractal parameters
    float sierpinski2_va[3] = {  -1.0, 0.618, 1.618 };
    float sierpinski2_vb[3] = { 0.618, 1.618,  -1.0 };
    float sierpinski2_vc[3] = { 1.618,  -1.0, 0.618 };
    int sierpinski2_iterations = 6;

    // MengerSponge1 fractal parameters
    float menger_sponge1_offset1 = 1.0;
    float menger_sponge1_offset2 = 1.0;
    int menger_sponge1_iterations = 4;

    // MengerSponge2 fractal parameters
    float menger_sponge2_offset1 = 1.0;
    float menger_sponge2_offset2 = 1.0;
    int menger_sponge2_iterations = 5;

    // Apollonian1 fractal parameters
    float apollonian1_offset1 = 12.0;
    float apollonian1_offset2 = 1.1;
    int apollonian1_iterations = 8;

    // Apollonian2 fractal parameters
    float apollonian2_offset1 = 12.0;
    float apollonian2_offset2 = 1.1;
    int apollonian2_iterations = 6;

    // Apollonian3 fractal parameters
    float apollonian3_offset1 = 20.0;
    float apollonian3_offset2 = 1.1;
    float apollonian3_offset3 = 1.0;
    float apollonian3_csize[3] = { 0.808, 0.8, 1.137 };
    int apollonian3_iterations = 9;
    
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
    ImGuiWindowFlags fractalParametersWindowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool show_demo_window = true; // потом надо удалить
    bool show_another_window = false; // потом надо удалить
    CameraType currentCameraType = CameraType::SphericalCamera;

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
    void Juliabulb1();
    void Julia1();
    void Julia2();
    void Julia3();
    void Julia4();
    void Sierpinski1();
    void Sierpinski2();
    void MengerSponge1();
    void MengerSponge2();
    void Apollonian1();
    void Apollonian2();
    void Apollonian3();

};

#endif /* End of __GUI__ */