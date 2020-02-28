#ifndef __SHADER_PROGRAM__
#define __SHADER_PROGRAM__

#include "window.h"
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include "errors.h"

class ShaderProgram {
public:
    void Init(const std::map<GLenum, std::string>& );
    void Compile();
    void Link();
    void Run();
    void Delete();

    void SetUniform(const char* name, const glm::vec2&);
    void SetUniform(const char* name, const glm::mat4&);
    void SetUniform(const char* name, float value);

private:
    std::map<GLenum, std::string> mapSources;
    std::map<GLenum, GLuint> mapShaders;
    GLuint descriptor;
};

#endif /* End of __SHADERPROGRAM__ */