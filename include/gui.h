#ifndef __GUI__
#define __GUI__

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

class Gui {
public:
    void Init(GLFWwindow* window);
    void Update();
    void Destroy();

private:
    GLFWwindow* window;
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


};

#endif /* End of __GUI__ */