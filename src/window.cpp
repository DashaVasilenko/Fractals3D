#include "window.h"




int Window::Init() {
    // init GLFW
    if (!glfwInit())
        exit(EXIT_FAILURE);

 	// Window creation + GL context
 	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
 	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
 	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
 	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Mac OS build fix
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X

    window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // context
 	glfwMakeContextCurrent(window);

    #ifndef __APPLE__
        // Glew init
 	    glewExperimental = true; 
 	    if (glewInit() != GLEW_OK) { glfwTerminate(); return -1; }
    #endif
    
    return 0;
 }

void Window::Destroy() {
    glfwDestroyWindow(window);
    glfwTerminate();
}
