#include "fractalController.h"

void FractalController::Init(Renderer* r) { 
    renderer = r; 
    program = renderer->GetShaderProgram(); 
    skybox = renderer->GetSkybox(); 
    skyboxHDR = renderer->GetSkyboxHDR();
}

void FractalController::SetSkyboxTexture(SkyboxTexture skyboxTexture) { 
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

void FractalController::SetSkyboxTexture(std::array<std::string, 6>& paths) { 
    renderer->fractalsParameters.skybox_texture = SkyboxTexture::Other;  // это возможно вообще надо удалить и в рендерере тоже
    skybox->Reload(paths);
}

void FractalController::SetSkyboxTextureHDR(SkyboxTextureHDR skyboxTextureHDR) { 
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

void FractalController::SetIrradianceMap(bool irradianceMap) {
    renderer->fractalsParameters.irradianceCubemap = irradianceMap;
    program->SetIrradianceCubemap(irradianceMap);
}

void FractalController::SetSkyboxTextureHDR(std::string path) { 
    renderer->fractalsParameters.skybox_texture_hdr = SkyboxTextureHDR::OtherHDR; // это возможно вообще надо удалить и в рендерере тоже
    skyboxHDR->ReloadHDR(path);
    renderer->ConvertHdrMapToCubemap();   
    skyboxHDR->ReloadIrradianceCubemap();
    renderer->CreateIrradianceCubeMap();     
}

void FractalController::SetColoringType(ColoringType c) {
    renderer->fractalsParameters.coloring_type = c;
    program->SetColoringType(c);
    LoadShaderProgram();
}

void FractalController::SetFractalType(FractalType c) { 
    program->SetFractalType(c); 
    renderer->currentFractalType = c; 

/*
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
        case FractalType::Julia1: {
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
        case FractalType::Julia2: {
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
        case FractalType::Julia3: {
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
        case FractalType::Julia4: {
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
    */
}
    