#include <iostream>
#include <string>
//#include <fstream>
//#include <streambuf>
#include "window.h"
#include "renderer.h"
#include "shaderProgram.h"
#include "camera.h"

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

	std::map<GLenum, std::string> mapSources;
 	mapSources[GL_VERTEX_SHADER] = "glsl/quad_vertex.glsl";
 	mapSources[GL_FRAGMENT_SHADER] = "glsl/quad_pixel.glsl";

	ShaderProgram program;
 	program.Init(mapSources);
 	program.Compile();
 	program.Link();

	GLfloat vertices[] = {
    -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
     1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f
	};  

	GLuint indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	GLuint VAO; // создаем буфер VAO (vertex array object)
 	glGenVertexArrays(1, &VAO);
 	glBindVertexArray(VAO); // привязываем буфер

	GLuint VBO; // создаем буфер VBO (vertex buffer objects)
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);  // привязываем буфер
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // копирования вершинных данных в этот буфер

	GLuint IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// сообщаем OpenGL как он должен интерпретировать вершинные данные
 	// (какой аргумент шейдера мы хотим настроить(layout (location = 0)), размер аргумента в шейдере, тип данных,
 	//  необходимость нормализовать входные данные, расстояние между наборами данных, смещение начала данных в буфере)
 	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
 	glEnableVertexAttribArray(0); // включаем атрибуты, т.е. передаем вершинному атрибуту позицию аргумента
 	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)) );
 	//glEnableVertexAttribArray(1);


	// в рендерер
 	Camera camera;
 	double currentTime = 0.0;
 	double lastTime = 0.0; // Время вывода последнего кадра
	//glm::mat4 view = camera.GetViewMatrix();
	//std::cout << view[3].x << view[3].y << view[3].z << std::endl; 
	
 	while (!glfwWindowShouldClose(window.GetPointer())) {
		glfwPollEvents(); // проверяет события (ввод с клавиатуры, перемещение мыши) и вызывает функции обратного вызова(callback))
		currentTime = glfwGetTime();
 		float deltaTime = currentTime - lastTime; // Время, прошедшее между последним и текущим кадром
 		lastTime = currentTime;
		camera.Update(deltaTime);
		glm::mat4 view = camera.GetViewMatrix();


		renderer.Update();

		//glm::mat4 view = camera.GetViewMatrix();

		program.Run();
		program.SetUniform("iResolution", glm::vec2(window.GetWidth(), window.GetHeight()));
		program.SetUniform("View", view);

 		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); 
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		//camera.Update(deltaTime);

 		glfwSwapBuffers(window.GetPointer());
     	//glfwPollEvents();
 	}

	program.Delete();
 	window.Destroy();
 	return 0;
}