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

    void ConvertHdrMapToCubemapMonster();


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
        // Light parameters
        glm::vec3 lightDirection1 = { 0.577, 0.577, -0.577 };
        glm::vec3 lightColor1 = { 1.0, 1.0, 0.70 };
        float lightIntensity1 = 12.0;

        glm::vec3 lightDirection2 = { -0.707, 0.000, 0.707 };
        glm::vec3 lightColor2 = { 0.25, 0.20, 0.15 };
        float lightIntensity2 = 4.0;

        glm::vec3 ambientLightColor = { 0.35, 0.30, 0.25 };
        float ambientLightIntensity = 2.5;

        // Background parameters
        BackgroundType background_type = BackgroundType::SkyboxHDR;
        glm::vec3 background_color = { 0.30, 0.36, 0.60 };
        SkyboxTexture skybox_texture = SkyboxTexture::Orbital;
        SkyboxTextureHDR skybox_texture_hdr = SkyboxTextureHDR::WinterForest;
        bool irradianceCubemap = true;

        // General fractal parameters
        float shadow_strength = 32.0f;
        float shininess = 32.0f;
        float reflection = 0.2f;

        // Test fractal parameters
        glm::vec3 test_color = { 0.19225, 0.19225, 0.19225 };

        // Mandelbulb fractal parameters
        glm::vec3 mandelbulb_color1 = { 0.10, 0.20, 0.30 };
        glm::vec3 mandelbulb_color2 = { 0.02, 0.10, 0.30 };
        glm::vec3 mandelbulb_color3 = { 0.30, 0.10, 0.02 };
        int mandelbulb_iterations = 8;
        float mandelbulb_bailout = 10.0f;
        float mandelbulb_power = 9.0f;

        SkyBoxHDR monsterColorHDR;

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