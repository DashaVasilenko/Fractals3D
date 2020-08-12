#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

//#include <iostream>

#include "gui.h"
#include "inputSystem.h"
#include "errors.h"

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
    //std::vector<const char*> image_filter = { ".png", ".jpg", ".jpeg", ".bmp", ".tga", ".hdr", ".obj" };
    //fileBrowserSaveImage.SetTypeFilters(image_filter);
 
}

void Gui::MenuBar() {
    if(ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
            //if (ImGui::MenuItem("Open", "Ctrl + O") || Input::GetKey(KeyCode::LeftCtrl)) {
                //m_FileExplorerLoadConfig.Open();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("New", "Ctrl+N")) {
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Save Parameters", "Ctrl+P")) {
            }
            if (ImGui::MenuItem("Load Parameters", "Ctrl+Shift+P")) {
            }
            ImGui::Separator();
            
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                //m_FileExplorerSaveConfig.Open();
            }
            if (ImGui::MenuItem("Save as..")) {
                //m_FileExplorerSaveConfig.Open();
            }
            ImGui::Separator();
            //if (ImGui::MenuItem("Export..")) {
                //m_FileExplorerSaveConfig.Open();
            //}
            
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
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                window->Close();
            }
            
            ImGui::EndMenu();
        }

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

        if (ImGui::BeginMenu("About")) {
            ImGui::Text("Fractals");
            ImGui::Separator();
            ImGui::Text("Was created as a part of PV097 Visual creativity informatics course");
            ImGui::Text("By Daria Vasilenko");
            ImGui::EndMenu();
        }
        /*
        if (ImGui::BeginMenu("Render")) {
            if (ImGui::MenuItem("Render Image", "F5")) {
                m_RenderSettingsVisible = true;
                s32 width;
                s32 height;
                window.GetSize(&width, &height);
                ImGui::SetNextWindowPos(ImVec2(width*0.5f, height*0.5f));
            }
            ImGui::EndMenu();
        }
        */
        ImGui::EndMainMenuBar();
    }
}

void Gui::Preview() {
    ImGui::SetNextWindowPos(ImVec2(0.0f, 18.0f));
    int width, height;
    window->GetSize(&width, &height);
    ImVec2 previewSize = ImVec2((float)width*2/3, (float)(height - 168));
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
    ImVec2 parametersSize = ImVec2((float)width*2/3, 150.0f);
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
        case FractalType::Monster: {
            ImGui::Text("Current fractal type: Monster fractal");
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
    }

    ImGui::End();
}

void Gui::MainParameters() {
    int width, height;
    window->GetSize(&width, &height);
    ImGui::SetNextWindowPos(ImVec2((float)width*2/3, 18.0f));
    ImVec2 parametersSize = ImVec2((float)width*1/3, (float)(height*2/3 + 25.0));
	ImGui::SetNextWindowSize(parametersSize);
    
    ImGui::Begin("Parameters", NULL, parametersWindowFlags); 
    ShaderProgram* program = fractalController->GetShaderProgram(); 
    bool flag = false;

    //-----------------------------Shadows-------------------------------
    if (ImGui::Checkbox("Soft shadows", &soft_shadows)) { 
        flag = true; 
    }
    if (MyDragFloat("Shadow strength", &shadow_strength, 1, 1, 128)) {
        fractalController->SetShadowStrength(shadow_strength);
    }
    //-------------------------------------------------------------------

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
    if (MyDragFloat("Intensity1", &light_intensity1, 0.5, 0, 30)) {
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
    if (MyDragFloat("Intensity2", &light_intensity2, 0.5, 0, 30)) {
        fractalController->SetLightIntensity2(light_intensity2);
    }
    ImGui::Separator();

    ImGui::Text("Ambient light:");
    if (ImGui::ColorEdit3("Color3", ambient_fractal_light_color)) {
        fractalController->SetAmbientFractalLightColor(glm::vec3(ambient_fractal_light_color[0], ambient_fractal_light_color[1], ambient_fractal_light_color[2]));
    }
    if (MyDragFloat("Intensity3", &ambient_light_intensity, 0.5, 0, 30)) {
        fractalController->SetAmbientFractalLightIntensity(ambient_light_intensity);
    }
    //-------------------------------------------------------------------

    //---------------------Background parameters-------------------------
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
    }
    //-------------------------------------------------------------------
    
    if (flag) {
        program->SetShaderParameters(soft_shadows);
        fractalController->LoadShaderProgram();
    }

    ImGui::End();
}

