#ifndef __WINDOW__
#define __WINDOW__

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#ifndef __APPLE__
    #include <GL/glew.h>
    #define GLEW_STATIC
#else
    #define GL_SILENCE_DEPRECATION
    #include <OpenGL/gl3.h>
#endif

#include <stdlib.h>
#include <string>

class Window {
public:
    void SetWidth(int width) { this->width = width; }
    void SetHeight(int height) { this->height = height; }
    void SetName(const std::string& name) { this->name = name; }

    GLFWwindow* GetPointer() { return this->window; }
    int GetWidth() { return this->width; }
    int GetHeight() { return this->height; }

    int Init();
    void Destroy();

private:
    GLFWwindow* window; 
    int width = 640;
    int height = 480;
    std::string name = "Window";
};

#endif /* End of __WINDOW__ */ 