#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "gui.h"
//#include "window.h"

void Gui::Init(Window* window, Renderer* renderer) {
    this->window = window;
    this->renderer = renderer;
    this->fbo = renderer->GetFBO();
    this->camera = renderer->GetCamera();

    //this->fbo = fbo;
    //this->camera = camera;

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
    std::vector<const char*> image_filter = { ".png", ".jpg", ".jpeg", ".bmp", ".tga", ".hdr", ".obj" };
    fileBrowserSaveImage.SetTypeFilters(image_filter);
 
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
                if (ImGui::MenuItem("Load Parameters", "Ctrl+P")) {

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
                        isExportPNG = true;
                        //fileBrowserSaveImage.Open();

                    /*
                        fbo->Bind();
                        unsigned char* imageData = (unsigned char*)malloc((int)(1080*768*(3)));
		                glReadPixels(0, 0, 1080, 768, GL_RGB, GL_UNSIGNED_BYTE, imageData);
                        unsigned char* imageData2 = (unsigned char*)malloc((int)(1080*768*(3)));
                        int k = 0;
                        for (int i = 768; i > 0; i--) {
                            for (int j = 0; j < 1080*3; j++) {
                                imageData2[k] = imageData[i*1080*3 + j];
                                k++;
                            }
                        }
		                // stbi_write_png("stbpng.png", width, height, CHANNEL_NUM, pixels, width * CHANNEL_NUM);
		                stbi_write_png("stbpng.png", 1080, 768, 3, imageData2, 1080 * 3);
		                free(imageData);
                        free(imageData2);
                        fbo->Unbind();
                    */
                    }
                    if (ImGui::MenuItem("BMP")){
                        
                    }
                    if (ImGui::MenuItem("JPEG")){

                    }
                    if (ImGui::MenuItem("JPG")){

                    }
                    if (ImGui::MenuItem("TGA")){

                    }
                    if (ImGui::MenuItem("HDR")){

                    }
                    if (ImGui::MenuItem("OBJ")){

                    }
                    ImGui::EndMenu();
                }
                ImGui::Separator();

                if (ImGui::MenuItem("Exit", "Alt+F4")) {
                    //window.Close();
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
    ImGui::SetNextWindowPos(ImVec2(0, 18));
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


void Gui::Update() {
    // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

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


        if (isExportPNG) {
            ImGui::Begin("Export PNG", &isExportPNG, ImGuiWindowFlags_NoCollapse); 
            int width, height;
            window->GetSize(&width, &height);
            ImGui::SetNextWindowPos(ImVec2(0.25f*width, 0.25f*height));
            ImVec2 windowSize = ImVec2(0.75f*width, 0.75f*height);
	        ImGui::SetNextWindowSize(windowSize);

            if (ImGui::Button("Path:")) {
                fileBrowserSaveImage.Open();
            }
            ImGui::SameLine();
            ImGui::Text((output_path + output_name_png).c_str());

            ImGui::Text("Width:");
            ImGui::SetNextItemWidth(200);
            ImGui::SameLine();
            ImGui::DragInt("##width", &output_width, 1.0, 120, 8640);
            ImGui::SameLine();
            ImGui::Text("(120..8640)");
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 50.0f);
            ImGui::Text("Height:");
            ImGui::SetNextItemWidth(200);
            ImGui::SameLine();
            ImGui::DragInt("##height", &output_height, 1.0, 120, 8640);
            ImGui::SameLine();
            ImGui::Text("(120..8640)");

            if (ImGui::Button("Save")) {
                fbo->Bind();
                unsigned char* imageData = (unsigned char*)malloc((int)(1080*768*(3)));
		        glReadPixels(0, 0, 1080, 768, GL_RGB, GL_UNSIGNED_BYTE, imageData);
                unsigned char* imageData2 = (unsigned char*)malloc((int)(1080*768*(3)));
                int k = 0;
                for (int i = 768; i > 0; i--) {
                    for (int j = 0; j < 1080*3; j++) {
                        imageData2[k] = imageData[i*1080*3 + j];
                        k++;
                    }
                }
		        // stbi_write_png("stbpng.png", width, height, CHANNEL_NUM, pixels, width * CHANNEL_NUM);
		        stbi_write_png("stbpng.png", 1080, 768, 3, imageData2, 1080 * 3);
		        free(imageData);
                free(imageData2);
                fbo->Unbind();

                /*
                int height, width;
                height = fbo->GetHeight();
                width = fbo->GetWidth();
                fbo->Resize(output_width, output_height);
                fbo->Bind();

                unsigned char* imageData = (unsigned char*)malloc((int)(output_width*output_height*(3)));
		        glReadPixels(0, 0, output_width, output_height, GL_RGB, GL_UNSIGNED_BYTE, imageData);
                unsigned char* imageData2 = (unsigned char*)malloc((int)(output_width*output_height*(3)));
                int k = 0;
                for (int i = output_height; i > 0; i--) {
                    for (int j = 0; j < output_width*3; j++) {
                        imageData2[k] = imageData[i*1080*3 + j];
                        k++;
                    }
                }
		        // stbi_write_png("stbpng.png", width, height, CHANNEL_NUM, pixels, width * CHANNEL_NUM);
		        stbi_write_png("stbpng.png", output_width, output_height, 3, imageData2, output_width * 3);
		        free(imageData);
                free(imageData2);
                fbo->Unbind();
                fbo->Resize(width, height);
                */
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
                isExportPNG = false;
            ImGui::End();
        }

//---------------------------------Save as PNG---------------------------------------------
        fileBrowserSaveImage.Display();
        if(fileBrowserSaveImage.HasSelected())
        {
            output_path = fileBrowserSaveImage.GetSelected().parent_path().u8string() + "/";
            output_name_png = fileBrowserSaveImage.GetSelected().filename().u8string();
            if (isExportPNG) {
                int i = output_name_png.length();
                bool flag = true;
                if (i < 4) flag = false;
                if (flag && output_name_png.compare(output_name_png.length() - 4, 4, ".png") != 0)
                    flag = false;
                if (!flag)
                    output_name_png = output_name_png + ".png";

            }
            fileBrowserSaveImage.ClearSelected();
            fileBrowserSaveImage.Close();
        }
//-----------------------------------------------------------------------------------------


		// Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window->GetPointer(), &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::Destroy() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
