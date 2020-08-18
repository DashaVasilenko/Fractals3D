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
    renderer->fractalsParameters.skybox_texture = SkyboxTexture::Other;
    renderer->fractalsParameters.skybox_paths = paths;
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
    renderer->fractalsParameters.skybox_texture_hdr = SkyboxTextureHDR::OtherHDR;
    renderer->fractalsParameters.skybox_hdr_path = path;
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
} 