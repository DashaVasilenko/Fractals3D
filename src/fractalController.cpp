#include "fractalController.h"

void FractalController::Init(Renderer* r, const std::string& path) { 
    renderer = r; 
    program = renderer->GetShaderProgram(); 
    skybox = renderer->GetSkybox(); 
    skyboxHDR = renderer->GetSkyboxHDR();
    program_path = path;
}

void FractalController::SetSkyboxTexture(SkyboxTexture skyboxTexture) { 
    renderer->fractalsParameters.skybox_texture = skyboxTexture; 
    switch(skyboxTexture) {
        case SkyboxTexture::Orbital: {
            skybox->Reload(skybox->orbital, program_path);
            break;
        }
        case SkyboxTexture::Night: {
            skybox->Reload(skybox->night, program_path);
            break;
        }
        case SkyboxTexture::PalmTrees: {
            skybox->Reload(skybox->palmTrees, program_path);
            break;
        }
        case SkyboxTexture::CoitTower: {
            skybox->Reload(skybox->coitTower, program_path);
            break;
        }
        case SkyboxTexture::MountainPath: {
            skybox->Reload(skybox->mountainPath, program_path);
            break;
        }
        case SkyboxTexture::NightPath: {
            skybox->Reload(skybox->nightPath, program_path);
            break;
        }
        case SkyboxTexture::Vasa: {
            skybox->Reload(skybox->vasa, program_path);
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
    skybox->Reload(paths, program_path);
}

void FractalController::SetSkyboxTextureHDR(SkyboxTextureHDR skyboxTextureHDR) { 
    renderer->fractalsParameters.skybox_texture_hdr = skyboxTextureHDR; 
    switch(skyboxTextureHDR) {
        case SkyboxTextureHDR::WinterForest: {
            skyboxHDR->ReloadHDR(skyboxHDR->winterForestHDR, program_path);
            break;
        }
        case SkyboxTextureHDR::Milkyway: {
            skyboxHDR->ReloadHDR(skyboxHDR->milkywayHDR, program_path);
            break;
        }
        case SkyboxTextureHDR::GrandCanyon: {
            skyboxHDR->ReloadHDR(skyboxHDR->grandCanyonHDR, program_path);
            break;
        }
        case SkyboxTextureHDR::IceLake: {
            skyboxHDR->ReloadHDR(skyboxHDR->iceLakeHDR, program_path);
            break;
        }
        case SkyboxTextureHDR::Factory: {
            skyboxHDR->ReloadHDR(skyboxHDR->factoryHDR, program_path);
            break;
        }
        case SkyboxTextureHDR::TopangaForest: {
            skyboxHDR->ReloadHDR(skyboxHDR->topangaForestHDR, program_path);
            break;
        }
        case SkyboxTextureHDR::TropicalBeach: {
            skyboxHDR->ReloadHDR(skyboxHDR->tropicalBeachHDR, program_path);
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
    skyboxHDR->ReloadHDR(path, program_path);
    renderer->ConvertHdrMapToCubemap();   
    skyboxHDR->ReloadIrradianceCubemap();
    renderer->CreateIrradianceCubeMap();     
}

void FractalController::SetColoringType(ColoringType c) {
    renderer->fractalsParameters.coloring_type = c;
    program->SetColoringType(c);
    LoadShaderProgram(program_path);
}

void FractalController::SetFractalType(FractalType c) { 
    program->SetFractalType(c); 
    renderer->currentFractalType = c; 
} 