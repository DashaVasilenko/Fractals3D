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

    // Textures
    void SetSkyboxTexture(SkyboxTexture skyboxTexture);
    void SetSkyboxTexture(std::array<std::string, 6>& paths);

    // HDR Textures
    void SetSkyboxTextureHDR(SkyboxTextureHDR skyboxTextureHDR);
    void SetIrradianceMap(bool irradianceMap);
    void SetSkyboxTextureHDR(std::string path);

    // Fractals coloring
    void SetColoringType(ColoringType c);
    void SetOneColorColor(const glm::vec3& c) { renderer->fractalsParameters.one_color_color = c; }
    void SetThreeColorsColor1(const glm::vec3& c) { renderer->fractalsParameters.three_colors_color1 = c; }
    void SetThreeColorsColor2(const glm::vec3& c) { renderer->fractalsParameters.three_colors_color2 = c; }
    void SetThreeColorsColor3(const glm::vec3& c) { renderer->fractalsParameters.three_colors_color3 = c; }

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
    void SetMandelbulbPower(float c) { renderer-> fractalsParameters.mandelbulb_power = c; }

    // Julia fractal parameters
    
private:
    Renderer* renderer;
    ShaderProgram* program;
    SkyBox* skybox;
    SkyBoxHDR* skyboxHDR;

};


#endif