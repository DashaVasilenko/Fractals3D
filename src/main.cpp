#include <iostream>
#include <string>

#ifdef _WIN32
	#include <Windows.h>
#endif

#if defined(__APPLE__) && defined(__MACH__)
	#include <libgen.h>
	#include <mach-o/dyld.h>
#endif

#include <filesystem>

#include "gui.h"

#ifdef _WIN32
int WINAPI WinMain (HINSTANCE, HINSTANCE, PSTR, int) {
#else
int main() {
#endif
	std::filesystem::path path_to_exe;
#ifdef _WIN32
	char exe_path[MAX_PATH];
    HMODULE hModule = GetModuleHandle(NULL);
    GetModuleFileName(hModule, exe_path, sizeof(exe_path));

    path_to_exe = std::filesystem::path(exe_path).parent_path();
#endif

//#ifdef __OSX__
#if defined(__APPLE__) && defined(__MACH__)
	// Here mac os variant
	char path[1024];
	uint32_t size = sizeof(path);
	if (_NSGetExecutablePath(path, &size) == 0)
	    printf("executable path is %s\n", path);
	else
	    printf("buffer too small; need size %u\n", size);
	path_to_exe = dirname(path);	
	//path_to_exe = "";
#endif

    Window window;
 	window.SetWidth(1080);
 	window.SetHeight(768);
 	window.SetName("Fractal3D");
 	window.Init();

	Renderer renderer;
 	renderer.SetWidth(window.GetWidth());
 	renderer.SetHeight(window.GetHeight());
 	renderer.Init(path_to_exe.u8string() + "/");

	Camera camera;
 	double currentTime = 0.0;
 	double lastTime = 0.0; // Время вывода последнего кадра
	renderer.SetActiveCamera(&camera);

	FractalController fractalController;
	fractalController.Init(&renderer, path_to_exe.u8string() + "/");

	Gui gui;
	gui.Init(&window, &fractalController, path_to_exe.u8string() + "/" );

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