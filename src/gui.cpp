#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "json.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <vector>

#include "gui.h"
#include "inputSystem.h"

bool MyDragInt(const char *label, int *v, float v_speed = (1.0F), int v_min = 0, int v_max = 0) {
    float v_backup = *v;
    if (!ImGui::DragInt(label, v, v_speed, v_min, v_max))
        return false;
    *v = glm::clamp(*v, v_min, v_max);
    return v_backup != *v;
}

bool MyDragFloat(const char *label, float *v, float v_speed = (1.0F), float v_min = 0, float v_max = 0) {
    float v_backup = *v;
    if (!ImGui::DragFloat(label, v, v_speed, v_min, v_max))
        return false;
    *v = glm::clamp(*v, v_min, v_max);
    return v_backup != *v;
}

bool MyDragFloat2(const char *label, float *v, float v_speed = (1.0F), float v_min = 0, float v_max = 0) {
    glm::vec2 v_backup = glm::vec2(v[0], v[1]);
    if (!ImGui::DragFloat2(label, v, v_speed, v_min, v_max))
        return false;
    v[0] = glm::clamp(v[0], v_min, v_max);
    v[1] = glm::clamp(v[1], v_min, v_max);
    return v_backup != glm::vec2(v[0], v[1]);
}

bool MyDragFloat3(const char *label, float *v, float v_speed = (1.0F), float v_min = 0, float v_max = 0) {
    glm::vec3 v_backup = glm::vec3(v[0], v[1], v[2]);
    if (!ImGui::DragFloat3(label, v, v_speed, v_min, v_max))
        return false;
    v[0] = glm::clamp(v[0], v_min, v_max);
    v[1] = glm::clamp(v[1], v_min, v_max);
    v[2] = glm::clamp(v[2], v_min, v_max);
    return v_backup != glm::vec3(v[0], v[1], v[2]);
}

void Gui::FileBrowserExport() {
    fileBrowserSaveImage.Display();
    if(fileBrowserSaveImage.HasSelected()) {
        output_path = fileBrowserSaveImage.GetSelected().parent_path().u8string() + "/";
        output_name = fileBrowserSaveImage.GetSelected().filename().u8string();
        int i = output_name.length();
        bool flag = true;

        switch(currentExportType) {
            case ExportType::Png: {
                if (i < 4) flag = false;
                if (flag && output_name.compare(output_name.length() - 4, 4, ".png") != 0)
                    flag = false;
                if (!flag)
                    output_name = output_name + ".png";                
                break;
            }
            case ExportType::Bmp: {
                if (i < 4) flag = false;
                if (flag && output_name.compare(output_name.length() - 4, 4, ".bmp") != 0)
                    flag = false;
                if (!flag)
                    output_name = output_name + ".bmp";
                break;
            }
            case ExportType::Jpg: {
                if (i < 4) flag = false;
                if (flag && output_name.compare(output_name.length() - 4, 4, ".jpg") != 0)
                    flag = false;
                if (!flag)
                    output_name = output_name + ".jpg";
                break;
            }
            case ExportType::Tga: {
                if (i < 4) flag = false;
                if (flag && output_name.compare(output_name.length() - 4, 4, ".tga") != 0)
                    flag = false;
                if (!flag)
                    output_name = output_name + ".tga";
                break;
            }
        }

        fileBrowserSaveImage.ClearSelected();
        fileBrowserSaveImage.Close();
    }
}

void Gui::FileBrowserSaveParameters() {
    fileBrowserSaveParameters.Display();
    if (fileBrowserSaveParameters.HasSelected()) {
        save_path = fileBrowserSaveParameters.GetSelected().parent_path().u8string() + "/";
        save_name = fileBrowserSaveParameters.GetSelected().filename().u8string();
        int i = save_name.length();
        bool flag = true;

        if (i < 5) flag = false;
        if (flag && save_name.compare(save_name.length() - 5, 5, ".json") != 0)
            flag = false;
        if (!flag)
            save_name = save_name + ".json";                    

        fileBrowserSaveParameters.ClearSelected();
        fileBrowserSaveParameters.Close();
    }
}

void Gui::FileBrowserLoadParameters() {
    fileBrowserLoadParameters.Display();
    if(fileBrowserLoadParameters.HasSelected()) {
        load_path = fileBrowserLoadParameters.GetSelected().parent_path().u8string() + "/";
        load_name = fileBrowserLoadParameters.GetSelected().filename().u8string();

        fileBrowserLoadParameters.ClearSelected();
        fileBrowserLoadParameters.Close();
    }
}

void Gui::FileBrowserSetupSkyboxHDR() {
    fileBrowserSetupSkyboxHDR.Display();
    if(fileBrowserSetupSkyboxHDR.HasSelected()) {
        skybox_hdr_path = "/" + fileBrowserSetupSkyboxHDR.GetSelected().relative_path().u8string();
        skybox_hdr_root = fileBrowserSetupSkyboxHDR.GetSelected().root_path().u8string();
        skybox_hdr_name = fileBrowserSetupSkyboxHDR.GetSelected().filename().u8string();

        fileBrowserSetupSkyboxHDR.ClearSelected();
        fileBrowserSetupSkyboxHDR.Close();
    }
}

void Gui::FileBrowserSetupSkybox() {
    fileBrowserSetupSkybox.Display();
    if(fileBrowserSetupSkybox.HasSelected()) {
        skybox_paths[static_cast<int>(cubemapSide)] = "/" + fileBrowserSetupSkybox.GetSelected().relative_path().u8string();
        skybox_roots[static_cast<int>(cubemapSide)] = fileBrowserSetupSkybox.GetSelected().root_path().u8string();
        skybox_names[static_cast<int>(cubemapSide)] = fileBrowserSetupSkybox.GetSelected().filename().u8string();

        fileBrowserSetupSkybox.ClearSelected();
        fileBrowserSetupSkybox.Close();
    }
}

