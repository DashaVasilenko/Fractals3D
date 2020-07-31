#include "renderer.h"
#include "inputSystem.h"

const GLfloat Renderer::vertices[20] = {  -1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
    									   1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
    									   1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
										  -1.0f, -1.0f, 0.0f, 0.0f, 1.0f	
}; 
 
const GLuint Renderer::indices[6] = {  0, 1, 2,
	                				   0, 2, 3
};

const GLfloat Renderer::cube_vertices[192] = {  // back face
            									-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left 0
            									 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right 1
            									 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right 2        
            									-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left 3
            									// front face
            									-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left 4
            									 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right 5
            									 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right 6
            									-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left 7
            									// left face
            									-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right 8
            									-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left 9
            									-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left 10
            									-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right 11
            									// right face
            									 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left 12
            									 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right 13
            									 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right 14    
             									 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left 15    
            									// bottom face
            									-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right 16
            									 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left 17
            									 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left 18
             									-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right 19
             									// top face
            									-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left 20
            									 0.5f,  0.5f , 0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right 21
            									 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right 22     
             									-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left 23        
};

const GLuint Renderer::cube_indices[36] = { 0, 1, 2,    // back face
											1, 0, 3,    // back face
											4, 5, 6,    // front face
											6, 7, 4,    // front face
											8, 9, 10,   // left face
											10, 11, 8,  // left face
											12, 13, 14, // right face
											13, 12, 15, // right face
											16, 17, 18, // bottom face
											18, 19, 16, // bottom face
											20, 21, 22, // top face
											21, 20, 23  // top face			
};    


//--------------------------------------------------------------------------
// convert HDR equirectangular environment map to cubemap equivalent
//--------------------------------------------------------------------------
void Renderer::ConvertHdrMapToCubemap() {
	cubeProgram.Run();
	cubeProgram.SetUniform("equirectangularMap", 0);
    cubeProgram.SetUniform("projection", skyBoxHDR.GetProjection());
    GLCall(glActiveTexture(GL_TEXTURE0));
    GLCall(glBindTexture(GL_TEXTURE_2D, skyBoxHDR.GetDescriptor()));

    GLCall(glViewport(0, 0, skyBoxHDR.GetSize(), skyBoxHDR.GetSize())); // configure the viewport to the capture dimensions.
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, skyBoxHDR.GetFBO()));

	glm::mat4* views = skyBoxHDR.GetView();
    for (unsigned int i = 0; i < 6; ++i) {
		cubeProgram.SetUniform("view", views[i]);
        GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, skyBoxHDR.GetCubemap(), 0));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        //render cube
		GLCall(glBindVertexArray(cubeVAO));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIBO)); 
		GLCall(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL));
    }
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}


//--------------------------------------------------------------------------
// solve diffuse integral by convolution to create an irradiance (cube)map
//--------------------------------------------------------------------------
void Renderer::CreateIrradianceCubeMap() {
    irradianceProgram.Run();
    irradianceProgram.SetUniform("environmentMap", 0);
    irradianceProgram.SetUniform("projection", skyBoxHDR.GetProjection());
    GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxHDR.GetCubemap()));

    GLCall(glViewport(0, 0, skyBoxHDR.GetIrradianceMapSize(), skyBoxHDR.GetIrradianceMapSize())); // don't forget to configure the viewport to the capture dimensions.
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, skyBoxHDR.GetFBO()));
	glm::mat4* views = skyBoxHDR.GetView();
    for (unsigned int i = 0; i < 6; ++i) {
        irradianceProgram.SetUniform("view", views[i]);
        GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, skyBoxHDR.GetIrradiance(), 0));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        //renderCube();
		GLCall(glBindVertexArray(cubeVAO));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIBO)); 
		GLCall(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL));
    }
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

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

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

	//-------------create shader program, vao, vbo, ibo for cubemap-------------
	cubeMapSources[GL_VERTEX_SHADER] = "glsl/equirectangular_to_cubemap_vertex.glsl";
 	cubeMapSources[GL_FRAGMENT_SHADER] = "glsl/equirectangular_to_cubemap_pixel.glsl";
	cubeProgram.Init(cubeMapSources);
 	cubeProgram.Compile();
 	cubeProgram.Link();
	cubeProgram.DeleteShaders();

	GLCall(glGenVertexArrays(1, &cubeVAO));
	GLCall(glBindVertexArray(cubeVAO));

    GLCall(glGenBuffers(1, &cubeVBO));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, cubeVBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW));
    
	GLCall(glGenBuffers(1, &cubeIBO));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIBO)); 
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW));

    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0));
	GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))));
    GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat))));
    GLCall(glEnableVertexAttribArray(2));

	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	//--------------------------------------------------------------------------

	//-------------------create shader for irradiance cubemap-------------------
	irradianceMapSources[GL_VERTEX_SHADER] = "glsl/get_irradiance_vertex.glsl";
 	irradianceMapSources[GL_FRAGMENT_SHADER] = "glsl/get_irradiance_pixel.glsl";
	irradianceProgram.Init(irradianceMapSources);
 	irradianceProgram.Compile();
 	irradianceProgram.Link();
	irradianceProgram.DeleteShaders();
	//--------------------------------------------------------------------------

	program.Run();
	program.SetUniform("skyBox", 0);
	program.SetUniform("irradianceMap", 1);
 	skyBox.Load(skyBox.orbital);
	skyBoxHDR.LoadHDR(skyBoxHDR.winterForestHDR);
	ConvertHdrMapToCubemap();
	skyBoxHDR.InitIrradianceCubemap();
	CreateIrradianceCubeMap();
	
}

