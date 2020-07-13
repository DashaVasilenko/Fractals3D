#ifndef __RENDERER__
#define __RENDERER__

#include "window.h"
#include "shaderProgram.h"
#include "camera.h"
#include "framebuffer.h"

class Renderer {
public:
    void SetWidth(int width) { this->width = width; }
    void SetHeight(int height) { this->height = height; }
    void SetActiveCamera(Camera* camera) { this->camera = camera; }
    FrameBuffer* GetFBO() { return &FBO; }
    Camera* GetCamera() { return camera; }
    void Init();
    void Update();
    void Render(int width, int height);
    void Destroy();

private:
    int width;
    int height;

    static const GLfloat vertices[20];
	static const GLuint indices[6];

    std::map<GLenum, std::string> mapSources;
    ShaderProgram program;
    GLuint VAO;
    GLuint VBO;
    GLuint IBO;
    FrameBuffer FBO;

    Camera* camera;

};

/*
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
*/
 
#endif /* End of __RENDERER__ */ 