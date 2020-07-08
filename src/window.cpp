#include <glm/glm.hpp>

#include "window.h"
#include "inputSystem.h"

void Window::OnKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (action == GLFW_PRESS) 
        InputSystem::keys[key] = true;
    else if (action == GLFW_RELEASE)
        InputSystem::keys[key] = false;
}

void Window::OnMouseMove(GLFWwindow* window, double xpos, double ypos) {
    if (InputSystem::mouse[RightButton]) {
        if (InputSystem::firstMouseMove) {
            InputSystem::lastCursPosX = xpos;
            InputSystem::lastCursPosY = ypos;
        }
        InputSystem::deltaCursPosX = xpos - InputSystem::lastCursPosX;
        InputSystem::deltaCursPosY = InputSystem::lastCursPosY - ypos;
        InputSystem::lastCursPosX = xpos;
        InputSystem::lastCursPosY = ypos;
        InputSystem::firstMouseMove = false;
    }
    else {
        InputSystem::firstMouseMove = true;
    }
}

void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        InputSystem::mouse[button] = true;
    }
    else if (action == GLFW_RELEASE) {
        InputSystem::mouse[button] = false;
    }
}

void Window::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) { 
    InputSystem::scrollOffsetX = xoffset;
    InputSystem::scrollOffsetY = yoffset;
    //InputSystem::fieldOfView -= yoffset;
    //InputSystem::fieldOfView = glm::clamp(InputSystem::fieldOfView, 1.0f, 45.0f);
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int Window::Init() {
    // init GLFW
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

 	// Window creation + GL context
 	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
 	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
 	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
 	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // выключение возможности изменения размера окна

    // Mac OS build fix
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X

    window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // скрыть курсор мыши  
    glfwSetCursorPosCallback(window, OnMouseMove);
    glfwSetKeyCallback(window, OnKeyPressed); // передача функции для клавиатуры в GLFW
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    // context
 	glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Glew init
 	glewExperimental = true; 
 	if (glewInit() != GLEW_OK) { glfwTerminate(); return -1; }

    return 0;
 }

void Window::Destroy() {
    glfwDestroyWindow(window);
    glfwTerminate();
}