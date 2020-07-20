#include <iostream>
#include <string>

#include "window.h"
#include "renderer.h"
#include "camera.h"
#include "gui.h"

int main() {
    Window window;
 	window.SetWidth(1080);
 	window.SetHeight(768);
 	window.SetName("Karl's window!");
 	window.Init();

	Renderer renderer;
 	renderer.SetWidth(window.GetWidth());
 	renderer.SetHeight(window.GetHeight());
 	renderer.Init();

	Camera camera;
 	double currentTime = 0.0;
 	double lastTime = 0.0; // Время вывода последнего кадра
	renderer.SetActiveCamera(&camera);

	Gui gui;
	gui.Init(&window, &renderer);

 	while (!glfwWindowShouldClose(window.GetPointer())) {
		glfwPollEvents(); // проверяет события (ввод с клавиатуры, перемещение мыши) и вызывает функции обратного вызова(callback))
		currentTime = glfwGetTime();
 		float deltaTime = currentTime - lastTime; // Время, прошедшее между последним и текущим кадром
 		lastTime = currentTime;
		camera.SetDeltaTime(deltaTime); 
		
		renderer.Render(window.GetWidth(), window.GetHeight());
		gui.Update();

 		glfwSwapBuffers(window.GetPointer());
 	}

 	gui.Destroy();
	renderer.Destroy();
 	window.Destroy();
 	return 0;
}