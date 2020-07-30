#ifndef __FRACTALCONTROLLER__
#define __FRACTALCONTROLLER__

#include "renderer.h"
#include "types.h"

class FractalController {
public:
    void Init(Renderer* r) { 
        renderer = r; 
        program = renderer->GetShaderProgram(); 
        skybox = renderer->GetSkybox(); 
        skyboxHDR = renderer->GetSkyboxHDR();
    }

    FrameBuffer* GetFBO() { return renderer->GetFBO(); }
    Camera* GetCamera() { return renderer->GetCamera(); }
    ShaderProgram* GetShaderProgram() { return renderer->GetShaderProgram(); }

    void Render(int width, int height) { renderer->Render(width, height); }
    void LoadShaderProgram() { program->Load(); }

    void SetLightDirection(const glm::vec3& c) { renderer->fractalsParameters.light_direction = c; }
    void SetAmbientLightColor(const glm::vec3& c) { renderer->fractalsParameters.ambient_light_color = c; }
    void SetDiffuseLightColor(const glm::vec3& c) { renderer->fractalsParameters.diffuse_light_color = c; }
    void SetSpecularLightColor(const glm::vec3& c) { renderer->fractalsParameters.specular_light_color = c; }

    void SetBackgroundType(BackgroundType c) { program->SetBackgroundType(c); renderer->fractalsParameters.background_type = c; }
    void SetBackgroundColor(const glm::vec3& c) { renderer->fractalsParameters.background_color = c; }

    void SetSkyboxTexture(SkyboxTexture skyboxTexture) { 
        renderer->fractalsParameters.skybox_texture = skyboxTexture; 
        switch(skyboxTexture) {
            case SkyboxTexture::Orbital: {
                skybox->Reload(skybox->orbital);
                break;
            }
            case SkyboxTexture::Night: {
                skybox->Reload(skybox->night);
                break;
            }
        }
    }

    void SetSkyboxTextureHDR(SkyboxTextureHDR skyboxTextureHDR) { 
        renderer->fractalsParameters.skybox_texture_hdr = skyboxTextureHDR; 
        switch(skyboxTextureHDR) {
            case SkyboxTextureHDR::WinterForest: {
                skyboxHDR->ReloadHDR(skyboxHDR->winterForestHDR);
                renderer->ConvertHdrMapToCubemap();
                break;
            }
            case SkyboxTextureHDR::Milkyway: {
                skyboxHDR->ReloadHDR(skyboxHDR->milkywayHDR);
                renderer->ConvertHdrMapToCubemap();
                break;
            }
        }
    }

    void SetSkyboxTextureHDR(std::string path) { 
        std::cout << path.c_str() << std::endl;
        renderer->fractalsParameters.skybox_texture_hdr = SkyboxTextureHDR::Other; 
        skyboxHDR->ReloadHDR(path);
        renderer->ConvertHdrMapToCubemap();        
    }

    void SetAmbientFractalColor(const glm::vec3& c) { renderer->fractalsParameters.ambient_fractal_color = c; }
    void SetDiffuseFractalColor(const glm::vec3& c) { renderer->fractalsParameters.diffuse_fractal_color = c; }
    void SetSpecularFractalColor(const glm::vec3& c) { renderer->fractalsParameters.specular_fractal_color = c; }
    void SetFractalShininess(float c) { renderer->fractalsParameters.shininess = c; }
    void SetFractalReflect(float c) { renderer->fractalsParameters.reflection = c; }

    void SetFractalType(FractalType c) { program->SetFractalType(c); renderer->currentFractalType = c; }
    void SetMandelbulbIterations(const int c) { renderer->fractalsParameters.mandelbulb_iterations = c;  }
    void SetMandelbulbBailout(const float c) { renderer->fractalsParameters.mandelbulb_bailout = c; }
    void SetMandelbulbPower(float c) { renderer-> fractalsParameters.mandelbulb_power = c; }

private:
    Renderer* renderer;
    ShaderProgram* program;
    SkyBox* skybox;
    SkyBoxHDR* skyboxHDR;

};


#endif