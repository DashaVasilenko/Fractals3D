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
            case SkyboxTexture::Other: {
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
            case SkyboxTextureHDR::OtherHDR: {
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

    void SetFractalShininess(float c) { renderer->fractalsParameters.shininess = c; }
    void SetFractalReflect(float c) { renderer->fractalsParameters.reflection = c; }

    void SetFractalType(FractalType c) { 
        program->SetFractalType(c); 
        renderer->currentFractalType = c; 
    
        switch(c) {
            case FractalType::Test: {

                break;
            }
            case FractalType::Mandelbulb: {
                renderer->fractalsParameters.lightDirection1 = glm::vec3(0.577, 0.577, -0.577);
                renderer->fractalsParameters.lightColor1 = glm::vec3(1.0, 1.0, 0.70);
                renderer->fractalsParameters.lightIntensity1 = 12.0;
                
                renderer->fractalsParameters.lightDirection2 = glm::vec3(-0.707, 0.000, 0.707);
                renderer->fractalsParameters.lightColor2 = glm::vec3(0.25, 0.20, 0.15);
                renderer->fractalsParameters.lightIntensity2 = 4.0;

                renderer->fractalsParameters.ambientLightColor = glm::vec3(0.35, 0.30, 0.25);
                renderer->fractalsParameters.ambientLightIntensity = 2.5;

                break;
            }
            case FractalType::Monster: {

                break;
            }
            case FractalType::Julia: {
                renderer->fractalsParameters.lightDirection1 = glm::vec3(0.577, 0.577,  0.577);
                renderer->fractalsParameters.lightColor1 = glm::vec3(1.00,0.90,0.70);
                renderer->fractalsParameters.lightIntensity1 = 3.5;
                
                renderer->fractalsParameters.lightDirection2 = glm::vec3(-0.707, 0.000, 0.707);
                renderer->fractalsParameters.lightColor2 = glm::vec3(1.00,0.90,0.70);
                renderer->fractalsParameters.lightIntensity2 = 3.5;

                renderer->fractalsParameters.ambientLightColor = glm::vec3(0.35, 0.30, 0.25);
                renderer->fractalsParameters.ambientLightIntensity = 2.5;

                break;
            }
        }
    }

    void SetShadowStrength(float c ) { renderer->fractalsParameters.shadow_strength = c; }

    void SetLightDirection1(const glm::vec3& c) { renderer->fractalsParameters.lightDirection1 = c; }
    void SetLightColor1(const glm::vec3& c) { renderer->fractalsParameters.lightColor1 = c; }
    void SetLightIntensity1(float c) { renderer->fractalsParameters.lightIntensity1 = c; }

    void SetLightDirection2(const glm::vec3& c) { renderer->fractalsParameters.lightDirection2 = c; }
    void SetLightColor2(const glm::vec3& c) { renderer->fractalsParameters.lightColor2 = c; }
    void SetLightIntensity2(float c) { renderer->fractalsParameters.lightIntensity2 = c; }

    void SetAmbientFractalLightColor(const glm::vec3& c) { renderer->fractalsParameters.ambientLightColor = c; }
    void SetAmbientFractalLightIntensity(float c) { renderer->fractalsParameters.ambientLightIntensity = c; }

    void SetTestColor(const glm::vec3& c) { renderer->fractalsParameters.test_color = c; }

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