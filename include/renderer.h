#ifndef __RENDERER__
#define __RENDERER__

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
    void SetCameraType(CameraType t) { camera->SetCameraType(t); }

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
        glm::vec3 room_background = { 0.0, 0.0, 0.0 };
        glm::vec3 sun_color = { 0.8, 0.7, 0.5 };
        SkyboxTexture skybox_texture = SkyboxTexture::Orbital;
        SkyboxTextureHDR skybox_texture_hdr = SkyboxTextureHDR::WinterForest;
        bool irradianceCubemap = true;
        float background_brightness = 1.0;
        int anti_aliasing = 1;

        // General fractal parameters
        float shadow_strength = 32.0f;
        float shininess = 32.0f;
        float reflection = 0.2f;
        glm::vec3 exposure = { 0.126, 0.7152, 0.0722 };
        bool tone_mapping = false;

        // Fractals coloring
        ColoringType coloring_type = ColoringType::Type1;
        glm::vec3 type1_color = { 1.0, 0.8, 0.7 };
        glm::vec3 type2_color1 = { 0.10, 0.20, 0.30 };
        glm::vec3 type2_color2 = { 0.02, 0.10, 0.30 };
        glm::vec3 type2_color3 = { 0.30, 0.10, 0.02 };
        glm::vec3 type3_color = { 1.0, 0.8, 0.7 };
        float type3_coef = 2.0;
        glm::vec3 type4_color = { 1.0, 0.8, 0.7 };
        glm::vec3 type5_color1 = { 1.0, 0.0, 0.0 };
        glm::vec3 type5_color2 = { 0.0, 1.0, 0.0 };
        glm::vec3 type5_color3 = { 0.0, 0.0, 1.0 };
        glm::vec3 type6_color = { 1.0, 0.8, 0.7 };
        glm::vec3 type7_color1 = { 1.0,  1.0, 1.0 };
        glm::vec3 type7_color2 = { 1.0, 0.80, 0.2 };
        glm::vec3 type7_color3 = { 1.0, 0.55, 0.0 };


        // Test fractal parameters

        // Mandelbulb fractal parameters
        int mandelbulb_iterations = 8;
        float mandelbulb_bailout = 10.0f;
        float mandelbulb_power = 9.0f;

        // Juliabulb1 fractal parameters
        float juliabulb1_offset = 1.6;
        float juliabulb1_smoothness = 8.0;
        int juliabulb1_iterations = 4;

        // Julia1 fractal parameters
        float julia1_offset = 1.0;
        float julia1_smoothness = 4.0;
        int julia1_iterations = 11;

        // Julia2 fractal parameters
        glm::vec3 julia2_offset = { 0.1498, 0.4479, 0.4495 };
        float julia2_w = 0.4491;
        float julia2_smoothness = 4.0;
        int julia2_iterations = 11;

        // Julia3 fractal parameters
        float julia3_offset = 1.0;
        float julia3_smoothness = 9.0;
        int julia3_iterations = 11;

        // Julia4 fractal parameters
        glm::vec3 julia4_offset = { -0.05, -0.052, 0.85 };
        float julia4_w = -0.151;
        float julia4_smoothness = 8.7;
        int julia4_iterations = 11;

        // Sierpinski1 fractal parameters
        glm::vec3 sierpinski1_vector1 = { 0.0, 0.57735,   0.0 };
        glm::vec3 sierpinski1_vector2 = { 0.0, -1.0,  1.15470 };
        glm::vec3 sierpinski1_vector3 = { 1.0, -1.0, -0.57735 };
        glm::vec3 sierpinski1_vector4 = {-1.0, -1.0, -0.57735 };
        int sierpinski1_iterations = 8;

        // Sierpinski2 fractal parameters
        glm::vec3 sierpinski2_vector1 = {  -1.0, 0.618, 1.618 };
        glm::vec3 sierpinski2_vector2 = { 0.618, 1.618,  -1.0 };
        glm::vec3 sierpinski2_vector3 = { 1.618,  -1.0, 0.618 };
        int sierpinski2_iterations = 6;

        // MengerSponge1 fractal parameters
        float menger_sponge1_offset1 = 1.0f;
        float menger_sponge1_offset2 = 1.0f;
        int menger_sponge1_iterations = 4;

        // MengerSponge2 fractal parameters
        float menger_sponge2_offset1 = 1.0f;
        float menger_sponge2_offset2 = 1.0f;
        int menger_sponge2_iterations = 5;

        //MengerSponge3 fractal parameters
        int menger_sponge3_type;

        // Apollonian1 fractal parameters
        float apollonian1_offset1 = 12.0f;
        float apollonian1_offset2 = 1.1f;
        int apollonian1_iterations = 8;

        // Apollonian2 fractal parameters
        float apollonian2_offset1 = 12.0f;
        float apollonian2_offset2 = 1.1f;
        int apollonian2_iterations = 6;

        // Apollonian3 fractal parameters
        float apollonian3_offset1 = 20.0f;
        float apollonian3_offset2 = 1.1f;
        float apollonian3_offset3 = 1.0f;
        glm::vec3 apollonian3_csize = { 0.808, 0.8, 1.137 };
        int apollonian3_iterations = 9;
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