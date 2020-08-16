#include <iostream>
#include <string>

#include "gui.h"

int main() {
    Window window;
 	window.SetWidth(1080);
 	window.SetHeight(768);
 	window.SetName("Fractal3D");
 	window.Init();

	Renderer renderer;
 	renderer.SetWidth(window.GetWidth());
 	renderer.SetHeight(window.GetHeight());
 	renderer.Init();

	Camera camera;
 	double currentTime = 0.0;
 	double lastTime = 0.0; // Время вывода последнего кадра
	renderer.SetActiveCamera(&camera);

	FractalController fractalController;
	fractalController.Init(&renderer);

	Gui gui;
	gui.Init(&window, &fractalController);

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