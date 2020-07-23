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
    }

    ImGui::End();
}

void Gui::MainParameters() {
    int width, height;
    window->GetSize(&width, &height);
    ImGui::SetNextWindowPos(ImVec2((float)width*2/3, 18.0f));
    ImVec2 parametersSize = ImVec2((float)width*1/3, (float)(height*1/3));
	ImGui::SetNextWindowSize(parametersSize);
    
    ImGui::Begin("Parameters", NULL, parametersWindowFlags); 
    ShaderProgram* program = fractalController->GetShaderProgram(); 
    uint32_t shaderParameters = 0; 
    bool flag = false;

    main_parameters_window_color_previous_state = main_parameters_window_color;
    ImGui::Checkbox("Color", &main_parameters_window_color);
    if (main_parameters_window_color != main_parameters_window_color_previous_state) {
        flag = true;
        //fractalController->SetColor(main_parameters_window_color);
        program->SetColor(main_parameters_window_color);
    }

    hard_shadows_previous_state = hard_shadows;
    ImGui::Checkbox("Hard shadows", &hard_shadows);
    if (hard_shadows != hard_shadows_previous_state) {
        flag = true;
    }
    if (hard_shadows) {
        shaderParameters = 1 << 1;
        soft_shadows = false;
    }

    soft_shadows_previous_state = soft_shadows;
    ImGui::Checkbox("Soft shadows", &soft_shadows);
    if (soft_shadows != soft_shadows_previous_state) {
        flag = true;
    }
    if (soft_shadows) {
        shaderParameters = 1 << 2;
        hard_shadows = false;
    }

    ambient_occlusion_previous_state = ambient_occlusion;
    ImGui::Checkbox("Ambient occlusion", &ambient_occlusion);
    if (ambient_occlusion != ambient_occlusion_previous_state) {
        flag = true;
    }
    if (ambient_occlusion) 
        shaderParameters |= 1 << 3;

    if (ImGui::BeginMenu("Type of fractal:")) {
        if (ImGui::MenuItem("Test")) {
            currentFractalType = FractalType::Test;
            fractalController->SetFractalType(currentFractalType);
            flag = true;
        }
        if (ImGui::MenuItem("Mandelbulb Fractal")) {
            currentFractalType = FractalType::Mandelbulb;
            fractalController->SetFractalType(currentFractalType);
            flag = true;
        }
            
            
        ImGui::EndMenu();
    }

    switch(currentFractalType) {
        case FractalType::Test: {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 150.0f);
            ImGui::Text("Test");
            break;
        }
        case FractalType::Mandelbulb: {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 150.0f);
            ImGui::Text("Mandelbulb fractal");
            break;
        }
    }
    
    if (flag) {
        program->SetShaderParameters(shaderParameters);
        fractalController->LoadShaderProgram();
        //program->Load();
    }

    ImGui::End();
}

void Gui::FractalParameters() {
    int width, height;
    window->GetSize(&width, &height);
    ImGui::SetNextWindowPos(ImVec2((float)width*2/3, (float)(height*1/3) + 18.0f));
    ImVec2 parametersSize = ImVec2((float)width*1/3, (float)(height*2/3) - 18.0f);
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
    }
    
}

void Gui::Test() {
    ImGui::Begin("Test parameters", NULL, parametersWindowFlags); 

    ImGui::End();
}

void Gui::Mandelbulb() {
    ImGui::Begin("Mandelbulb parameters", NULL, parametersWindowFlags); 

    ImGui::Text("Iterations:");
    ImGui::SetNextItemWidth(200);
    ImGui::SameLine();
    if (MyDragInt("##mandelbulb_iterations", &mandelbulb_iterations, 1, 1, 15)) {
        fractalController->SetMandelbulbIterations(mandelbulb_iterations);
    }
    ImGui::SameLine();
    ImGui::Text("(1..15)");

    ImGui::Text("Bailout:");
    ImGui::SetNextItemWidth(200);
    ImGui::SameLine();
    if (MyDragFloat("##mandelbulb_bailout", &mandelbulb_bailout, 1, 1, 30)) {
        fractalController->SetMandelbulbBailout(mandelbulb_bailout);
    }
    ImGui::SameLine();
    ImGui::Text("(1..30)");

    ImGui::Text("Power:");
    ImGui::SetNextItemWidth(200);
    ImGui::SameLine();
    if (MyDragFloat("##mandelbulb_power", &mandelbulb_power, 1, 1, 30)) {
        fractalController->SetMandelbulbPower(mandelbulb_power);
    }
    ImGui::SameLine();
    ImGui::Text("(1..30)");

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
        ImGui::Text((output_path + output_name).c_str());

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
