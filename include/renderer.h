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
    SkyBox* GetSkybox() { return &skyBox; }
    SkyBoxHDR* GetSkyboxHDR() { return &skyBoxHDR; }

    void ConvertHdrMapToCubemap();
    void CreateIrradianceCubeMap();

    void Init();
    void Update();
    void Render(int width, int height);
    void Destroy();

    friend class FractalController;

private:
    static const GLfloat cube_vertices[192];
	static const GLuint cube_indices[36];

    static const GLfloat vertices[20];
	static const GLuint indices[6];

    struct FractalsParameters {
        glm::vec3 light_direction = { 0.0f, -1.0f, 0.0f };
        glm::vec3 ambient_light_color = { 1.0, 1.0, 1.0 };
        glm::vec3 diffuse_light_color = { 1.0, 1.0, 1.0 };
        glm::vec3 specular_light_color = { 1.0, 1.0, 1.0 };

        BackgroundType background_type = BackgroundType::SkyboxHDR;
        glm::vec3 background_color = { 0.30, 0.36, 0.60 };
        SkyboxTexture skybox_texture = SkyboxTexture::Orbital;
        SkyboxTextureHDR skybox_texture_hdr = SkyboxTextureHDR::WinterForest;
        bool irradianceCubemap = true;

        glm::vec3 ambient_fractal_color = { 0.19225, 0.19225, 0.19225 };
        glm::vec3 diffuse_fractal_color = { 0.50754, 0.50754, 0.50754 };
        glm::vec3 specular_fractal_color = { 0.50827, 0.50827, 0.50827 };
        float shininess = 2.0f;
        float reflection = 0.2f;

        int mandelbulb_iterations = 8;
        float mandelbulb_bailout = 10.0f;
        float mandelbulb_power = 9.0f;

    };
    FractalsParameters fractalsParameters;

    FractalType currentFractalType = FractalType::Test;
    SkyBox skyBox;
    SkyBoxHDR skyBoxHDR;

    std::map<GLenum, std::string> mapSources;
    ShaderProgram program;
    GLuint VAO;
    GLuint VBO;
    GLuint IBO;
    FrameBuffer FBO;

    std::map<GLenum, std::string> cubeMapSources;
    ShaderProgram cubeProgram;
    GLuint cubeVAO;
    GLuint cubeVBO;
    GLuint cubeIBO;

    std::map<GLenum, std::string> irradianceMapSources;
    ShaderProgram irradianceProgram;

    Camera* camera;

    int width;
    int height;
};
 
#endif /* End of __RENDERER__ */ 