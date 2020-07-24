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
	GLCall(glViewport(0, 0, width, height));
	mapSources[GL_VERTEX_SHADER] = "glsl/quad_vertex.glsl";
 	mapSources[GL_FRAGMENT_SHADER] = "glsl/quad_pixel.glsl";
	program.Init(mapSources);
	program.Load();
 	program.Compile();
 	program.Link();
	program.DeleteShaders();
	currentFractalType = program.GetFractalType();

	GLCall(glGenVertexArrays(1, &VAO));
 	GLCall(glBindVertexArray(VAO)); 

	GLCall(glGenBuffers(1, &VBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));  // привязываем буфер
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW)); // копирования вершинных данных в этот буфер

	GLCall(glGenBuffers(1, &IBO));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO)); 
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

	FBO.BufferInit(width, height);

	// сообщаем OpenGL как он должен интерпретировать вершинные данные
 	// (какой аргумент шейдера мы хотим настроить(layout (location = 0)), размер аргумента в шейдере, тип данных,
 	//  необходимость нормализовать входные данные, расстояние между наборами данных, смещение начала данных в буфере)
 	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0));
 	GLCall(glEnableVertexAttribArray(0)); // включаем атрибуты, т.е. передаем вершинному атрибуту позицию аргумента
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT,GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))));
	GLCall(glEnableVertexAttribArray(1)); 

	std::array<std::string, 6> fileNames = { "textures/skybox1/front.tga",
 											 "textures/skybox1/back.tga",
 											 "textures/skybox1/up.tga",

 											 "textures/skybox1/down.tga",
 											 "textures/skybox1/right.tga",
 											 "textures/skybox1/left.tga"
 											 };

 	skyBox.Load(fileNames);

	program.Run();
	program.SetUniform("skyBox", 0);
}

void Renderer::Render(int width, int height) {
	GLCall(glViewport(0, 0, width, height));
	if (InputSystem::isWindowSizeChange) {
		FBO.Resize(width, height);
		this->width = width;
		this->height = height;
	}
	
	FBO.Bind();
	glm::mat4 view = camera->GetViewMatrix();
	float fov = camera->GetFieldOfView();
	Update();

	program.Run();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox.GetDescriptor());

	program.SetUniform("View", view);
	program.SetUniform("iResolution", glm::vec2(width, height));
	program.SetUniform("fieldOfView", fov);

	program.SetUniform("lightDirection", fractalsParameters.light_direction);
	program.SetUniform("ambientLightColor", fractalsParameters.ambient_light_color);
	program.SetUniform("diffuseLightColor", fractalsParameters.diffuse_light_color);
	program.SetUniform("specularLightColor", fractalsParameters.specular_light_color);

	program.SetUniform("ambientColor", fractalsParameters.ambient_fractal_color);
	program.SetUniform("diffuseColor", fractalsParameters.diffuse_fractal_color);
	program.SetUniform("specularColor", fractalsParameters.specular_fractal_color);
	program.SetUniform("shininess", fractalsParameters.shininess);
	program.SetUniform("reflection", fractalsParameters.reflection);

	switch(currentFractalType) {
        case FractalType::Test: {
            
            break;
        }
        case FractalType::Mandelbulb: {
			program.SetUniform("Iterations", fractalsParameters.mandelbulb_iterations);
			program.SetUniform("Bailout", fractalsParameters.mandelbulb_bailout);
			program.SetUniform("Power", fractalsParameters.mandelbulb_power);
            break;
        }
    }

	//program.SetUniform("iResolution", glm::vec2(width, height));
	//program.SetUniform("fieldOfView", fov);
	//program.SetUniform("View", view);

	program.SetUniform("Time", (float)glfwGetTime());

	GLCall(glBindVertexArray(VAO));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO)); 
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL));
	FBO.Unbind();
}

void Renderer::Update() {
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
 	GLCall(glClearColor(0.0f, 0.0f, 1.0f, 1.0f));
}

void Renderer::Destroy() {
	program.Delete();
}