void Gui::FractalParameters() {
    int width, height;
    window->GetSize(&width, &height);
    ImGui::SetNextWindowPos(ImVec2((float)width*2/3, (float)(height*2/3) + 43.0f));
    ImVec2 parametersSize = ImVec2((float)width*1/3, (float)(height*1/3) - 43.0f);
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
        case FractalType::Monster: {

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
    }

    if (static_cast<ColoringType>(currentColoringType) == ColoringType::Type4) {
        if (ImGui::ColorEdit3("Type4 color", type4_color)) {
            fractalController->SetType4Color(glm::vec3(type4_color[0], type4_color[1], type4_color[2]));
        }
    }

    if (static_cast<ColoringType>(currentColoringType) == ColoringType::Type5) {
        if (ImGui::ColorEdit3("Type5 color", type5_color)) {
            fractalController->SetType5Color(glm::vec3(type5_color[0], type5_color[1], type5_color[2]));
        }
    }

    if (static_cast<ColoringType>(currentColoringType) == ColoringType::Type6) {
        if (ImGui::ColorEdit3("Type6 color", type6_color)) {
            fractalController->SetType6Color(glm::vec3(type6_color[0], type6_color[1], type6_color[2]));
        }
    }

    if (MyDragFloat("Shininess", &shininess, 1, 1, 100)) {
        fractalController->SetFractalShininess(shininess);
    }
    if (MyDragFloat("Reflection", &reflection, 0.1, 0, 1)) {
        fractalController->SetFractalReflect(reflection);
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

    if (MyDragInt("Iterations", &mandelbulb_iterations, 1, 1, 15)) {
        fractalController->SetMandelbulbIterations(mandelbulb_iterations);
    }

    if (MyDragFloat("Bailout", &mandelbulb_bailout, 1, 1, 30)) {
        fractalController->SetMandelbulbBailout(mandelbulb_bailout);
    }

    if (MyDragFloat("Power", &mandelbulb_power, 1, 1, 30)) {
        fractalController->SetMandelbulbPower(mandelbulb_power);
    }

    ImGui::End();
}

void Gui::Julia1() {
    ImGui::Begin("Julia1 parameters", NULL, parametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (MyDragFloat("Julia1 offset", &julia1_offset, 0.1, 0, 100)) {
        fractalController->SetJulia1Offset(julia1_offset);
    }
    if (MyDragFloat("Julia1 smoothness", &julia1_smoothness, 0.1, 0, 45)) {
        fractalController->SetJulia1Smoothness(julia1_smoothness);
    }
    ImGui::End();
}

void Gui::Julia2() {
    ImGui::Begin("Julia2 parameters", NULL, parametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (MyDragFloat3("Julia2 offset", julia2_offset, 0.1, 0, 100)) {
        fractalController->SetJulia2Offset(glm::vec3(julia2_offset[0], julia2_offset[1], julia2_offset[2]));
    }
    if (MyDragFloat("Julia2 W", &julia2_w, 0.1, 0, 100)) {
        fractalController->SetJulia2W(julia2_w);
    }
    if (MyDragFloat("Julia2 smoothness", &julia2_smoothness, 0.1, 0, 45)) {
        fractalController->SetJulia2Smoothness(julia2_smoothness);
    }
    ImGui::End();
}

void Gui::Julia3() {
    ImGui::Begin("Julia3 parameters", NULL, parametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (MyDragFloat("Julia3 offset", &julia3_offset, 0.1, 0, 100)) {
        fractalController->SetJulia3Offset(julia3_offset);
    }
    if (MyDragFloat("Julia3 smoothness", &julia3_smoothness, 0.1, 0, 100)) {
        fractalController->SetJulia3Smoothness(julia3_smoothness);
    }
    ImGui::End();
}

void Gui::Julia4() {
    ImGui::Begin("Julia4 parameters", NULL, parametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (MyDragFloat3("Julia4 offset", julia4_offset, 0.1, -100, 100)) {
        fractalController->SetJulia4Offset(glm::vec3(julia4_offset[0], julia4_offset[1], julia4_offset[2]));
    }
    if (MyDragFloat("Julia4 W", &julia4_w, 0.1, -100, 100)) {
        fractalController->SetJulia4W(julia4_w);
    }
    if (MyDragFloat("Julia4 smoothness", &julia4_smoothness, 0.1, 0, 45)) {
        fractalController->SetJulia4Smoothness(julia4_smoothness);
    }
    ImGui::End();
}

void Gui::Sierpinski1() {
    ImGui::Begin("Sierpinski1 parameters", NULL, parametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (MyDragFloat3("Sierpinski1 v1", sierpinski1_va, 0.1, -100, 100)) {
        fractalController->SetSierpinski1Vector1(glm::vec3(sierpinski1_va[0], sierpinski1_va[1], sierpinski1_va[2]));
    }
    if (MyDragFloat3("Sierpinski1 v2", sierpinski1_vb, 0.1, -100, 100)) {
        fractalController->SetSierpinski1Vector2(glm::vec3(sierpinski1_vb[0], sierpinski1_vb[1], sierpinski1_vb[2]));
    }
    if (MyDragFloat3("Sierpinski1 v3", sierpinski1_vc, 0.1, -100, 100)) {
        fractalController->SetSierpinski1Vector3(glm::vec3(sierpinski1_vc[0], sierpinski1_vc[1], sierpinski1_vc[2]));
    }
    if (MyDragFloat3("Sierpinski1 v4", sierpinski1_vd, 0.1, -100, 100)) {
        fractalController->SetSierpinski1Vector4(glm::vec3(sierpinski1_vd[0], sierpinski1_vd[1], sierpinski1_vd[2]));
    }
    
    ImGui::End();
}

void Gui::Sierpinski2() {
    ImGui::Begin("Sierpinski2 parameters", NULL, parametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (MyDragFloat3("Sierpinski2 v1", sierpinski2_va, 0.1, -100, 100)) {
        fractalController->SetSierpinski2Vector1(glm::vec3(sierpinski2_va[0], sierpinski2_va[1], sierpinski2_va[2]));
    }
    if (MyDragFloat3("Sierpinski2 v2", sierpinski2_vb, 0.1, -100, 100)) {
        fractalController->SetSierpinski2Vector2(glm::vec3(sierpinski2_vb[0], sierpinski2_vb[1], sierpinski2_vb[2]));
    }
    if (MyDragFloat3("Sierpinski2 v3", sierpinski2_vc, 0.1, -100, 100)) {
        fractalController->SetSierpinski2Vector3(glm::vec3(sierpinski2_vc[0], sierpinski2_vc[1], sierpinski2_vc[2]));
    }
    if (MyDragFloat3("Sierpinski2 v4", sierpinski2_vd, 0.1, -100, 100)) {
        fractalController->SetSierpinski2Vector4(glm::vec3(sierpinski2_vd[0], sierpinski2_vd[1], sierpinski2_vd[2]));
    }
    
    ImGui::End();
}

void Gui::MengerSponge1() {
    ImGui::Begin("MengerSponge1 parameters", NULL, parametersWindowFlags); 
    FractalColor();
    ImGui::Separator();

    if (MyDragFloat("MengerSponge1 offset1", &menger_sponge1_offset1, 0.1, -30, 30)) {
        fractalController->SetMengerSponge1Offset1(menger_sponge1_offset1);
    }
    if (MyDragFloat("MengerSponge1 ofset2", &menger_sponge1_offset2, 0.1, -30, 30)) {
        fractalController->SetMengerSponge1Offset2(menger_sponge1_offset2);
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

        if (ImGui::Button("Save")) {
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

            //printf("Hey\n");
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
        
        FileBrowserExport();
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
