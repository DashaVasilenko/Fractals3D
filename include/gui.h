#ifndef __GUI__
#define __GUI__

#include "imgui.h"
#include "imfilebrowser.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#include "renderer.h"
#include "framebuffer.h"
#include "window.h"
#include "camera.h"
#include "types.h"


class Gui {
public:
    void Init(Window* window, Renderer* renderer);
    void Update();
    void Destroy();

private:
    enum ExportType {
        Png,
        Bmp,
        Jpg,
        Tga,
        //Obj,
    };

    Renderer* renderer;
    Window* window;
    FrameBuffer* fbo;
    Camera* camera;
    bool show_demo_window = true;
    bool show_another_window = false;

    bool main_parameters_window_color = false;
    bool main_parameters_window_color_previous_state = false;
    bool hard_shadows = false;
    bool hard_shadows_previous_state = false;
    bool soft_shadows = false;
    bool soft_shadows_previous_state = false;
    bool ambient_occlusion = false;
    bool ambient_occlusion_previous_state = false;

    ExportType currentExportType = ExportType::Png;
    FractalType currentFractalType = FractalType::Test;

    bool exportWindowFlag = false;
    ImGuiWindowFlags parametersWindowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse ;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGui::FileBrowser fileBrowserSaveImage = ImGui::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CreateNewDir);
    std::string output_path = std::filesystem::current_path().u8string() + "/";
    std::string output_name = "image";
    std::string output_name_obj = "image.obj";

    int output_width = 1920;
    int output_height = 1080;
    int output_quality = 100;

    void MenuBar();
    void Preview();
    void Stats();
    void MainParameters();
    void FractalParameters();

    void Test();
    void Mandelbulb();

    void ExportAs();
    void FileBrowserExport();
};

#endif /* End of __GUI__ */