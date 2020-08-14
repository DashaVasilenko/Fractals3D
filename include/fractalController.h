#ifndef __FRACTALCONTROLLER__
#define __FRACTALCONTROLLER__

#include "renderer.h"
#include "types.h"

class FractalController {
public:
    void Init(Renderer* r);
    void Render(int width, int height) { renderer->Render(width, height); }
    void LoadShaderProgram() { program->Load(); }

    FrameBuffer* GetFBO() { return renderer->GetFBO(); }
    Camera* GetCamera() { return renderer->GetCamera(); }
    ShaderProgram* GetShaderProgram() { return renderer->GetShaderProgram(); }

    // Background
    void SetBackgroundType(BackgroundType c) { program->SetBackgroundType(c); renderer->fractalsParameters.background_type = c; }
    void SetBackgroundColor(const glm::vec3& c) { renderer->fractalsParameters.background_color = c; }
    void SetSunColor(const glm::vec3& c) { renderer->fractalsParameters.sun_color = c; }
    void SetBackgroundBrightness(float c) { renderer->fractalsParameters.background_brightness = c; }
        

    // Textures
    void SetSkyboxTexture(SkyboxTexture skyboxTexture);
    void SetSkyboxTexture(std::array<std::string, 6>& paths);

    // HDR Textures
    void SetSkyboxTextureHDR(SkyboxTextureHDR skyboxTextureHDR);
    void SetIrradianceMap(bool irradianceMap);
    void SetSkyboxTextureHDR(std::string path);

    // Fractals coloring
    void SetColoringType(ColoringType c);
    void SetType1Color(const glm::vec3& c) { renderer->fractalsParameters.type1_color = c; }
    void SetType2Color1(const glm::vec3& c) { renderer->fractalsParameters.type2_color1 = c; }
    void SetType2Color2(const glm::vec3& c) { renderer->fractalsParameters.type2_color2 = c; }
    void SetType2Color3(const glm::vec3& c) { renderer->fractalsParameters.type2_color3 = c; }
    void SetType3Color(const glm::vec3& c) { renderer->fractalsParameters.type3_color = c; }
    void SetType3Coef(float c) { renderer->fractalsParameters.type3_coef = c; }
    void SetType4Color(const glm::vec3& c) { renderer->fractalsParameters.type4_color = c; }
    void SetType5Color1(const glm::vec3& c) { renderer->fractalsParameters.type5_color1 = c; }
    void SetType5Color2(const glm::vec3& c) { renderer->fractalsParameters.type5_color2 = c; }
    void SetType5Color3(const glm::vec3& c) { renderer->fractalsParameters.type5_color3 = c; }
    void SetType6Color(const glm::vec3& c) { renderer->fractalsParameters.type6_color = c; }
    void SetType7Color1(const glm::vec3& c) { renderer->fractalsParameters.type7_color1 = c; }
    void SetType7Color2(const glm::vec3& c) { renderer->fractalsParameters.type7_color2 = c; }
    void SetType7Color3(const glm::vec3& c) { renderer->fractalsParameters.type7_color3 = c; }


    // General fractal parameters
    void SetShadowStrength(float c ) { renderer->fractalsParameters.shadow_strength = c; }
    void SetFractalShininess(float c) { renderer->fractalsParameters.shininess = c; }
    void SetFractalReflect(float c) { renderer->fractalsParameters.reflection = c; }

    void SetFractalType(FractalType c);
    
    // Sun parameters
    void SetLightDirection1(const glm::vec3& c) { renderer->fractalsParameters.lightDirection1 = c; }
    void SetLightColor1(const glm::vec3& c) { renderer->fractalsParameters.lightColor1 = c; }
    void SetLightIntensity1(float c) { renderer->fractalsParameters.lightIntensity1 = c; }

    // Light parameters
    void SetLightDirection2(const glm::vec3& c) { renderer->fractalsParameters.lightDirection2 = c; }
    void SetLightColor2(const glm::vec3& c) { renderer->fractalsParameters.lightColor2 = c; }
    void SetLightIntensity2(float c) { renderer->fractalsParameters.lightIntensity2 = c; }

    // Ambient light parameters
    void SetAmbientFractalLightColor(const glm::vec3& c) { renderer->fractalsParameters.ambientLightColor = c; }
    void SetAmbientFractalLightIntensity(float c) { renderer->fractalsParameters.ambientLightIntensity = c; }

    // Test fractal parameters

    // Mandelbulb fractal parameters
    void SetMandelbulbIterations(const int c) { renderer->fractalsParameters.mandelbulb_iterations = c;  }
    void SetMandelbulbBailout(const float c) { renderer->fractalsParameters.mandelbulb_bailout = c; }
    void SetMandelbulbPower(float c) { renderer->fractalsParameters.mandelbulb_power = c; }

