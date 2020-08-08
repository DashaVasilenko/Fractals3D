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
            case SkyboxTexture::PalmTrees: {
                skybox->Reload(skybox->palmTrees);
                break;
            }
            case SkyboxTexture::CoitTower: {
                skybox->Reload(skybox->coitTower);
                break;
            }
            case SkyboxTexture::MountainPath: {
                skybox->Reload(skybox->mountainPath);
                break;
            }
            case SkyboxTexture::NightPath: {
                skybox->Reload(skybox->nightPath);
                break;
            }
            case SkyboxTexture::Vasa: {
                skybox->Reload(skybox->vasa);
                break;
            }
        }
    }

    void SetSkyboxTexture(std::array<std::string, 6>& paths) { 
        renderer->fractalsParameters.skybox_texture = SkyboxTexture::Other;  // это возможно вообще надо удалить и в рендерере тоже
        skybox->Reload(paths);
    }

    void SetSkyboxTextureHDR(SkyboxTextureHDR skyboxTextureHDR) { 
        renderer->fractalsParameters.skybox_texture_hdr = skyboxTextureHDR; 
        switch(skyboxTextureHDR) {
            case SkyboxTextureHDR::WinterForest: {
                skyboxHDR->ReloadHDR(skyboxHDR->winterForestHDR);
                break;
            }
            case SkyboxTextureHDR::Milkyway: {
                skyboxHDR->ReloadHDR(skyboxHDR->milkywayHDR);
                break;
            }
            case SkyboxTextureHDR::GrandCanyon: {
                skyboxHDR->ReloadHDR(skyboxHDR->grandCanyonHDR);
                break;
            }
            case SkyboxTextureHDR::IceLake: {
                skyboxHDR->ReloadHDR(skyboxHDR->iceLakeHDR);
                break;
            }
            case SkyboxTextureHDR::Factory: {
                skyboxHDR->ReloadHDR(skyboxHDR->factoryHDR);
                break;
            }
            case SkyboxTextureHDR::TopangaForest: {
                skyboxHDR->ReloadHDR(skyboxHDR->topangaForestHDR);
                break;
            }
            case SkyboxTextureHDR::TropicalBeach: {
                skyboxHDR->ReloadHDR(skyboxHDR->tropicalBeachHDR);
                break;
            }
        }
        renderer->ConvertHdrMapToCubemap();
        skyboxHDR->ReloadIrradianceCubemap();
        renderer->CreateIrradianceCubeMap();
    }

    void SetIrradianceMap(bool irradianceMap) {
        renderer->fractalsParameters.irradianceCubemap = irradianceMap;
        program->SetIrradianceCubemap(irradianceMap);
    }

    void SetSkyboxTextureHDR(std::string path) { 
        //std::cout << path.c_str() << std::endl;
        renderer->fractalsParameters.skybox_texture_hdr = SkyboxTextureHDR::OtherHDR; // это возможно вообще надо удалить и в рендерере тоже
        skyboxHDR->ReloadHDR(path);
        renderer->ConvertHdrMapToCubemap();   
        skyboxHDR->ReloadIrradianceCubemap();
        renderer->CreateIrradianceCubeMap();     
    }

    void SetAmbientFractalColor(const glm::vec3& c) { renderer->fractalsParameters.ambient_fractal_color = c; }
    void SetDiffuseFractalColor(const glm::vec3& c) { renderer->fractalsParameters.diffuse_fractal_color = c; }
    void SetSpecularFractalColor(const glm::vec3& c) { renderer->fractalsParameters.specular_fractal_color = c; }
    void SetFractalShininess(float c) { renderer->fractalsParameters.shininess = c; }
    void SetFractalReflect(float c) { renderer->fractalsParameters.reflection = c; }

    void SetFractalType(FractalType c) { program->SetFractalType(c); renderer->currentFractalType = c; }

    void SetLightDirection1(const glm::vec3& c) { renderer->fractalsParameters.mandelbulb_lightDirection1 = c; }
    void SetLightColor1(const glm::vec3& c) { renderer->fractalsParameters.mandelbulb_lightColor1 = c; }
    void SetLightIntensity1(float c) { renderer->fractalsParameters.mandelbulb_lightIntensity1 = c; }

    void SetLightDirection2(const glm::vec3& c) { renderer->fractalsParameters.mandelbulb_lightDirection2 = c; }
    void SetLightColor2(const glm::vec3& c) { renderer->fractalsParameters.mandelbulb_lightColor2 = c; }
    void SetLightIntensity2(float c) { renderer->fractalsParameters.mandelbulb_lightIntensity2 = c; }

    void SetAmbientFractalLightColor(const glm::vec3& c) { renderer->fractalsParameters.mandelbulb_ambientLightColor = c; }
    void SetAmbientFractalLightIntensity(float c) { renderer->fractalsParameters.mandelbulb_ambientLightIntensity = c; }

    void SetMandelbulbColor1(const glm::vec3& c) { renderer->fractalsParameters.mandelbulb_color1 = c; }
    void SetMandelbulbColor2(const glm::vec3& c) { renderer->fractalsParameters.mandelbulb_color2 = c; }
    void SetMandelbulbColor3(const glm::vec3& c) { renderer->fractalsParameters.mandelbulb_color3 = c; }
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