void Renderer::Render(int width, int height) {
	GLCall(glViewport(0, 0, width, height));
	GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glDepthFunc(GL_LEQUAL)); // set depth function to less than AND equal for skybox depth trick.

	//---------------------------change size of window--------------------------
	if (InputSystem::isWindowSizeChange) {
		FBO.Resize(width, height);
		this->width = width;
		this->height = height;
	}
	//--------------------------------------------------------------------------
	
	FBO.Bind();
	glm::mat4 view = camera->GetViewMatrix();
	float fov = camera->GetFieldOfView();
	Update();

	program.Run();
	//-------------set general parameters and light parameters------------------ 
	program.SetUniform("View", view);
	program.SetUniform("iResolution", glm::vec2(width, height));
	program.SetUniform("fieldOfView", fov);
	program.SetUniform("Time", (float)glfwGetTime());

	program.SetUniform("lightDirection", fractalsParameters.light_direction);
	program.SetUniform("ambientLightColor", fractalsParameters.ambient_light_color);
	program.SetUniform("diffuseLightColor", fractalsParameters.diffuse_light_color);
	program.SetUniform("specularLightColor", fractalsParameters.specular_light_color);
	//--------------------------------------------------------------------------

	//-------------------------set background type------------------------------
	switch(fractalsParameters.background_type) {
        case BackgroundType::Solid: {
			program.SetUniform("reflectedColor", fractalsParameters.background_color);
            break;
        }
        case BackgroundType::Skybox: {
			GLCall(glActiveTexture(GL_TEXTURE0));
			GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox.GetDescriptor()));
            break;
        }
		case BackgroundType::SkyboxHDR: {
			GLCall(glActiveTexture(GL_TEXTURE0));
			GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxHDR.GetCubemap()));
			//if (fractalsParameters.irradianceCubemap) {
			//	program.SetUniform("irradianceMap", 1);
				GLCall(glActiveTexture(GL_TEXTURE1));
				GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxHDR.GetIrradiance()));
			//}
            break;
        }
    }
	//--------------------------------------------------------------------------
	
	//---------------------------set fractal color------------------------------
	program.SetUniform("ambientColor", fractalsParameters.ambient_fractal_color);
	program.SetUniform("diffuseColor", fractalsParameters.diffuse_fractal_color);
	program.SetUniform("specularColor", fractalsParameters.specular_fractal_color);
	program.SetUniform("shininess", fractalsParameters.shininess);
	program.SetUniform("reflection", fractalsParameters.reflection);
	//--------------------------------------------------------------------------

	//-------------------------set fractal parameters---------------------------
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
	//--------------------------------------------------------------------------

	//---------------------------draw fractal-----------------------------------
	GLCall(glBindVertexArray(VAO)); 
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO)); 
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL));
	//--------------------------------------------------------------------------

	FBO.Unbind();
}

void Renderer::Update() {
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
 	GLCall(glClearColor(0.0f, 0.0f, 1.0f, 1.0f));
}

void Renderer::Destroy() {
	program.Delete();
}