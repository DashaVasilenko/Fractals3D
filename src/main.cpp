#include <iostream>

#include "window.h"

int main() {
    Window window;
 	window.SetWidth(1080);
 	window.SetHeight(768);
 	window.SetName("Karl's window!");
 	window.Init();

 	while (!glfwWindowShouldClose(window.GetPointer())) {
 		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 		glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

 		glfwSwapBuffers(window.GetPointer());
     	glfwPollEvents();
 	}

 	window.Destroy();
 	return 0;
}