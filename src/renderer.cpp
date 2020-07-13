#include "renderer.h"
#include "inputSystem.h"


const GLfloat Renderer::vertices[20] = {
    -1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f, 1.0f	
	}; 
 
const GLuint Renderer::indices[6] = {
	                	0, 1, 2,
	                	0, 2, 3
	                };

void Renderer::Init() {
	glViewport(0, 0, width, height);
	mapSources[GL_VERTEX_SHADER] = "glsl/quad_vertex.glsl";
 	mapSources[GL_FRAGMENT_SHADER] = "glsl/quad_pixel.glsl";
	program.Init(mapSources);
 	program.Compile();
 	program.Link();

	glGenVertexArrays(1, &VAO);
 	glBindVertexArray(VAO); 

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);  // привязываем буфер
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // копирования вершинных данных в этот буфер

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	FBO.BufferInit(width, height);

	// сообщаем OpenGL как он должен интерпретировать вершинные данные
 	// (какой аргумент шейдера мы хотим настроить(layout (location = 0)), размер аргумента в шейдере, тип данных,
 	//  необходимость нормализовать входные данные, расстояние между наборами данных, смещение начала данных в буфере)
 	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
 	glEnableVertexAttribArray(0); // включаем атрибуты, т.е. передаем вершинному атрибуту позицию аргумента
	glVertexAttribPointer(1, 2, GL_FLOAT,GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1); 
}

void Renderer::Render(int width, int height) {
	if (InputSystem::isWindowSizeChange) {
		FBO.Resize(width, height);
		//this->width = width;
		//this->height = height;
	}
	
	FBO.Bind();
	glm::mat4 view = camera->GetViewMatrix();
	float fov = camera->GetFieldOfView();
	Update();
	program.Run();
	program.SetUniform("iResolution", glm::vec2(width, height));
	program.SetUniform("fieldOfView", fov);
	program.SetUniform("View", view);
	program.SetUniform("Time", glfwGetTime());
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); 
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
	FBO.Unbind();
}

void Renderer::Update() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
}

void Renderer::Destroy() {
	program.Delete();
}