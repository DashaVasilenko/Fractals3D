#ifndef __RENDERER__
#define __RENDERER__

#include "window.h"
#include "shaderProgram.h"
#include "camera.h"
#include "framebuffer.h"
#include "skybox.h"
#include "texture.h"

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
        glm::vec3 lightDirection1 = { 0.469846, 0.573576, 0.67101 };
        glm::vec3 lightColor1 = { 1.0, 1.0, 0.70 };
        float lightIntensity1 = 3.5;

        glm::vec3 lightDirection2 = { -0.707, 0.000, 0.707 };
        glm::vec3 lightColor2 = { 0.25, 0.20, 0.15 };
        float lightIntensity2 = 4.0;

        glm::vec3 ambientLightColor = { 0.35, 0.30, 0.25 };
        float ambientLightIntensity = 2.5;

        // Background parameters
        BackgroundType background_type = BackgroundType::SkyboxHDR;
        glm::vec3 background_color = { 0.30, 0.36, 0.60 };
        glm::vec3 sun_color = { 0.8, 0.7, 0.5 };
        SkyboxTexture skybox_texture = SkyboxTexture::Orbital;
        SkyboxTextureHDR skybox_texture_hdr = SkyboxTextureHDR::WinterForest;
        bool irradianceCubemap = true;

        // General fractal parameters
        float shadow_strength = 32.0f;
        float shininess = 32.0f;
        float reflection = 0.2f;

        // Fractals coloring
        ColoringType coloring_type = ColoringType::Type1;
        glm::vec3 type1_color = { 1.0, 0.8, 0.7 };
        glm::vec3 type2_color1 = { 0.10, 0.20, 0.30 };
        glm::vec3 type2_color2 = { 0.02, 0.10, 0.30 };
        glm::vec3 type2_color3 = { 0.30, 0.10, 0.02 };
        glm::vec3 type3_color = { 1.0, 0.8, 0.7 };
        glm::vec3 type4_color = { 1.0, 0.8, 0.7 };
        glm::vec3 type5_color = { 1.0, 0.8, 0.7 };

        // Test fractal parameters

        // Mandelbulb fractal parameters
        int mandelbulb_iterations = 8;
        float mandelbulb_bailout = 10.0f;
        float mandelbulb_power = 9.0f;

        // Julia1 fractal parameters
        float julia1_offset = 1.0;
        float julia1_smoothness = 4.0;

        // Julia2 fractal parameters
        glm::vec3 julia2_offset = { 0.1498, 0.4479, 0.4495 };
        float julia2_w = 0.4491;
        float julia2_smoothness = 4.0;

        // Julia3 fractal parameters
        float julia3_offset = 1.0;
        float julia3_smoothness = 9.0;

        // Julia4 fractal parameters
        glm::vec3 julia4_offset = { -0.05, -0.052, 0.85 };
        float julia4_w = -0.151;
        float julia4_smoothness = 8.7;

        // Sierpinski1 fractal parameters
        glm::vec3 sierpinski1_vector1 = { 0.0, 0.57735,   0.0 };
        glm::vec3 sierpinski1_vector2 = { 0.0, -1.0,  1.15470 };
        glm::vec3 sierpinski1_vector3 = { 1.0, -1.0, -0.57735 };
        glm::vec3 sierpinski1_vector4 = {-1.0, -1.0, -0.57735 };


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