    // Juliabulb1 fractal parameters
    void SetJuliabulb1Offset(float c) { renderer->fractalsParameters.juliabulb1_offset = c; }
    void SetJuliabulb1Smoothness(float c) { renderer->fractalsParameters.juliabulb1_smoothness = c; }
    void SetJuliabulb1Iterations(int c) { renderer->fractalsParameters.juliabulb1_iterations = c; }

    // Julia1 fractal parameters
    void SetJulia1Offset(float c) { renderer->fractalsParameters.julia1_offset = c; }
    void SetJulia1Smoothness(float c) { renderer->fractalsParameters.julia1_smoothness = c; }
    void SetJulia1Iterations(int c) { renderer->fractalsParameters.julia1_iterations = c; }

    // Julia2 fractal parameters
    void SetJulia2Offset(const glm::vec3& c) { renderer->fractalsParameters.julia2_offset = c; }
    void SetJulia2W(float c) { renderer->fractalsParameters.julia2_w = c; }
    void SetJulia2Smoothness(float c) { renderer->fractalsParameters.julia2_smoothness = c; }
    void SetJulia2Iterations(int c) { renderer->fractalsParameters.julia2_iterations = c; }

    // Julia3 fractal parameters
    void SetJulia3Offset(float c) { renderer->fractalsParameters.julia3_offset = c; }
    void SetJulia3Smoothness(float c) { renderer->fractalsParameters.julia3_smoothness = c; }
    void SetJulia3Iterations(int c) { renderer->fractalsParameters.julia3_iterations = c; }

    // Julia4 fractal parameters
    void SetJulia4Offset(const glm::vec3& c) { renderer->fractalsParameters.julia4_offset = c; }
    void SetJulia4W(float c) { renderer->fractalsParameters.julia4_w = c; }
    void SetJulia4Smoothness(float c) { renderer->fractalsParameters.julia4_smoothness = c; }
    void SetJulia4Iterations(int c) { renderer->fractalsParameters.julia4_iterations = c; }

    // Sierpinski1 fractal parameters
    void SetSierpinski1Vector1(const glm::vec3& c) { renderer->fractalsParameters.sierpinski1_vector1 = c; }
    void SetSierpinski1Vector2(const glm::vec3& c) { renderer->fractalsParameters.sierpinski1_vector2 = c; }
    void SetSierpinski1Vector3(const glm::vec3& c) { renderer->fractalsParameters.sierpinski1_vector3 = c; }
    void SetSierpinski1Vector4(const glm::vec3& c) { renderer->fractalsParameters.sierpinski1_vector4 = c; }
    void SetSierpinski1Iterations(int c) { renderer->fractalsParameters.sierpinski1_iterations = c; }

    // Sierpinski2 fractal parameters
    void SetSierpinski2Vector1(const glm::vec3& c) { renderer->fractalsParameters.sierpinski2_vector1 = c; }
    void SetSierpinski2Vector2(const glm::vec3& c) { renderer->fractalsParameters.sierpinski2_vector2 = c; }
    void SetSierpinski2Vector3(const glm::vec3& c) { renderer->fractalsParameters.sierpinski2_vector3 = c; }
    void SetSierpinski2Iterations(int c) { renderer->fractalsParameters.sierpinski2_iterations = c; }

    // MengerSponge1 fractal parameters
    void SetMengerSponge1Offset1(float c) { renderer->fractalsParameters.menger_sponge1_offset1 = c; }
    void SetMengerSponge1Offset2(float c) { renderer->fractalsParameters.menger_sponge1_offset2 = c; }
    void SetMengerSponge1Iterations(int c) { renderer->fractalsParameters.menger_sponge1_iterations = c; }

    // MengerSponge2 fractal parameters
    void SetMengerSponge2Offset1(float c) { renderer->fractalsParameters.menger_sponge2_offset1 = c; }
    void SetMengerSponge2Offset2(float c) { renderer->fractalsParameters.menger_sponge2_offset2 = c; }
    void SetMengerSponge2Iterations(int c) { renderer->fractalsParameters.menger_sponge2_iterations = c; }

    // Apollonian1 fractal parameters
    void SetApollonian1Offset(float c) { renderer->fractalsParameters.apollonian1_offset = c; }
    void SetApollonian1Iterations(int c) { renderer->fractalsParameters.apollonian1_iterations = c; }

private:
    Renderer* renderer;
    ShaderProgram* program;
    SkyBox* skybox;
    SkyBoxHDR* skyboxHDR;

};


#endif