#include "skybox.h"

void SkyBox::Load(const std::array<std::string, 6>& fileNames) {
    GLCall(glGenTextures(1, &descriptor));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, descriptor));

    stbi_set_flip_vertically_on_load(false);
    int width, height, nrChannels;
    for(unsigned int i = 0; i < fileNames.size(); i++) {
        unsigned char *data = stbi_load(fileNames[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
        }
        else {
            std::cout << "Cubemap texture failed to load at path: " << fileNames[i] << std::endl;
            stbi_image_free(data);
        }
    }

    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));  
}

void SkyBox::Reload(const std::array<std::string, 6>& fileNames) {
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, descriptor));

    stbi_set_flip_vertically_on_load(false);
    int width, height, nrChannels;
    for(unsigned int i = 0; i < fileNames.size(); i++) {
        unsigned char *data = stbi_load(fileNames[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
            stbi_image_free(data);
        }
        else {
            std::cout << "Cubemap texture failed to load at path: " << fileNames[i] << std::endl;
            stbi_image_free(data);
        }
    }

    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));  
}

SkyBox::~SkyBox() {
    GLCall(glDeleteTextures(1, &descriptor));
} 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
// initialization: setup framebuffer, load the HDR environment map, setup and attach cubemap
//-------------------------------------------------------------------------------------------------
void SkyBoxHDR::LoadHDR(const std::string& fileName) {
    // ----------------------------setup framebuffer----------------------------
    GLCall(glGenFramebuffers(1, &FBO));
    GLCall(glGenRenderbuffers(1, &RBO));

    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
    GLCall(glBindRenderbuffer(GL_RENDERBUFFER, RBO));
    GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024));
    GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO));
    //--------------------------------------------------------------------------

    //----------------------load the HDR environment map------------------------
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float *data = stbi_loadf(fileName.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLCall(glGenTextures(1, &descriptor));
        GLCall(glBindTexture(GL_TEXTURE_2D, descriptor));

        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data)); 
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        stbi_image_free(data); 
    }
    else {
        std::cout << "Failed to load HDR image." << std::endl;
        stbi_image_free(data); 
    }  
    //--------------------------------------------------------------------------

    //-----------setup cubemap to render to and attach to framebuffer-----------
    GLCall(glGenTextures(1, &envCubemap));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap));
    for (unsigned int i = 0; i < 6; ++i) {
        GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 1024, 1024, 0, GL_RGB, GL_FLOAT, nullptr));
    }
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR)); 
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR)); 
    //--------------------------------------------------------------------------
}


//-------------------------------------------------------------------------------------------------
// create an irradiance cubemap, and rescale FBO to irradiance scale
//-------------------------------------------------------------------------------------------------
void SkyBoxHDR::InitIrradianceCubemap() {
    GLCall(glGenTextures(1, &irradianceMap));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap));
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
    GLCall(glBindRenderbuffer(GL_RENDERBUFFER, RBO));
    GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32));
}

//-------------------------------------------------------------------------------------------------
// reload the HDR environment map
//-------------------------------------------------------------------------------------------------
void SkyBoxHDR::ReloadHDR(const std::string& fileName) {
    // rescale FBO to environment cubemap scale
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
    GLCall(glBindRenderbuffer(GL_RENDERBUFFER, RBO));
    GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024));
    GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO));

    //std::cout << fileName.c_str() << std::endl;
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float *data = stbi_loadf(fileName.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLCall(glBindTexture(GL_TEXTURE_2D, descriptor));

        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data)); 
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        stbi_image_free(data); 
    }
    else {
        std::cout << "Failed to load HDR image." << std::endl;
        stbi_image_free(data); 
    }  
}

void SkyBoxHDR::ReloadIrradianceCubemap() {
    // rescale FBO to irradiance scale
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
    GLCall(glBindRenderbuffer(GL_RENDERBUFFER, RBO));
    GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32));
}

SkyBoxHDR::~SkyBoxHDR() {
    GLCall(glDeleteTextures(1, &descriptor));
} 