#ifndef __SKYBOX__
#define __SKYBOX__

#include <iostream>
#include <string>
#include <array>
#include <GL/glew.h>
#include "stb_image.h"
#include "errors.h"

class SkyBox {
public:
    void Load(const std::array<std::string, 6>& fileNames);
    unsigned int GetDescriptor() { return descriptor; }
    ~SkyBox();

private:
    unsigned int descriptor;

};

#endif /* End of __SKYBOX__ */