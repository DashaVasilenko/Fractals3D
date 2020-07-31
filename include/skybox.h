#ifndef __SKYBOX__
#define __SKYBOX__

#include <iostream>
#include <string>
#include <array>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "stb_image.h"
#include "errors.h"

class SkyBox {
public:
    void Load(const std::array<std::string, 6>& fileNames);
    void Reload(const std::array<std::string, 6>& fileNames);

    unsigned int GetDescriptor() { return descriptor; }
    ~SkyBox();

    std::array<std::string, 6> orbital = {   "textures/skybox1/front.tga",
 											 "textures/skybox1/back.tga",
 											 "textures/skybox1/up.tga",

 											 "textures/skybox1/down.tga",
 											 "textures/skybox1/right.tga",
 											 "textures/skybox1/left.tga"
 											 };

    std::array<std::string, 6> night = {     "textures/night/posz.jpg",
 											 "textures/night/negz.jpg",
 											 "textures/night/posy.jpg",

 											 "textures/night/negy.jpg",
 											 "textures/night/negx.jpg",
 											 "textures/night/posx.jpg"
 											 };


private:
    unsigned int descriptor;

};

class SkyBoxHDR {
public:
	void LoadHDR(const std::string& fileName);
	void InitIrradianceCubemap();
    void ReloadHDR(const std::string& fileName);
	void ReloadIrradianceCubemap();

    unsigned int GetDescriptor() { return descriptor; }
	unsigned int GetIrradiance() { return irradianceMap; }
	unsigned int GetFBO() { return FBO; }
    unsigned int GetRBO() { return RBO; }
    unsigned int GetCubemap() { return envCubemap; }
	unsigned int GetSize() { return size; }
	unsigned int GetIrradianceMapSize() { return irradianceMapSize; }
	glm::mat4 GetProjection() { return projection; }
	glm::mat4* GetView() { return views; }

    ~SkyBoxHDR();

	const std::string winterForestHDR = "textures/WinterForest/WinterForest_Ref.hdr";
	const std::string milkywayHDR = "textures/Milkyway/Milkyway_small.hdr";

private:
	// projection and view matrices for capturing data onto the 6 cubemap face directions
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 views[6] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

	unsigned int size = 1024;
	unsigned int irradianceMapSize = 32;

	unsigned int FBO;
    unsigned int RBO;
	unsigned int envCubemap;

	unsigned int descriptor;
	unsigned int irradianceMap;
};

#endif /* End of __SKYBOX__ */