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

    std::array<std::string, 6> orbital = {   "textures/orbital/front.tga",
 											 "textures/orbital/back.tga",
 											 "textures/orbital/up.tga",

 											 "textures/orbital/down.tga",
 											 "textures/orbital/right.tga",
 											 "textures/orbital/left.tga"
 											 };

    std::array<std::string, 6> night = {     "textures/Night/posz.jpg",
 											 "textures/Night/negz.jpg",
 											 "textures/Night/posy.jpg",

 											 "textures/Night/negy.jpg",
 											 "textures/Night/negx.jpg",
 											 "textures/Night/posx.jpg"
 											 };

	std::array<std::string, 6> palmTrees = { "textures/PalmTrees/posz.jpg",
 											 "textures/PalmTrees/negz.jpg",
 											 "textures/PalmTrees/posy.jpg",

 											 "textures/PalmTrees/negy.jpg",
 											 "textures/PalmTrees/negx.jpg",
 											 "textures/PalmTrees/posx.jpg"
 											 };	

	std::array<std::string, 6> coitTower = { "textures/CoitTower/posz.jpg",
 											 "textures/CoitTower/negz.jpg",
 											 "textures/CoitTower/posy.jpg",

 											 "textures/CoitTower/negy.jpg",
 											 "textures/CoitTower/negx.jpg",
 											 "textures/CoitTower/posx.jpg"
 											 };	

	std::array<std::string, 6> mountainPath = { "textures/MountainPath/posz.jpg",
 											 	"textures/MountainPath/negz.jpg",
 											 	"textures/MountainPath/posy.jpg",

 											 	"textures/MountainPath/negy.jpg",
 											 	"textures/MountainPath/negx.jpg",
 											 	"textures/MountainPath/posx.jpg"
 											 };		

	std::array<std::string, 6> nightPath = { "textures/NightPath/posz.jpg",
 											 "textures/NightPath/negz.jpg",
 											 "textures/NightPath/posy.jpg",

 											 "textures/NightPath/negy.jpg",
 											 "textures/NightPath/negx.jpg",
 											 "textures/NightPath/posx.jpg"
 											 };	

	std::array<std::string, 6> vasa = { "textures/Vasa/posz.jpg",
 										"textures/Vasa/negz.jpg",
 										"textures/Vasa/posy.jpg",

 										"textures/Vasa/negy.jpg",
 										"textures/Vasa/negx.jpg",
 										"textures/Vasa/posx.jpg"
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

	const std::string winterForestHDR = "textures/HDR/WinterForest/WinterForest.hdr";
	const std::string milkywayHDR = "textures/HDR/Milkyway/Milkyway.hdr";
	const std::string grandCanyonHDR = "textures/HDR/GrandCanyon/GrandCanyon.hdr";
	const std::string spaceHDR = "textures/HDR/Space/Space.hdr";
	const std::string factoryHDR = "textures/HDR/Factory/Factory.hdr";

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