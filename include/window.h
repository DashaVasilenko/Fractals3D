#ifndef __WINDOW__
#define __WINDOW__

#include <GL/glew.h>
#define GLEW_STATIC


#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


#include <stdlib.h>
#include <string>

class Window {
public:
    void SetWidth(int width) { this->width = width; }
    void SetHeight(int height) { this->height = height; }
    void SetName(const std::string& name) { this->name = name; }

    GLFWwindow* GetPointer() { return this->window; }
    int GetWidth() const { return this->width; }
    int GetHeight() const { return this->height; }

    int Init();
    void Destroy();

    static void OnKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mode);
    static void OnMouseMove(GLFWwindow* window, double xpos, double ypos);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);


private:
    GLFWwindow* window; 
    int width = 640;
    int height = 480;
    std::string name = "Window";
};

#endif /* End of __WINDOW__ */ 