void Gui::Init(Window* window, FractalController* fr) {
    this->window = window;
    this->fractalController = fr;
    this->fbo = fractalController->GetFBO();
    this->camera = fractalController->GetCamera();

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

	// Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	const char* glsl_version =  "#version 330";
    ImGui_ImplGlfw_InitForOpenGL(window->GetPointer(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);    

    fileBrowserSaveImage.SetTitle("Save image as..");
    fileBrowserSetupSkyboxHDR.SetTitle("Setup HDR skybox");
    fileBrowserSaveParameters.SetTitle("Save parameters");
    fileBrowserLoadParameters.SetTitle("Load parameters");
    //std::vector<const char*> image_filter = { ".png", ".jpg", ".jpeg", ".bmp", ".tga", ".hdr", ".obj" };
    //fileBrowserSaveImage.SetTypeFilters(image_filter);
}

void Gui::MenuBar() {
    if(ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            //if (ImGui::MenuItem("Open", "Ctrl+O")) {
            //if (ImGui::MenuItem("Open", "Ctrl + O") || Input::GetKey(KeyCode::LeftCtrl)) {
                //m_FileExplorerLoadConfig.Open();
            //}
            //ImGui::Separator();
            //if (ImGui::MenuItem("New", "Ctrl+N")) {
            //}
            ImGui::Separator();
            if (ImGui::MenuItem("Save Parameters", "Исправить Ctrl+P")) {
                saveWindowFlag = true;
            }
            if (ImGui::MenuItem("Load Parameters", "Исправить Ctrl+Shift+P")) {
                loadWindowFlag = true;
            }
            ImGui::Separator();
            
            //if (ImGui::MenuItem("Save", "Ctrl+S")) {
                //m_FileExplorerSaveConfig.Open();
            //}
            //if (ImGui::MenuItem("Save as..")) {
                //m_FileExplorerSaveConfig.Open();
            //}
            ImGui::Separator();
            
            if (ImGui::BeginMenu("Export..")) {
                if (ImGui::MenuItem("PNG")) {
                    currentExportType = ExportType::Png;
                    exportWindowFlag = true;
                    output_name = "image.png";
                }
                if (ImGui::MenuItem("BMP")){
                    currentExportType = ExportType::Bmp;
                    exportWindowFlag = true;
                    output_name = "image.bmp";
                }
                if (ImGui::MenuItem("JPG")){
                    currentExportType = ExportType::Jpg;
                    exportWindowFlag = true;
                    output_name = "image.jpg";
                }
                if (ImGui::MenuItem("TGA")){
                    currentExportType = ExportType::Tga;
                    exportWindowFlag = true;
                    output_name = "image.tga";
                }
                if (ImGui::MenuItem("OBJ")){
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Esc")) {
                window->Close();
            }
            
            ImGui::EndMenu();
        }
/*
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
                
            }
            
            if (ImGui::MenuItem("Redo", "Ctrl+Shift+Z")) {
                
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Copy", "Ctrl+C")) {
                
            }
            if (ImGui::MenuItem("Paste", "Ctrl+V")) {
                
            }
            ImGui::Separator();
            ImGui::EndMenu();
        }
*/
        if (ImGui::BeginMenu("About")) {
            ImGui::Text("Fractals");
            ImGui::Separator();
            ImGui::Text("Was created as a part of PV097 Visual creativity informatics course");
            ImGui::Text("By Daria Vasilenko");
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

void Gui::Preview() {
    ImGui::SetNextWindowPos(ImVec2(0.0f, 18.0f));
    int width, height;
    window->GetSize(&width, &height);
    ImVec2 previewSize = ImVec2((float)width*2/3-20, (float)(height - 168));
	ImGui::SetNextWindowSize(previewSize);
    ImGuiWindowFlags previewWindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollWithMouse ;
    ImGui::Begin("Test", NULL, previewWindowFlags);
    if (ImGui::IsWindowHovered()) {
        camera->Update();
    }
    ImGui::Image((ImTextureID)fbo->GetTexDescriptor(), ImVec2(previewSize.x, previewSize.y), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
	ImGui::End();
}

void Gui::Stats() {
    int width, height;
    window->GetSize(&width, &height);
    ImGui::SetNextWindowPos(ImVec2(0.0f, (float)(height - 150)));
    ImVec2 parametersSize = ImVec2((float)width*2/3-20, 150.0f);
	ImGui::SetNextWindowSize(parametersSize);

    ImGui::Begin("Stats", NULL, parametersWindowFlags);                         

    ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Separator();

    switch(currentFractalType) {
        case FractalType::Test: {
            ImGui::Text("Current fractal type: Test");
            break;
        }
        case FractalType::Mandelbulb: {
            ImGui::Text("Current fractal type: Mandelbulb fractal");
            break;
        }
        case FractalType::Juliabulb1: {
            ImGui::Text("Current fractal type: Juliabulb1 fractal");
            break;
        }
        case FractalType::Julia1: {
            ImGui::Text("Current fractal type: Julia1 fractal");
            break;
        }
        case FractalType::Julia2: {
            ImGui::Text("Current fractal type: Julia2 fractal");
            break;
        }
        case FractalType::Julia3: {
            ImGui::Text("Current fractal type: Julia3 fractal");
            break;
        }
        case FractalType::Julia4: {
            ImGui::Text("Current fractal type: Julia4 fractal");
            break;
        }
        case FractalType::Sierpinski1: {
            ImGui::Text("Current fractal type: Sierpinski1 fractal");
            break;
        }
        case FractalType::Sierpinski2: {
            ImGui::Text("Current fractal type: Sierpinski2 fractal");
            break;
        }
        case FractalType::MengerSponge1: {
            ImGui::Text("Current fractal type: MengerSponge1 fractal");
            break;
        }
        case FractalType::MengerSponge2: {
            ImGui::Text("Current fractal type: MengerSponge2 fractal");
            break;
        }
        case FractalType::MengerSponge3: {
            ImGui::Text("Current fractal type: MengerSponge3 fractal");
            break;
        }
        case FractalType::Apollonian1: {
            ImGui::Text("Current fractal type: Apollonian1 fractal");
            break;
        }
        case FractalType::Apollonian2: {
            ImGui::Text("Current fractal type: Apollonian2 fractal");
            break;
        }
        case FractalType::Apollonian3: {
            ImGui::Text("Current fractal type: Apollonian3 fractal");
            break;
        }
    }

    ImGui::End();
}

void Gui::MainParameters() {
    int width, height;
    window->GetSize(&width, &height);
    ImGui::SetNextWindowPos(ImVec2((float)width*2/3-20, 18.0f));
    ImVec2 parametersSize = ImVec2((float)width*1/3+20, (float)(height*2/3 + 25.0));
	ImGui::SetNextWindowSize(parametersSize);
    
    ImGui::Begin("Parameters", NULL, fractalParametersWindowFlags); 
    //ShaderProgram* program = fractalController->GetShaderProgram(); 
    bool flag = false;

    //-----------------------------Shadows-------------------------------
    if (currentFractalType != FractalType::Apollonian1 && currentFractalType != FractalType::Apollonian2 &&
        currentFractalType != FractalType::Apollonian3 && currentFractalType != FractalType::MengerSponge3) {
        if (ImGui::Checkbox("Soft shadows", &soft_shadows)) { 
            flag = true; 
            //program->SetShaderParameters(soft_shadows);
            fractalController->SetSoftShadows(soft_shadows);
        }
        if (MyDragFloat("Shadow strength", &shadow_strength, 1, 1, 128)) {
            fractalController->SetShadowStrength(shadow_strength);
        }
    }
    //-------------------------------------------------------------------
    if (ImGui::Checkbox("Tone mapping", &tone_mapping)) { 
        flag = true; 
        fractalController->SetToneMapping(tone_mapping);
    }
    if (tone_mapping) {
        if (MyDragFloat3("Exposure", exposure, 0.01, 0, 1)) {
            fractalController->SetExposure(glm::vec3(exposure[0], exposure[1], exposure[2]));
        }
    }


    //--------------------------Light parameters-------------------------
    ImGui::Separator();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + parametersSize[0]/2.0 - 57.0);
    ImGui::Text("Light parameters");
    ImGui::Separator();

    ImGui::Text("Light 1:");
    if (MyDragFloat2("Direction1", light_direction1, 1.0, 0, 360)) {
        float x = sin(glm::radians(light_direction1[1])) * cos(glm::radians(light_direction1[0]));
        float y = cos(glm::radians(light_direction1[1]));
        float z = sin(glm::radians(light_direction1[0])) * sin(glm::radians(light_direction1[1]));
        fractalController->SetLightDirection1(glm::vec3(x, y, z));
    }
    if (ImGui::ColorEdit3("Color1", light_color1)) {
        fractalController->SetLightColor1(glm::vec3(light_color1[0], light_color1[1], light_color1[2]));
    }
    if (MyDragFloat("Intensity1", &light_intensity1, 0.1, 0, 30)) {
        fractalController->SetLightIntensity1(light_intensity1);
    }
    ImGui::Separator();

    ImGui::Text("Light 2:");
    if (MyDragFloat2("Direction2", light_direction2, 1.0, 0, 360)) {
        float x = sin(glm::radians(light_direction2[1])) * cos(glm::radians(light_direction2[0]));
        float y = cos(glm::radians(light_direction2[1]));
        float z = sin(glm::radians(light_direction2[0])) * sin(glm::radians(light_direction2[1]));
        fractalController->SetLightDirection2(glm::vec3(x, y, z));
    }
    if (ImGui::ColorEdit3("Color2", light_color2)) {
        fractalController->SetLightColor2(glm::vec3(light_color2[0], light_color2[1], light_color2[2]));
    }
    if (MyDragFloat("Intensity2", &light_intensity2, 0.1, 0, 30)) {
        fractalController->SetLightIntensity2(light_intensity2);
    }
    ImGui::Separator();

    ImGui::Text("Ambient light:");
    if (ImGui::ColorEdit3("Color3", ambient_fractal_light_color)) {
        fractalController->SetAmbientFractalLightColor(glm::vec3(ambient_fractal_light_color[0], ambient_fractal_light_color[1], ambient_fractal_light_color[2]));
    }
    if (MyDragFloat("Intensity3", &ambient_light_intensity, 0.1, 0, 30)) {
        fractalController->SetAmbientFractalLightIntensity(ambient_light_intensity);
    }
    //-------------------------------------------------------------------

    //---------------------Background parameters-------------------------
    if (currentFractalType == FractalType::Apollonian1 || currentFractalType == FractalType::Apollonian2 ||
        currentFractalType == FractalType::Apollonian3 || currentFractalType == FractalType::MengerSponge3) {

        ImGui::NewLine();
        ImGui::Separator();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + parametersSize[0]/2.0 - 80.0);
        ImGui::Text("Background parameters");
        ImGui::Separator();
        if (ImGui::ColorEdit3("Background", room_background)) {
            fractalController->SetRoomBackgroundColor(glm::vec3(room_background[0], room_background[1], room_background[2]));
        }
    }
    else {
        ImGui::NewLine();
        ImGui::Separator();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + parametersSize[0]/2.0 - 80.0);
        ImGui::Text("Background parameters");
        ImGui::Separator();
        if (ImGui::Combo("Type", &current_background_type, background_types, IM_ARRAYSIZE(background_types))) {
            flag = true;
            fractalController->SetBackgroundType(static_cast<BackgroundType>(current_background_type));
        }

        // Solid background
        if (current_background_type == 0) {
            if (ImGui::ColorEdit3("Solid color", background_color)) {
                fractalController->SetBackgroundColor(glm::vec3(background_color[0], background_color[1], background_color[2]));
            }
        }

        // Solid background with sun
        if (current_background_type == 1) {
            if (ImGui::ColorEdit3("Solid color", background_color)) {
                fractalController->SetBackgroundColor(glm::vec3(background_color[0], background_color[1], background_color[2]));
            }
            if (ImGui::ColorEdit3("Sun color", sun_color)) {
                fractalController->SetSunColor(glm::vec3(sun_color[0], sun_color[1], sun_color[2]));
            }
        }

        // Texture background
        if (current_background_type == 2) {
            if (MyDragFloat("Brightness", &background_brightness, 0.1, 0, 30)) {
                fractalController->SetBackgroundBrightness(background_brightness);
            }
            if (ImGui::Combo("Texture", &current_skybox_texture, skybox_texture, IM_ARRAYSIZE(skybox_texture))) {
                if (static_cast<SkyboxTexture>(current_skybox_texture) != SkyboxTexture::Other)
                    fractalController->SetSkyboxTexture(static_cast<SkyboxTexture>(current_skybox_texture));
            }
            if (static_cast<SkyboxTexture>(current_skybox_texture) == SkyboxTexture::Other) {
                std::string text = "";
                if (ImGui::Button("Front(+Z):")) {
                    cubemapSide = CubemapSide::Front;
                    fileBrowserSetupSkybox.Open();
                }
                ImGui::SameLine();
                text = skybox_roots[0] + "..." + skybox_names[0];
                ImGui::Text("%s", text.c_str());

                if (ImGui::Button("Back(-Z):")) {
                    cubemapSide = CubemapSide::Back;
                    fileBrowserSetupSkybox.Open();
                }
                ImGui::SameLine();
                text = skybox_roots[1] + "..." + skybox_names[1];
                ImGui::Text("%s", text.c_str());
            
                if (ImGui::Button("Up(+Y):")) {
                    cubemapSide = CubemapSide::Up;
                    fileBrowserSetupSkybox.Open();
                }
                ImGui::SameLine();
                text = skybox_roots[2] + "..." + skybox_names[2];
                ImGui::Text("%s", text.c_str());

                if (ImGui::Button("Down(-Y):")) {
                    cubemapSide = CubemapSide::Down;
                    fileBrowserSetupSkybox.Open();
                }
                ImGui::SameLine();
                text = skybox_roots[3] + "..." + skybox_names[3];
                ImGui::Text("%s", text.c_str());

                if (ImGui::Button("Left(-X):")) {
                    cubemapSide = CubemapSide::Left;
                    fileBrowserSetupSkybox.Open();
                }
                ImGui::SameLine();
                text = skybox_roots[4] + "..." + skybox_names[4];
                ImGui::Text("%s", text.c_str());

                if (ImGui::Button("Right(+X):")) {
                    cubemapSide = CubemapSide::Right;
                    fileBrowserSetupSkybox.Open();
                }
                ImGui::SameLine();
                text = skybox_roots[5] + "..." + skybox_names[5];
                ImGui::Text("%s", text.c_str());

                if (ImGui::Button("Use")) {
                    fractalController->SetSkyboxTexture(skybox_paths);
                }   
            }  
        }
    
        // HDR Texture background
        if (current_background_type == 3) {
            if (MyDragFloat("Brightness", &background_brightness, 0.1, 0, 30)) {
                fractalController->SetBackgroundBrightness(background_brightness);
            }
            if (ImGui::Combo("HDR Texture", &current_skybox_texture_hdr, skybox_texture_hdr, IM_ARRAYSIZE(skybox_texture_hdr))) {
                if (static_cast<SkyboxTextureHDR>(current_skybox_texture_hdr) != SkyboxTextureHDR::OtherHDR)
                    fractalController->SetSkyboxTextureHDR(static_cast<SkyboxTextureHDR>(current_skybox_texture_hdr));
            }
            if (ImGui::Checkbox("Use irradiance cubemap", &irradiance_cubemap)) { 
                fractalController->SetIrradianceMap(irradiance_cubemap);
                flag = true; 
            }

            if (static_cast<SkyboxTextureHDR>(current_skybox_texture_hdr) == SkyboxTextureHDR::OtherHDR) {
                if (ImGui::Button("File name:")) {
                    fileBrowserSetupSkyboxHDR.Open();
                }
                ImGui::SameLine();
                std::string t = skybox_hdr_root + "..." + skybox_hdr_name;
                ImGui::Text("%s", t.c_str());

                if (ImGui::Button("Use")) {
                    fractalController->SetSkyboxTextureHDR(skybox_hdr_path);
                }
            }

        }
    }
    //-------------------------------------------------------------------

    //--------------------------Type of fractal--------------------------
    ImGui::NewLine();
    ImGui::Separator();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + parametersSize[0]/2.0 - 57.0);
    ImGui::Text("Type of fractal");
    ImGui::Separator();
    if (ImGui::Combo("", &current_fractal_type, fractal_type, IM_ARRAYSIZE(fractal_type))) {
        currentFractalType = static_cast<FractalType>(current_fractal_type);
        fractalController->SetFractalType(currentFractalType);
        flag = true;
        if (currentFractalType == FractalType::Apollonian1 || currentFractalType == FractalType::Apollonian2 ||
            currentFractalType == FractalType::Apollonian3 || currentFractalType == FractalType::MengerSponge3) {

            currentCameraType = CameraType::CartesianCamera;
            fractalController->SetCameraType(currentCameraType);
        }
        else {
            currentCameraType = CameraType::SphericalCamera;
            fractalController->SetCameraType(currentCameraType);
        }
    }
    //-------------------------------------------------------------------
    
    if (flag) {
        //program->SetShaderParameters(soft_shadows);
        fractalController->LoadShaderProgram();
    }

    ImGui::End();
}

void Gui::FractalParameters() {
    int width, height;
    window->GetSize(&width, &height);
    ImGui::SetNextWindowPos(ImVec2((float)width*2/3-20, (float)(height*2/3) + 43.0f));
    ImVec2 parametersSize = ImVec2((float)width*1/3+20, (float)(height*1/3) - 43.0f);
	ImGui::SetNextWindowSize(parametersSize);
    
    switch(currentFractalType) {
        case FractalType::Test: {
            Test();
            break;
        }
        case FractalType::Mandelbulb: {
            Mandelbulb();
            break;
        }
        case FractalType::Juliabulb1: {
            Juliabulb1();
            break;
        }
        case FractalType::Julia1: {
            Julia1();
            break;
        }
        case FractalType::Julia2: {
            Julia2();
            break;
        }
        case FractalType::Julia3: {
            Julia3();
            break;
        }
        case FractalType::Julia4: {
            Julia4();
            break;
        }
        case FractalType::Sierpinski1: {
            Sierpinski1();
            break;
        }
        case FractalType::Sierpinski2: {
            Sierpinski2();
            break;
        }
        case FractalType::MengerSponge1: {
            MengerSponge1();
            break;
        }
        case FractalType::MengerSponge2: {
            MengerSponge2();
            break;
        }
        case FractalType::MengerSponge3: {
            MengerSponge3();
            break;
        }
        case FractalType::Apollonian1: {
            Apollonian1();
            break;
        }
        case FractalType::Apollonian2: {
            Apollonian2();
            break;
        }
        case FractalType::Apollonian3: {
            Apollonian3();
            break;
        }
    }
    
}

void Gui::FractalColor() {
    if (ImGui::Combo("Coloring type", &current_coloring_type, coloring_type, IM_ARRAYSIZE(coloring_type))) {
        currentColoringType = static_cast<ColoringType>(current_coloring_type);
        fractalController->SetColoringType(currentColoringType);
    }

    if (static_cast<ColoringType>(currentColoringType) == ColoringType::Type1) {
        if (ImGui::ColorEdit3("Type1 color", type1_color)) {
            fractalController->SetType1Color(glm::vec3(type1_color[0], type1_color[1], type1_color[2]));
        }
    }

    if (static_cast<ColoringType>(currentColoringType) == ColoringType::Type2) {
        if (ImGui::ColorEdit3("Type2 color1", type2_color1)) {
            fractalController->SetType2Color1(glm::vec3(type2_color1[0], type2_color1[1], type2_color1[2]));
        }

        if (ImGui::ColorEdit3("Type2 color2", type2_color2)) {
            fractalController->SetType2Color2(glm::vec3(type2_color2[0], type2_color2[1], type2_color2[2]));
        }

        if (ImGui::ColorEdit3("Type2 color3", type2_color3)) {
            fractalController->SetType2Color3(glm::vec3(type2_color3[0], type2_color3[1], type2_color3[2])); 
        }
    }

    if (static_cast<ColoringType>(currentColoringType) == ColoringType::Type3) {
        if (ImGui::ColorEdit3("Type3 color", type3_color)) {
            fractalController->SetType3Color(glm::vec3(type3_color[0], type3_color[1], type3_color[2]));
        }
        if (MyDragFloat("Type3 a", &type3_coef, 0.1, -10.0, 10.0)) {
            fractalController->SetType3Coef(type3_coef);
        }
    }

    if (static_cast<ColoringType>(currentColoringType) == ColoringType::Type4) {
        if (ImGui::ColorEdit3("Type4 color", type4_color)) {
            fractalController->SetType4Color(glm::vec3(type4_color[0], type4_color[1], type4_color[2]));
        }
    }

    if (static_cast<ColoringType>(currentColoringType) == ColoringType::Type5) {
        if (ImGui::ColorEdit3("Type5 color1", type5_color1)) {
            fractalController->SetType5Color1(glm::vec3(type5_color1[0], type5_color1[1], type5_color1[2]));
        }

        if (ImGui::ColorEdit3("Type5 color2", type5_color2)) {
            fractalController->SetType5Color2(glm::vec3(type5_color2[0], type5_color2[1], type5_color2[2]));
        }

        if (ImGui::ColorEdit3("Type5 color3", type5_color3)) {
            fractalController->SetType5Color3(glm::vec3(type5_color3[0], type5_color3[1], type5_color3[2])); 
        }
    }

    if (static_cast<ColoringType>(currentColoringType) == ColoringType::Type6) {
        if (ImGui::ColorEdit3("Type6 color", type6_color)) {
            fractalController->SetType6Color(glm::vec3(type6_color[0], type6_color[1], type6_color[2]));
        }
    }

    if (static_cast<ColoringType>(currentColoringType) == ColoringType::Type7) {
        if (ImGui::ColorEdit3("Type7 color1", type7_color1)) {
            fractalController->SetType7Color1(glm::vec3(type7_color1[0], type7_color1[1], type7_color1[2]));
        }

        if (ImGui::ColorEdit3("Type7 color2", type7_color2)) {
            fractalController->SetType7Color2(glm::vec3(type7_color2[0], type7_color2[1], type7_color2[2]));
        }

        if (ImGui::ColorEdit3("Type7 color3", type7_color3)) {
            fractalController->SetType7Color3(glm::vec3(type7_color3[0], type7_color3[1], type7_color3[2])); 
        }
    }

    if (currentFractalType != FractalType::Apollonian1 && currentFractalType != FractalType::Apollonian2 &&
        currentFractalType != FractalType::Apollonian3 && currentFractalType != FractalType::MengerSponge3) {

        if (MyDragFloat("Shininess", &shininess, 1, 1, 100)) {
            fractalController->SetFractalShininess(shininess);
        }
        if (MyDragFloat("Reflection", &reflection, 0.01, 0, 1)) {
            fractalController->SetFractalReflect(reflection);
        }
    }
}

void Gui::Test() {
    ImGui::Begin("Test parameters", NULL, parametersWindowFlags); 
    FractalColor();
    ImGui::End();
}

void Gui::Mandelbulb() {
    ImGui::Begin("Mandelbulb parameters", NULL, parametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (MyDragInt("Iterations", &mandelbulb_iterations, 0.1, 1, 15)) {
        fractalController->SetMandelbulbIterations(mandelbulb_iterations);
    }
    if (MyDragFloat("Bailout", &mandelbulb_bailout, 0.1, 1, 30)) {
        fractalController->SetMandelbulbBailout(mandelbulb_bailout);
    }
    if (MyDragFloat("Power", &mandelbulb_power, 0.1, 1, 30)) {
        fractalController->SetMandelbulbPower(mandelbulb_power);
    }
    ImGui::End();
}

void Gui::Juliabulb1() {
    ImGui::Begin("Juliabulb1 parameters", NULL, parametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (MyDragFloat("Juliabulb1 shift", &juliabulb1_offset, 0.01, 0, 50)) {
        fractalController->SetJuliabulb1Offset(juliabulb1_offset);
    }
    if (MyDragFloat("Juliabulb1 smooth", &juliabulb1_smoothness, 0.1, 0, 45)) {
        fractalController->SetJuliabulb1Smoothness(juliabulb1_smoothness);
    }
    if (MyDragInt("Juliabulb1 it", &juliabulb1_iterations, 0.1, 0, 10)) {
        fractalController->SetJuliabulb1Iterations(juliabulb1_iterations);
    }
    ImGui::End();
}

void Gui::Julia1() {
    ImGui::Begin("Julia1 parameters", NULL, parametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (MyDragFloat("Julia1 offset", &julia1_offset, 0.01, 0, 100)) {
        fractalController->SetJulia1Offset(julia1_offset);
    }
    if (MyDragFloat("Julia1 smoothness", &julia1_smoothness, 0.1, 0, 45)) {
        fractalController->SetJulia1Smoothness(julia1_smoothness);
    }
    if (MyDragInt("Julia1 i", &julia1_iterations, 0.1, 0, 10)) {
        fractalController->SetJulia1Iterations(julia1_iterations);
    }
    ImGui::End();
}

void Gui::Julia2() {
    ImGui::Begin("Julia2 parameters", NULL, fractalParametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (MyDragFloat3("Julia2 offset", julia2_offset, 0.01, 0, 100)) {
        fractalController->SetJulia2Offset(glm::vec3(julia2_offset[0], julia2_offset[1], julia2_offset[2]));
    }
    if (MyDragFloat("Julia2 W", &julia2_w, 0.01, 0, 100)) {
        fractalController->SetJulia2W(julia2_w);
    }
    if (MyDragFloat("Julia2 smoothness", &julia2_smoothness, 0.1, 0, 45)) {
        fractalController->SetJulia2Smoothness(julia2_smoothness);
    }
    if (MyDragInt("Julia2 i", &julia2_iterations, 0.1, 0, 10)) {
        fractalController->SetJulia2Iterations(julia2_iterations);
    }
    ImGui::End();
}

void Gui::Julia3() {
    ImGui::Begin("Julia3 parameters", NULL, parametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (MyDragFloat("Julia3 offset", &julia3_offset, 0.01, 0, 100)) {
        fractalController->SetJulia3Offset(julia3_offset);
    }
    if (MyDragFloat("Julia3 smoothness", &julia3_smoothness, 0.1, 0, 100)) {
        fractalController->SetJulia3Smoothness(julia3_smoothness);
    }
    if (MyDragInt("Julia3 i", &julia3_iterations, 0.1, 0, 10)) {
        fractalController->SetJulia3Iterations(julia3_iterations);
    }
    ImGui::End();
}

void Gui::Julia4() {
    ImGui::Begin("Julia4 parameters", NULL, parametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (MyDragFloat3("Julia4 offset", julia4_offset, 0.01, -100, 100)) {
        fractalController->SetJulia4Offset(glm::vec3(julia4_offset[0], julia4_offset[1], julia4_offset[2]));
    }
    if (MyDragFloat("Julia4 W", &julia4_w, 0.01, -100, 100)) {
        fractalController->SetJulia4W(julia4_w);
    }
    if (MyDragFloat("Julia4 smoothness", &julia4_smoothness, 0.1, 0, 45)) {
        fractalController->SetJulia4Smoothness(julia4_smoothness);
    }
    if (MyDragInt("Julia4 i", &julia4_iterations, 0.1, 0, 10)) {
        fractalController->SetJulia4Iterations(julia4_iterations);
    }
    ImGui::End();
}

void Gui::Sierpinski1() {
    ImGui::Begin("Sierpinski1 parameters", NULL, fractalParametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (MyDragFloat3("Sierpinski1 v1", sierpinski1_va, 0.01, -100, 100)) {
        fractalController->SetSierpinski1Vector1(glm::vec3(sierpinski1_va[0], sierpinski1_va[1], sierpinski1_va[2]));
    }
    if (MyDragFloat3("Sierpinski1 v2", sierpinski1_vb, 0.01, -100, 100)) {
        fractalController->SetSierpinski1Vector2(glm::vec3(sierpinski1_vb[0], sierpinski1_vb[1], sierpinski1_vb[2]));
    }
    if (MyDragFloat3("Sierpinski1 v3", sierpinski1_vc, 0.01, -100, 100)) {
        fractalController->SetSierpinski1Vector3(glm::vec3(sierpinski1_vc[0], sierpinski1_vc[1], sierpinski1_vc[2]));
    }
    if (MyDragFloat3("Sierpinski1 v4", sierpinski1_vd, 0.01, -100, 100)) {
        fractalController->SetSierpinski1Vector4(glm::vec3(sierpinski1_vd[0], sierpinski1_vd[1], sierpinski1_vd[2]));
    }
    if (MyDragInt("Sierpinski1 i", &sierpinski1_iterations, 0.1, 1, 15)) {
        fractalController->SetSierpinski1Iterations(sierpinski1_iterations);
    }
    
    ImGui::End();
}

void Gui::Sierpinski2() {
    ImGui::Begin("Sierpinski2 parameters", NULL, parametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (MyDragFloat3("Sierpinski2 v1", sierpinski2_va, 0.01, -100, 100)) {
        fractalController->SetSierpinski2Vector1(glm::vec3(sierpinski2_va[0], sierpinski2_va[1], sierpinski2_va[2]));
    }
    if (MyDragFloat3("Sierpinski2 v2", sierpinski2_vb, 0.01, -100, 100)) {
        fractalController->SetSierpinski2Vector2(glm::vec3(sierpinski2_vb[0], sierpinski2_vb[1], sierpinski2_vb[2]));
    }
    if (MyDragFloat3("Sierpinski2 v3", sierpinski2_vc, 0.01, -100, 100)) {
        fractalController->SetSierpinski2Vector3(glm::vec3(sierpinski2_vc[0], sierpinski2_vc[1], sierpinski2_vc[2]));
    }
    if (MyDragInt("Sierpinski2 i", &sierpinski2_iterations, 0.1, 0, 10)) {
        fractalController->SetSierpinski2Iterations(sierpinski2_iterations);
    }
    ImGui::End();
}

void Gui::MengerSponge1() {
    ImGui::Begin("MengerSponge1 parameters", NULL, parametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (MyDragFloat("Sponge1 offset1", &menger_sponge1_offset1, 0.01, -30, 30)) {
        fractalController->SetMengerSponge1Offset1(menger_sponge1_offset1);
    }
    if (MyDragFloat("Sponge1 offset2", &menger_sponge1_offset2, 0.01, -30, 30)) {
        fractalController->SetMengerSponge1Offset2(menger_sponge1_offset2);
    }
    if (MyDragInt("Sponge1 i", &menger_sponge1_iterations, 0.1, 0, 10)) {
        fractalController->SetMengerSponge1Iterations(menger_sponge1_iterations);
    }
    ImGui::End();
}

void Gui::MengerSponge2() {
    ImGui::Begin("MengerSponge2 parameters", NULL, parametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (MyDragFloat("Sponge2 offset1", &menger_sponge2_offset1, 0.01, -30, 30)) {
        fractalController->SetMengerSponge2Offset1(menger_sponge2_offset1);
    }
    if (MyDragFloat("Sponge2 ofset2", &menger_sponge2_offset2, 0.01, -30, 30)) {
        fractalController->SetMengerSponge2Offset2(menger_sponge2_offset2);
    }
    if (MyDragInt("Sponge2 i", &menger_sponge2_iterations, 0.1, 0, 10)) {
        fractalController->SetMengerSponge2Iterations(menger_sponge2_iterations);
    }
    ImGui::End();
}

void Gui::MengerSponge3() {
    ImGui::Begin("MengerSponge3 parameters", NULL, parametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (ImGui::Combo("Sponge type", &current_menger_sponge3_type, menger_sponge3_type, IM_ARRAYSIZE(menger_sponge3_type))) {
        fractalController->SetMengerSponge3Type(current_menger_sponge3_type);
    }
    ImGui::End();
}

void Gui::Apollonian1() {
    ImGui::Begin("Apollonian1 parameters", NULL, parametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (MyDragFloat("Apollonian1 a1", &apollonian1_offset1, 0.01, 12, 20)) {
        fractalController->SetApollonian1Offset1(apollonian1_offset1);
    }
    if (MyDragFloat("Apollonian1 a2", &apollonian1_offset2, 0.01, 0, 3)) {
        fractalController->SetApollonian1Offset2(apollonian1_offset2);
    }
    if (MyDragInt("Apollonian1 i", &apollonian1_iterations, 0.1, 1, 15)) {
        fractalController->SetApollonian1Iterations(apollonian1_iterations);
    }
    ImGui::End();
}

void Gui::Apollonian2() {
    ImGui::Begin("Apollonian2 parameters", NULL, parametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (MyDragFloat("Apollonian2 a1", &apollonian2_offset1, 0.01, 12, 20)) {
        fractalController->SetApollonian2Offset1(apollonian2_offset1);
    }
    if (MyDragFloat("Apollonian2 a2", &apollonian2_offset2, 0.01, 0, 3)) {
        fractalController->SetApollonian2Offset2(apollonian2_offset2);
    }
    if (MyDragInt("Apollonian2 i", &apollonian2_iterations, 0.1, 1, 15)) {
        fractalController->SetApollonian2Iterations(apollonian2_iterations);
    }
    ImGui::End();
}

void Gui::Apollonian3() {
    ImGui::Begin("Apollonian3 parameters", NULL, parametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (MyDragFloat("Apollonian3 a1", &apollonian3_offset1, 0.01, 12, 20)) {
        fractalController->SetApollonian3Offset1(apollonian3_offset1);
    }
    if (MyDragFloat("Apollonian3 a2", &apollonian3_offset2, 0.01, 0, 3)) {
        fractalController->SetApollonian3Offset2(apollonian3_offset2);
    }
    if (MyDragFloat("Apollonian3 a3", &apollonian3_offset3, 0.01, 0, 5)) {
        fractalController->SetApollonian3Offset3(apollonian3_offset3);
    }
    if (MyDragFloat3("Apollonian c", apollonian3_csize, 0.1, 0, 10)) {
        fractalController->SetApollonian3Csize(glm::vec3(apollonian3_csize[0], apollonian3_csize[1], apollonian3_csize[2]));
    }
    if (MyDragInt("Apollonian3 i", &apollonian3_iterations, 0.1, 1, 15)) {
        fractalController->SetApollonian3Iterations(apollonian3_iterations);
    }
    ImGui::End();
}

void Gui::ExportAs() {
    std::string name; 
    switch(currentExportType) {
        case ExportType::Png: {
            name = "PNG";
            break;
        }
        case ExportType::Bmp: {
            name = "BMP";
            break;
        }
        case ExportType::Jpg: {
            name = "JPG";
            break;
        }
        case ExportType::Tga: {
            name = "TGA";
            break;
        }
    }
    if (exportWindowFlag) {
        ImGuiWindowFlags exportWindowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;
        ImGui::Begin(("Export " + name).c_str(), &exportWindowFlag, exportWindowFlags);
        int width, height;
        window->GetSize(&width, &height);
        ImGui::SetNextWindowPos(ImVec2(0.25f*width, 0.25f*height));
        ImVec2 windowSize = ImVec2(0.75f*width, 0.75f*height);
	    ImGui::SetNextWindowSize(windowSize);

        if (ImGui::Button("File name:")) {
            fileBrowserSaveImage.Open();
        }
        ImGui::SameLine();
        std::string t = output_path + output_name;
        ImGui::Text("%s", t.c_str());

        ImGui::Text("Width:");
        ImGui::SetNextItemWidth(200);
        ImGui::SameLine();
        MyDragInt("##width", &output_width, 1, 120, 8640);
        ImGui::SameLine();
        ImGui::Text("(120..8640)");

        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 50.0f);
        ImGui::Text("Height:");
        ImGui::SetNextItemWidth(200);
        ImGui::SameLine();
        MyDragInt("##height", &output_height, 1, 120, 8640);
        ImGui::SameLine();
        ImGui::Text("(120..8640)");

        if (currentExportType == ExportType::Jpg) {
            ImGui::Text("Quality:");
            ImGui::SetNextItemWidth(200);
            ImGui::SameLine();
            MyDragInt("##quality", &output_quality, 1, 1, 100);
            ImGui::SameLine();
            ImGui::Text("(1..100)");
        }

        ImGui::Text("Anti-aliasing:");
        ImGui::SetNextItemWidth(200);
        ImGui::SameLine();
        MyDragInt("##AA", &anti_aliasing, 0.1, 1, 10);
        ImGui::SameLine();
        ImGui::Text("(1..10)");

        if (ImGui::Button("Save")) {
            fractalController->SetAntiAliasing(anti_aliasing);
            int fbo_height, fbo_width;
            fbo_height = fbo->GetHeight();
            fbo_width = fbo->GetWidth();
            fbo->Resize(output_width, output_height);
            fractalController->Render(output_width, output_height);
            fbo->Bind();
            unsigned char* imageData = (unsigned char*)malloc(output_width*output_height*3);
	        GLCall(glReadPixels(0, 0, output_width, output_height, GL_RGB, GL_UNSIGNED_BYTE, imageData));
            unsigned char* imageData2 = (unsigned char*)malloc(output_width*output_height*3);
            int k = 0;
            for (int i = output_height - 1; i > 0; i--) {
                for (int j = 0; j < output_width*3; j++) {
                    imageData2[k++] = imageData[i*output_width*3 + j];
                }
            }

            switch(currentExportType) {
                case ExportType::Png: {
                    stbi_write_png((output_path + output_name).c_str(), output_width, output_height, 3, imageData2, output_width * 3);
                    break;
                }
                case ExportType::Bmp: {
                    stbi_write_bmp((output_path + output_name).c_str(), output_width, output_height, 3, imageData2);
                    break;
                }
                case ExportType::Jpg: {
                    stbi_write_jpg((output_path + output_name).c_str(), output_width, output_height, 3, imageData2, output_quality);
                    break;
                }
                case ExportType::Tga: {
                    stbi_write_tga((output_path + output_name).c_str(), output_width, output_height, 3, imageData2);
                    break;
                }
            }
            exportWindowFlag = false;
            anti_aliasing = 1;
            fractalController->SetAntiAliasing(anti_aliasing);

	        free(imageData);
            free(imageData2);
            fbo->Unbind();
            fbo->Resize(fbo_width, fbo_height);
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            exportWindowFlag = false;
        }
        ImGui::End();
    }
}

void Gui::SaveParameters(const char* path) {
	nlohmann::json j;

	//--------------------------Save camera parameters--------------------------
	glm::mat4 view = camera->GetViewMatrix();
	float* start = glm::value_ptr(view);
	std::vector<float> view_vector(start, start + 16);

	glm::vec3 position = camera->GetPosition();
	start = glm::value_ptr(position);
	std::vector<float> position_vector(start, start + 3);

	float fieldOfView = camera->GetFieldOfView();
	CameraType camera_type = camera->GetCameraType();

	j["CameraType"] = (int)camera_type;

	switch(camera_type) {
		case CameraType::CartesianCamera: {
			float pitch = camera->GetPitch();
			float jaw = camera->GetYaw();

			j["Camera"] = { {"View", view_vector}, 
                            {"Position", position_vector}, 
                            {"FOV", fieldOfView}, 
                            {"CameraType", (int)camera_type},
							{"Pitch", pitch}, 
                            {"Jaw", jaw} };
			break;
		}
		case CameraType::SphericalCamera: {
			float phi = camera->GetPhi();
			float theta = camera->GetTheta();
			float r = camera->GetR();
			j["Camera"] = { {"View", view_vector}, 
                            {"Position", position_vector}, 
                            {"FOV", fieldOfView}, 
                            {"CameraType", (int)camera_type},
							{"Phi", phi}, 
                            {"Theta", theta}, 
                            {"R", r} };
			break;
		}
	}
	//--------------------------------------------------------------------------

	//---------------------------Save light parameters--------------------------
    j["Light1"] = { {"Direction", light_direction1}, {"Color", light_color1}, {"Intensity", light_intensity1} };
    j["Light2"] = { {"Direction", light_direction2}, {"Color", light_color2}, {"Intensity", light_intensity2} };
	j["AmbientLight"] = { {"Color", ambient_fractal_light_color}, {"Intensity", ambient_light_intensity} };
    //--------------------------------------------------------------------------

	//-----------------------Save general fractal parameters--------------------
	j["SoftShadows"] = soft_shadows;
	j["ShadowStrength"] = shadow_strength;
	j["Shininess"] = shininess;
	j["Reflection"] = reflection;
	j["ToneMapping"] = tone_mapping;
	j["Exposure"] = exposure;
	j["AntiAliasing"] = anti_aliasing;
	//--------------------------------------------------------------------------

	//----------------------Save background parameters--------------------------
	j["RoomBackground"] = room_background;
	j["BackgroundType"] = current_background_type;

	switch(static_cast<BackgroundType>(current_background_type)) {
		case BackgroundType::Solid: {
			j["SolidBackground"] = { {"Color", background_color} };
			break;
		}
		case BackgroundType::SolidWithSun: {
			j["SolidBackgroundWithSun"] = { {"Color", background_color}, 
                                            {"Sun", sun_color} };
			break;
		}
		case BackgroundType::Skybox: {
			if (static_cast<SkyboxTexture>(current_skybox_texture) == SkyboxTexture::Other) {
				j["Skybox"] = { {"Brightness", background_brightness}, 
                                     {"Path", skybox_paths},
                                     {"Type", current_skybox_texture} };
			}
			else {
				j["Skybox"] = { {"Brightness", background_brightness}, 
                                {"Type", current_skybox_texture} };
			}
			break;
		}
		case BackgroundType::SkyboxHDR: {
			if (static_cast<SkyboxTextureHDR>(current_skybox_texture_hdr) == SkyboxTextureHDR::OtherHDR) {
				j["SkyboxHDR"] = { {"Brightness", background_brightness}, 
                                        {"Irradiance", irradiance_cubemap}, 
                                        {"Path", skybox_hdr_path},
                                        {"Type", current_skybox_texture_hdr} };
			}
			else {
				j["SkyboxHDR"] = { {"Brightness", background_brightness}, 
                                   {"Irradiance", irradiance_cubemap}, 
                                   {"Type", current_skybox_texture_hdr} };
			}
			break;
		}
	}
	//--------------------------------------------------------------------------

	//-----------------------Save fractal coloring parameters-------------------
	j["ColoringType"] = (int)currentColoringType;

	switch(currentColoringType) {
		case ColoringType::Type1: {
			j["Type1"] = { {"Color", type1_color} };
			break;
		}
		case ColoringType::Type2: {
			j["Type2"] = { {"Color1", type2_color1}, 
                           {"Color2", type2_color2}, 
                           {"Color3", type2_color3} };
			break;
		}
		case ColoringType::Type3: {
			j["Type3"] = { {"Color", type3_color}, 
                           {"Coef", type3_coef} };
			break;
		}
		case ColoringType::Type4: {
			j["Type4"] = { {"Color", type4_color} };
			break;
		}
		case ColoringType::Type5: {
			j["Type5"] = { {"Color1", type5_color1}, 
                           {"Color2", type5_color2}, 
                           {"Color3", type5_color3} };
			break;
		}
		case ColoringType::Type6: {
			j["Type6"] = { {"Color", type6_color} };
			break;
		}
		case ColoringType::Type7: {
			j["Type7"] = { {"Color1", type7_color1}, 
                           {"Color2", type7_color2}, 
                           {"Color3", type7_color3} };
			break;
		}
	}
	//--------------------------------------------------------------------------

	//--------------------------Save fractal parameters-------------------------
    j["FractalType"] = current_fractal_type;

	switch(currentFractalType) {
		case FractalType::Test: {
			break;
		}
		case FractalType::Mandelbulb: {
			j["Mandelbulb"] = { {"Iterations", mandelbulb_iterations}, 
                                {"Bailout", mandelbulb_bailout}, 
                                {"Power", mandelbulb_power} };
			break;
		}
		case FractalType::Juliabulb1: {
			j["Juliabulb1"] = { {"Iterations", juliabulb1_iterations}, 
                                {"Offset", juliabulb1_offset}, 
                                {"Smoothness", juliabulb1_smoothness} };
			break;
		}
		case FractalType::Julia1: {
			j["Julia1"] = { {"Iterations", julia1_iterations}, 
                            {"Offset", julia1_offset}, 
                            {"Smoothness", julia1_smoothness} };
			break;
		}
		case FractalType::Julia2: {
			j["Julia2"] = { {"Iterations", julia2_iterations}, 
                            {"Smoothness", julia2_smoothness}, 
                            {"W", julia2_w}, 
                            {"Offset", julia2_offset} };
			break;
		}
		case FractalType::Julia3: {
			j["Julia3"] = { {"Iterations", julia3_iterations}, 
                            {"Offset", julia3_offset}, 
                            {"Smoothness", julia3_smoothness} };
			break;
		}
		case FractalType::Julia4: {
			j["Julia4"] = { {"Iterations", julia4_iterations}, 
                            {"Smoothness", julia4_smoothness}, 
                            {"W", julia4_w}, 
                            {"Offset", julia4_offset} };
			break;
		}
		case FractalType::Sierpinski1: {
			j["Sierpinski1"] = { {"Vector1", sierpinski1_va}, 
                                 {"Vector2", sierpinski1_vb}, 
                                 {"Vector3", sierpinski1_vc}, 
                                 {"Vector4", sierpinski1_vd}, 
                                 {"Iterations", sierpinski1_iterations} };
			break;
		}
		case FractalType::Sierpinski2: {
			j["Sierpinski2"] = { {"Vector1", sierpinski2_va}, 
                                 {"Vector2", sierpinski2_vb}, 
                                 {"Vector3", sierpinski2_vc}, 
                                 {"Iterations", sierpinski2_iterations} };

			break;
		}
		case FractalType::MengerSponge1: {
			j["MengerSponge1"] = { {"Offset1", menger_sponge1_offset1}, 
                                   {"Offset2", menger_sponge1_offset2}, 
                                   {"Iterations", menger_sponge1_iterations} };
			break;
		}
		case FractalType::MengerSponge2: {
			j["MengerSponge2"] = { {"Offset1", menger_sponge2_offset1}, 
                                   {"Offset2", menger_sponge2_offset2}, 
                                   {"Iterations", menger_sponge2_iterations} };
			break;
		}
		case FractalType::MengerSponge3: {
			j["MengerSponge3"] = { {"Type", menger_sponge3_type} };
			break;
		}
		case FractalType::Apollonian1: {
			j["Apollonian1"] = { {"Offset1", apollonian1_offset1}, 
                                 {"Offset2", apollonian1_offset2}, 
                                 {"Iterations", apollonian1_iterations} };
			break;
		}
		case FractalType::Apollonian2: {
			j["Apollonian2"] = { {"Offset1", apollonian2_offset1}, 
                                 {"Offset2", apollonian2_offset2}, 
                                 {"Iterations", apollonian2_iterations} };
			break;
		}
		case FractalType::Apollonian3: {
			j["Apollonian3"] = { {"Offset1", apollonian3_offset1}, 
                                 {"Offset2", apollonian3_offset2}, 
                                 {"Offset3", apollonian3_offset3}, 
                                 {"Iterations", apollonian3_iterations}, 
                                 {"CSize", apollonian3_csize} };
			break;
		}
	}
	//--------------------------------------------------------------------------
    //std::cout << j["Apollonian3"]["Offset1"] << std::endl;
    //std::cout << j["FractalType"] << std::endl;

	std::ofstream file(path);
	file << j << std::endl;
}

void Gui::LoadParameters(const char* path) {
    std::ifstream fail(path);
    nlohmann::json j;
    fail >> j;

    //--------------------------Load camera parameters--------------------------
    std::vector<float> view_vector = j["Camera"]["View"].get<std::vector<float>>();
    glm::mat4 view = glm::make_mat4(view_vector.data());
    camera->SetViewMatrix(view);

    std::vector<float> position_vector = j["Camera"]["Position"].get<std::vector<float>>();
    glm::vec3 position = glm::make_vec3(position_vector.data());
    camera->SetPosition(position);

    camera->SetFieldOfView(j["Camera"]["FOV"]);
    currentCameraType = static_cast<CameraType>(j["CameraType"]);
    camera->SetCameraType(currentCameraType);

    switch(currentCameraType) {
        case CameraType::CartesianCamera: {
            camera->SetPitch(j["Camera"]["Pitch"]);
            camera->SetYaw(j["Camera"]["Jaw"]);
            break;
        }
        case CameraType::SphericalCamera: {
            camera->SetPhi(j["Camera"]["Phi"]);
            camera->SetTheta(j["Camera"]["Theta"]);
            camera->SetR(j["Camera"]["R"]);
            break;
        }
    }

    //---------------------------Load light1 parameters--------------------------
    std::vector<float> light_direction1_vector =  j["Light1"]["Direction"].get<std::vector<float>>();
    light_direction1[1] = glm::degrees(acos(light_direction1_vector[1]));
    light_direction1[0] = glm::degrees( acos(light_direction1_vector[0]/sin(glm::radians(light_direction1[1]))) );
    fractalController->SetLightDirection1(glm::make_vec3(light_direction1_vector.data()));
    
    std::vector<float> light_color1_vector = j["Light1"]["Color"].get<std::vector<float>>();
    memcpy(light_color1, light_color1_vector.data(), 3);
    fractalController->SetLightColor1(glm::make_vec3(light_color1_vector.data()));

    light_intensity1 = j["Light1"]["Intensity"];
    fractalController->SetLightIntensity1(light_intensity1);
    //--------------------------------------------------------------------------

    //---------------------------Load light2 parameters--------------------------
    std::vector<float> light_direction2_vector =  j["Light2"]["Direction"].get<std::vector<float>>();
    light_direction2[1] = glm::degrees(acos(light_direction2_vector[1]));
    light_direction2[0] = glm::degrees( acos(light_direction2_vector[0]/sin(glm::radians(light_direction2[1]))) );
    fractalController->SetLightDirection2(glm::make_vec3(light_direction2_vector.data()));
    
    std::vector<float> light_color2_vector = j["Light2"]["Color"].get<std::vector<float>>();
    memcpy(light_color2, light_color2_vector.data(), 3);
    fractalController->SetLightColor2(glm::make_vec3(light_color2_vector.data()));

    light_intensity2 = j["Light2"]["Intensity"];
    fractalController->SetLightIntensity2(light_intensity2);
    //--------------------------------------------------------------------------

    //------------------------Load ambient light parameters---------------------
    std::vector<float> ambient_color_vector = j["AmbientLight"]["Color"].get<std::vector<float>>();
    memcpy(ambient_fractal_light_color, ambient_color_vector.data(), 3);
    fractalController->SetLightColor2(glm::make_vec3(ambient_color_vector.data()));

    ambient_light_intensity = j["AmbientLight"]["Intensity"];
    fractalController->SetLightIntensity2(ambient_light_intensity);
    //--------------------------------------------------------------------------

    //-----------------------Load general fractal parameters--------------------
	soft_shadows = j["SoftShadows"];
    fractalController->SetSoftShadows(soft_shadows);

	shadow_strength = j["ShadowStrength"];
    fractalController->SetShadowStrength(shadow_strength);

	shininess = j["Shininess"];
    fractalController->SetFractalShininess(shininess);

	reflection = j["Reflection"];
    fractalController->SetFractalReflect(reflection);

	tone_mapping = j["ToneMapping"];
    fractalController->SetToneMapping(tone_mapping);

    std::vector<float> exposure_vector = j["Exposure"].get<std::vector<float>>();
    memcpy(exposure, exposure_vector.data(), 3);
    fractalController->SetExposure(glm::make_vec3(exposure_vector.data()));

	// anti_aliasing = j["AntiAliasing"];
	//--------------------------------------------------------------------------

    //----------------------Load background parameters--------------------------
    std::vector<float> room_background_vector = j["RoomBackground"].get<std::vector<float>>();
    memcpy(room_background, room_background_vector.data(), 3);
    fractalController->SetRoomBackgroundColor(glm::make_vec3(room_background_vector.data()));
    
    current_background_type = j["BackgroundType"];

	switch(static_cast<BackgroundType>(current_background_type)) {
		case BackgroundType::Solid: {
            std::vector<float> background_color_vector = j["SolidBackground"]["Color"].get<std::vector<float>>();
            memcpy(background_color, background_color_vector.data(), 3);
            fractalController->SetBackgroundColor(glm::make_vec3(background_color_vector.data()));
    		break;
		}
		case BackgroundType::SolidWithSun: {
            std::vector<float> background_color_vector = j["SolidBackgroundWithSun"]["Color"].get<std::vector<float>>();
            memcpy(background_color, background_color_vector.data(), 3);
            fractalController->SetBackgroundColor(glm::make_vec3(background_color_vector.data()));
    		
            std::vector<float> sun_color_vector = j["SolidBackgroundWithSun"]["Sun"].get<std::vector<float>>();
            memcpy(sun_color, sun_color_vector.data(), 3);
            fractalController->SetSunColor(glm::make_vec3(sun_color_vector.data()));
    		
			break;
		}
		case BackgroundType::Skybox: {
            current_skybox_texture = j["Skybox"]["Type"];

            background_brightness = j["Skybox"]["Brightness"];
            fractalController->SetBackgroundBrightness(background_brightness);

			if (static_cast<SkyboxTexture>(current_skybox_texture) == SkyboxTexture::Other) {   
                std::vector<std::string> skybox_paths_vector = j["Skybox"]["Path"].get<std::vector<std::string>>();
                std::copy_n(skybox_paths_vector.begin(), 6, skybox_paths.begin());
                fractalController->SetSkyboxTexture(skybox_paths);
            }
			
			break;
		}
		case BackgroundType::SkyboxHDR: {
            current_skybox_texture_hdr = j["SkyboxHDR"]["Type"];

            background_brightness = j["SkyboxHDR"]["Brightness"];
            fractalController->SetBackgroundBrightness(background_brightness);

            irradiance_cubemap =  j["SkyboxHDR"]["Irradiance"];
            fractalController->SetIrradianceMap(irradiance_cubemap);

			if (static_cast<SkyboxTextureHDR>(current_skybox_texture_hdr) == SkyboxTextureHDR::OtherHDR) {
			    skybox_hdr_path = j["SkyboxHDR"]["Path"];
			}
	
			break;
		}
	}
	//--------------------------------------------------------------------------

    //-----------------------Load fractal coloring parameters-------------------
	current_coloring_type = j["ColoringType"];
    currentColoringType = static_cast<ColoringType>(current_coloring_type);

	switch(currentColoringType) {
		case ColoringType::Type1: {
            std::vector<float> type1_color_vector = j["Type1"]["Color"].get<std::vector<float>>();
            memcpy(type1_color, type1_color_vector.data(), 3);
            fractalController->SetType1Color(glm::make_vec3(type1_color_vector.data()));
			break;
		}
		case ColoringType::Type2: {
            std::vector<float> type2_color1_vector = j["Type2"]["Color1"].get<std::vector<float>>();
            memcpy(type2_color1, type2_color1_vector.data(), 3);
            fractalController->SetType2Color1(glm::make_vec3(type2_color1_vector.data()));
			
            std::vector<float> type2_color2_vector = j["Type2"]["Color2"].get<std::vector<float>>();
            memcpy(type2_color2, type2_color2_vector.data(), 3);
            fractalController->SetType2Color2(glm::make_vec3(type2_color2_vector.data()));
			
            std::vector<float> type2_color3_vector = j["Type2"]["Color3"].get<std::vector<float>>();
            memcpy(type2_color3, type2_color3_vector.data(), 3);
            fractalController->SetType2Color3(glm::make_vec3(type2_color3_vector.data()));
			
			break;
		}
		case ColoringType::Type3: {
            std::vector<float> type3_color_vector = j["Type3"]["Color"].get<std::vector<float>>();
            memcpy(type3_color, type3_color_vector.data(), 3);
            fractalController->SetType3Color(glm::make_vec3(type3_color_vector.data()));
			
            type3_coef = j["Type3"]["Coef"];
			fractalController->SetType3Coef(type3_coef);

			break;
		}
		case ColoringType::Type4: {
            std::vector<float> type4_color_vector = j["Type4"]["Color"].get<std::vector<float>>();
            memcpy(type4_color, type4_color_vector.data(), 3);
            fractalController->SetType4Color(glm::make_vec3(type4_color_vector.data()));
			break;
		}
		case ColoringType::Type5: {
            std::vector<float> type5_color1_vector = j["Type5"]["Color1"].get<std::vector<float>>();
            memcpy(type5_color1, type5_color1_vector.data(), 3);
            fractalController->SetType5Color1(glm::make_vec3(type5_color1_vector.data()));
			
            std::vector<float> type5_color2_vector = j["Type5"]["Color2"].get<std::vector<float>>();
            memcpy(type5_color2, type5_color2_vector.data(), 3);
            fractalController->SetType5Color2(glm::make_vec3(type5_color2_vector.data()));
			
            std::vector<float> type5_color3_vector = j["Type5"]["Color3"].get<std::vector<float>>();
            memcpy(type5_color3, type5_color3_vector.data(), 3);
            fractalController->SetType5Color3(glm::make_vec3(type5_color3_vector.data()));
			
			break;
		}
		case ColoringType::Type6: {
            std::vector<float> type6_color_vector = j["Type6"]["Color"].get<std::vector<float>>();
            memcpy(type6_color, type6_color_vector.data(), 3);
            fractalController->SetType6Color(glm::make_vec3(type6_color_vector.data()));
			break;
		}
		case ColoringType::Type7: {
            std::vector<float> type7_color1_vector = j["Type7"]["Color1"].get<std::vector<float>>();
            memcpy(type7_color1, type7_color1_vector.data(), 3);
            fractalController->SetType7Color1(glm::make_vec3(type7_color1_vector.data()));
			
            std::vector<float> type7_color2_vector = j["Type7"]["Color2"].get<std::vector<float>>();
            memcpy(type7_color2, type7_color2_vector.data(), 3);
            fractalController->SetType7Color2(glm::make_vec3(type7_color2_vector.data()));
			
            std::vector<float> type7_color3_vector = j["Type7"]["Color3"].get<std::vector<float>>();
            memcpy(type7_color3, type7_color3_vector.data(), 3);
            fractalController->SetType7Color3(glm::make_vec3(type7_color3_vector.data()));
			
			break;
		}
	}
	//--------------------------------------------------------------------------

    //--------------------------Load fractal parameters-------------------------
    current_fractal_type = j["FractalType"];
    currentFractalType = static_cast<FractalType>(current_fractal_type);

	switch(currentFractalType) {
		case FractalType::Test: {
			break;
		}
		case FractalType::Mandelbulb: {
            mandelbulb_iterations = j["Mandelbulb"]["Iterations"];
            fractalController->SetMandelbulbIterations(mandelbulb_iterations);

            mandelbulb_bailout = j["Mandelbulb"]["Bailout"];
            fractalController->SetMandelbulbBailout(mandelbulb_bailout);

            mandelbulb_power = j["Mandelbulb"]["Power"];
            fractalController->SetMandelbulbPower(mandelbulb_power);

			break;
		}
		case FractalType::Juliabulb1: {
            juliabulb1_iterations = j["Juliabulb1"]["Iterations"];
            fractalController->SetJuliabulb1Iterations(juliabulb1_iterations);

            juliabulb1_offset = j["Juliabulb1"]["Offset"];
            fractalController->SetJuliabulb1Offset(juliabulb1_offset);

            juliabulb1_smoothness = j["Juliabulb1"]["Smoothness"];
            fractalController->SetJuliabulb1Smoothness(juliabulb1_smoothness);

			break;
		}
		case FractalType::Julia1: {
            julia1_iterations = j["Julia1"]["Iterations"];
            fractalController->SetJulia1Iterations(julia1_iterations);

            julia1_offset = j["Julia1"]["Offset"];
            fractalController->SetJulia1Offset(julia1_offset);

            julia1_smoothness = j["Julia1"]["Smoothness"];
            fractalController->SetJulia1Smoothness(julia1_smoothness);

			break;
		}
		case FractalType::Julia2: {
            julia2_iterations = j["Julia2"]["Iterations"];
            fractalController->SetJulia2Iterations(julia2_iterations);

            std::vector<float> julia2_offset_vector = j["Julia2"]["Offset"].get<std::vector<float>>();
            memcpy(julia2_offset, julia2_offset_vector.data(), 3);
            fractalController->SetJulia2Offset(glm::make_vec3(julia2_offset_vector.data()));

            julia2_smoothness = j["Julia2"]["Smoothness"];
            fractalController->SetJulia2Smoothness(julia2_smoothness);

            julia2_w = j["Julia2"]["W"];
            fractalController->SetJulia2W(julia2_w);

			break;
		}
		case FractalType::Julia3: {
            julia3_iterations = j["Julia3"]["Iterations"];
            fractalController->SetJulia3Iterations(julia3_iterations);

            julia3_offset = j["Julia3"]["Offset"];
            fractalController->SetJulia3Offset(julia3_offset);

            julia3_smoothness = j["Julia3"]["Smoothness"];
            fractalController->SetJulia3Smoothness(julia3_smoothness);

			break;
		}
		case FractalType::Julia4: {
            julia4_iterations = j["Julia4"]["Iterations"];
            fractalController->SetJulia4Iterations(julia4_iterations);

            std::vector<float> julia4_offset_vector = j["Julia4"]["Offset"].get<std::vector<float>>();
            memcpy(julia4_offset, julia4_offset_vector.data(), 3);
            fractalController->SetJulia4Offset(glm::make_vec3(julia4_offset_vector.data()));

            julia4_smoothness = j["Julia4"]["Smoothness"];
            fractalController->SetJulia4Smoothness(julia4_smoothness);

            julia4_w = j["Julia4"]["W"];
            fractalController->SetJulia4W(julia4_w);

			break;
		}
		case FractalType::Sierpinski1: {
            std::vector<float> sierpinski1_va_vector = j["Sierpinski1"]["Vector1"].get<std::vector<float>>();
            memcpy(sierpinski1_va, sierpinski1_va_vector.data(), 3);
            fractalController->SetSierpinski1Vector1(glm::make_vec3(sierpinski1_va_vector.data()));
			
            std::vector<float> sierpinski1_vb_vector = j["Sierpinski1"]["Vector2"].get<std::vector<float>>();
            memcpy(sierpinski1_vb, sierpinski1_vb_vector.data(), 3);
            fractalController->SetSierpinski1Vector2(glm::make_vec3(sierpinski1_vb_vector.data()));
			
            std::vector<float> sierpinski1_vc_vector = j["Sierpinski1"]["Vector3"].get<std::vector<float>>();
            memcpy(sierpinski1_vc, sierpinski1_vc_vector.data(), 3);
            fractalController->SetSierpinski1Vector3(glm::make_vec3(sierpinski1_vc_vector.data()));
			
            std::vector<float> sierpinski1_vd_vector = j["Sierpinski1"]["Vector4"].get<std::vector<float>>();
            memcpy(sierpinski1_vd, sierpinski1_vd_vector.data(), 3);
            fractalController->SetSierpinski1Vector4(glm::make_vec3(sierpinski1_vd_vector.data()));
			
            sierpinski1_iterations = j["Sierpinski1"]["Iterations"];
            fractalController->SetSierpinski1Iterations(sierpinski1_iterations);

			break;
		}
		case FractalType::Sierpinski2: {
            std::vector<float> sierpinski2_va_vector = j["Sierpinski2"]["Vector1"].get<std::vector<float>>();
            memcpy(sierpinski2_va, sierpinski2_va_vector.data(), 3);
            fractalController->SetSierpinski2Vector1(glm::make_vec3(sierpinski2_va_vector.data()));
			
            std::vector<float> sierpinski2_vb_vector = j["Sierpinski2"]["Vector2"].get<std::vector<float>>();
            memcpy(sierpinski2_vb, sierpinski2_vb_vector.data(), 3);
            fractalController->SetSierpinski2Vector2(glm::make_vec3(sierpinski2_vb_vector.data()));
			
            std::vector<float> sierpinski2_vc_vector = j["Sierpinski2"]["Vector3"].get<std::vector<float>>();
            memcpy(sierpinski2_vc, sierpinski2_vc_vector.data(), 3);
            fractalController->SetSierpinski2Vector3(glm::make_vec3(sierpinski2_vc_vector.data()));
			
            sierpinski2_iterations = j["Sierpinski2"]["Iterations"];
            fractalController->SetSierpinski2Iterations(sierpinski2_iterations);

			break;
		}
		case FractalType::MengerSponge1: {
            menger_sponge1_offset1 = j["MengerSponge1"]["Offset1"];
            fractalController->SetMengerSponge1Offset1(menger_sponge1_offset1);

            menger_sponge1_offset2 = j["MengerSponge1"]["Offset2"];
            fractalController->SetMengerSponge1Offset2(menger_sponge1_offset2);

            menger_sponge1_iterations = j["MengerSponge1"]["Iterations"];
            fractalController->SetMengerSponge1Iterations(menger_sponge1_iterations);

			break;
		}
		case FractalType::MengerSponge2: {
            menger_sponge2_offset1 = j["MengerSponge2"]["Offset1"];
            fractalController->SetMengerSponge2Offset1(menger_sponge2_offset1);

            menger_sponge2_offset2 = j["MengerSponge2"]["Offset2"];
            fractalController->SetMengerSponge2Offset2(menger_sponge2_offset2);

            menger_sponge2_iterations = j["MengerSponge2"]["Iterations"];
            fractalController->SetMengerSponge2Iterations(menger_sponge2_iterations);

			break;
		}
		case FractalType::MengerSponge3: {
            current_menger_sponge3_type = j["MengerSponge3"]["Type"];
            fractalController->SetMengerSponge3Type(current_menger_sponge3_type);
			break;
		}
		case FractalType::Apollonian1: {
            apollonian1_offset1 = j["Apollonian1"]["Offset1"];
            fractalController->SetApollonian1Offset1(apollonian1_offset1);

            apollonian1_offset2 = j["Apollonian1"]["Offset2"];
            fractalController->SetApollonian1Offset2(apollonian1_offset2);

            apollonian1_iterations = j["Apollonian1"]["Iterations"];
            fractalController->SetApollonian1Iterations(apollonian1_iterations);

			break;
		}
		case FractalType::Apollonian2: {
            apollonian2_offset1 = j["Apollonian2"]["Offset1"];
            fractalController->SetApollonian2Offset1(apollonian2_offset1);

            apollonian2_offset2 = j["Apollonian2"]["Offset2"];
            fractalController->SetApollonian2Offset2(apollonian2_offset2);

            apollonian2_iterations = j["Apollonian2"]["Iterations"];
            fractalController->SetApollonian2Iterations(apollonian2_iterations);

			break;
		}
		case FractalType::Apollonian3: {
            apollonian3_offset1 = j["Apollonian3"]["Offset1"];
            fractalController->SetApollonian3Offset1(apollonian3_offset1);

            apollonian3_offset2 = j["Apollonian3"]["Offset2"];
            fractalController->SetApollonian3Offset2(apollonian3_offset2);

            apollonian3_offset3 = j["Apollonian3"]["Offset3"];
            fractalController->SetApollonian3Offset3(apollonian3_offset3);

            apollonian3_iterations = j["Apollonian3"]["Iterations"];
            fractalController->SetApollonian3Iterations(apollonian3_iterations);

            std::vector<float> apollonian3_csize_vector = j["Apollonian3"]["CSize"].get<std::vector<float>>();
            memcpy(apollonian3_csize, apollonian3_csize_vector.data(), 3);
            fractalController->SetApollonian3Csize(glm::make_vec3(apollonian3_csize_vector.data()));
			
			break;
		}
	}
    //--------------------------------------------------------------------------

    fractalController->LoadShaderProgram();
}

void Gui::SaveParameters() {    
    if (saveWindowFlag) {
        ImGuiWindowFlags saveWindowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;
        int width, height;
        window->GetSize(&width, &height);
        ImGui::SetNextWindowPos(ImVec2(0.10f*width, 0.15f*height));
        ImVec2 windowSize = ImVec2(0.65f*width, 0.10f*height);
	    ImGui::SetNextWindowSize(windowSize);
        ImGui::Begin("Save parameters", &saveWindowFlag, saveWindowFlags);
    
        if (ImGui::Button("File name:")) {
            fileBrowserSaveParameters.Open();
        }
        ImGui::SameLine();
        std::string t = save_path + save_name;
        ImGui::Text("%s", t.c_str());

        if (ImGui::Button("Save")) {
            SaveParameters(t.c_str());
            saveWindowFlag = false;
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            saveWindowFlag = false;
        }
        ImGui::End();
    }
}

void Gui::LoadParameters() {    
    if (loadWindowFlag) {
        ImGuiWindowFlags loadWindowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;
        int width, height;
        window->GetSize(&width, &height);
        ImGui::SetNextWindowPos(ImVec2(0.10f*width, 0.15f*height));
        ImVec2 windowSize = ImVec2(0.65f*width, 0.10f*height);
	    ImGui::SetNextWindowSize(windowSize);
        ImGui::Begin("Load parameters", &loadWindowFlag, loadWindowFlags);

        if (ImGui::Button("File name:")) {
            fileBrowserLoadParameters.Open();
        }
        ImGui::SameLine();
        std::string t = load_path + load_name;
        ImGui::Text("%s", t.c_str());

        if (ImGui::Button("Load")) {
            LoadParameters(t.c_str());
            loadWindowFlag = false;
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            loadWindowFlag = false;
        }
        ImGui::End();
    }
}


void Gui::Update() {
    // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        if ( (InputSystem::keys[GLFW_KEY_LEFT_ALT] || InputSystem::keys[GLFW_KEY_RIGHT_ALT]) && InputSystem::keys[GLFW_KEY_F4]) {
            window->Close();
        }
        

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        MenuBar();
        Preview();
        Stats();
        MainParameters();
        FractalParameters();
        ExportAs();
        SaveParameters();
        LoadParameters();
        
        FileBrowserExport();
        FileBrowserSaveParameters();
        FileBrowserLoadParameters();
        FileBrowserSetupSkybox();
        FileBrowserSetupSkyboxHDR();

		// Rendering
        ImGui::Render();
        int display_w, display_h;
        GLCall(glfwGetFramebufferSize(window->GetPointer(), &display_w, &display_h));
        GLCall(glViewport(0, 0, display_w, display_h));
        GLCall(glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::Destroy() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}