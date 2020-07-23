#ifndef __RENDERER__
#define __RENDERER__

#include "window.h"
#include "shaderProgram.h"
#include "camera.h"
#include "framebuffer.h"
#include "skybox.h"

class Renderer {
public:
    void SetWidth(int width) { this->width = width; }
    void SetHeight(int height) { this->height = height; }
    void SetActiveCamera(Camera* camera) { this->camera = camera; }
    void SetFractalType(FractalType fractalType) { this->currentFractalType = fractalType; }

    FrameBuffer* GetFBO() { return &FBO; }
    Camera* GetCamera() { return camera; }
    ShaderProgram* GetShaderProgram() { return &program; }

    void Init();
    void Update();
    void Render(int width, int height);
    void Destroy();

    friend class FractalController;

private:
    static const GLfloat vertices[20];
	static const GLuint indices[6];

    int width;
    int height;

    struct FractalsParameters {
        int mandelbulb_iterations = 8;
        float mandelbulb_bailout = 10.0f;
        float mandelbulb_power = 9.0f;

    };
    FractalsParameters fractalsParameters;

    FractalType currentFractalType = FractalType::Test;


    std::map<GLenum, std::string> mapSources;
    ShaderProgram program;
    GLuint VAO;
    GLuint VBO;
    GLuint IBO;
    FrameBuffer FBO;
    SkyBox skyBox;


    Camera* camera;

};
 
#endif /* End of __RENDERER__ */ 