#ifndef __SHADER_PROGRAM__
#define __SHADER_PROGRAM__

#include "window.h"
#include <map>

class ShaderProgram {
public:
    void Init(const std::map<GLenum, std::string>& );
    void Compile();
    void Link();
    void Run();
    void Delete();
private:
    std::map<GLenum, std::string> mapSources;
    std::map<GLenum, GLuint> mapShaders;
    GLuint descriptor;
};

#endif /* End of __SHADERPROGRAM__ */