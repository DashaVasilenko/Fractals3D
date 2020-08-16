#ifndef __SHADER_PROGRAM__
#define __SHADER_PROGRAM__

#include <stdio.h>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "window.h"
#include "types.h"

class ShaderProgram {
public:
    void Init(const std::map<GLenum, std::string>& );
    void Load();
    void Compile();
    void Link();
    void Run();
    void DeleteShaders();
    void Delete();

    void SetShaderParameters(bool c) { shader_parameters = c; }
    void SetFractalType(FractalType c) { currentFractalType = c; }
    void SetBackgroundType(BackgroundType c) { currentBackgroundType = c; }
    void SetIrradianceCubemap(bool c) { irradianceCubemap = c; }
    void SetColoringType(ColoringType c) { currentColoringType = c; }
    void SetToneMapping(bool c) { toneMapping = c; }

    void SetUniform(const char* name, const glm::vec2&);
    void SetUniform(const char* name, const glm::vec3&);
    void SetUniform(const char* name, const glm::vec4&);
    void SetUniform(const char* name, const glm::mat4&);
    void SetUniform(const char* name, float value);
    void SetUniform(const char* name, int value);

    FractalType GetFractalType() { return currentFractalType; }

private:
    std::string version = "#version 330 core\n";
    std::string defines = "";
    std::map<GLenum, std::string> mapSources;
    std::map<GLenum, GLuint> mapShaders;
    GLuint descriptor;

    bool shader_parameters = false;
    FractalType currentFractalType = FractalType::Test;
    BackgroundType currentBackgroundType = BackgroundType::SkyboxHDR;
    bool irradianceCubemap = true;
    ColoringType currentColoringType = ColoringType::Type1;
    bool toneMapping = false;
};

#endif /* End of __SHADERPROGRAM__ */