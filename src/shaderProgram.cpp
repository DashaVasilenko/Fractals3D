#include "shaderProgram.h"
#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>

void ShaderProgram::Init(const std::map<GLenum, std::string>& mapSources) {
    this->mapSources = mapSources;
}

void ShaderProgram::Load() {
    // разбираемся с дефайнами до компиляции
    mapSources[GL_VERTEX_SHADER] = "glsl/quad_vertex.glsl";
    switch(currentFractalType) {
        case FractalType::Test: {
 	        mapSources[GL_FRAGMENT_SHADER] = "glsl/quad_pixel.glsl";
            break;
        }
        case FractalType::Mandelbulb: {
 	        mapSources[GL_FRAGMENT_SHADER] = "glsl/mandelbulb_pixel.glsl";
            break;
        }
        case FractalType::Juliabulb1: {
 	        mapSources[GL_FRAGMENT_SHADER] = "glsl/juliabulb_pixel.glsl";
            break;
        }
        case FractalType::Monster: {
 	        mapSources[GL_FRAGMENT_SHADER] = "glsl/monster_pixel.glsl";
            break;
        }
        case FractalType::Julia1: {
 	        mapSources[GL_FRAGMENT_SHADER] = "glsl/julia1_pixel.glsl";
            break;
        }
        case FractalType::Julia2: {
 	        mapSources[GL_FRAGMENT_SHADER] = "glsl/julia2_pixel.glsl";
            break;
        }
        case FractalType::Julia3: {
 	        mapSources[GL_FRAGMENT_SHADER] = "glsl/julia3_pixel.glsl";
            break;
        }
        case FractalType::Julia4: {
 	        mapSources[GL_FRAGMENT_SHADER] = "glsl/julia4_pixel.glsl";
            break;
        }
        case FractalType::Sierpinski1: {
            mapSources[GL_FRAGMENT_SHADER] = "glsl/sierpinski1_pixel.glsl";
            break;
        }
        case FractalType::Sierpinski2: {
            mapSources[GL_FRAGMENT_SHADER] = "glsl/sierpinski2_pixel.glsl";
            break;
        }
        case FractalType::MengerSponge1: {
            mapSources[GL_FRAGMENT_SHADER] = "glsl/menger_sponge1_pixel.glsl";
            break;
        }
        case FractalType::MengerSponge2: {
            mapSources[GL_FRAGMENT_SHADER] = "glsl/menger_sponge2_pixel.glsl";
            break;
        }
        case FractalType::Apollonian1: {
            mapSources[GL_FRAGMENT_SHADER] = "glsl/apollonian1_pixel.glsl";
            break;
        }
        case FractalType::Apollonian2: {
            mapSources[GL_FRAGMENT_SHADER] = "glsl/apollonian2_pixel.glsl";
            break;
        }
    }
    Init(mapSources);
    
    defines = "";
    if (shader_parameters)
        defines += "#define FLAG_SOFT_SHADOWS" + std::string("\n");
    
    /*
    switch(currentFractalType) {
        case FractalType::Test: {
            defines += "#define TEST" + std::string("\n");
            break;
        }
        case FractalType::Mandelbulb: {
            defines += "#define MANDELBULB" + std::string("\n");
            break;
        }
    }
    */
    switch(currentBackgroundType) {
        case BackgroundType::Solid: {
            defines += "#define SOLID_BACKGROUND" + std::string("\n");
            break;
        }
        case BackgroundType::SolidWithSun: {
            defines += "#define SOLID_BACKGROUND_WITH_SUN" + std::string("\n");
            break;
        }
        case BackgroundType::Skybox: {
            defines += "#define SKYBOX_BACKGROUND" + std::string("\n");
            break;
        }
        case BackgroundType::SkyboxHDR: {
            defines += "#define SKYBOX_BACKGROUND_HDR" + std::string("\n");
            //if (irradianceCubemap) 
            //    defines += "#define IRRADIANCE_CUBEMAP" + std::string("\n");
            break;
        }
    }
    if (irradianceCubemap) 
        defines += "#define IRRADIANCE_CUBEMAP" + std::string("\n");
    //std::cout << currentBackgroundType << std::endl;

    switch(currentColoringType) {
        case ColoringType::Type1: {
            defines += "#define COLORING_TYPE_1" + std::string("\n");
            break;
        }
        case ColoringType::Type2: {
            defines += "#define COLORING_TYPE_2" + std::string("\n");
            break;
        }
        case ColoringType::Type3: {
            defines += "#define COLORING_TYPE_3" + std::string("\n");
            break;
        }
        case ColoringType::Type4: {
            defines += "#define COLORING_TYPE_4" + std::string("\n");
            break;
        }
        case ColoringType::Type5: {
            defines += "#define COLORING_TYPE_5" + std::string("\n");
            break;
        }
        case ColoringType::Type6: {
            defines += "#define COLORING_TYPE_6" + std::string("\n");
            break;
        }
        case ColoringType::Type7: {
            defines += "#define COLORING_TYPE_7" + std::string("\n");
            break;
        }
    }
    
    Compile();
    Link();
    DeleteShaders();
}

void ShaderProgram::Compile() {
    for (auto& element: mapSources) {
        std::ifstream t(element.second);
 	    std::string source_cpp;
 	    t.seekg(0, std::ios::end);   
 	    source_cpp.reserve(t.tellg());
 	    t.seekg(0, std::ios::beg);
 	    source_cpp.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

        mapShaders[element.first] = glCreateShader(element.first);
        GLuint& shader_descriptor = mapShaders[element.first];

        const char *sources[3] = { version.c_str(), defines.c_str(), source_cpp.c_str() };
	    GLCall(glShaderSource(shader_descriptor, 3, sources, NULL));  
	    GLCall(glCompileShader(shader_descriptor)); // компилируем шейдер

 	    // проверка на ошибки при сборке шейдера
 	    GLint success;
 	    GLCall(glGetShaderiv(shader_descriptor, GL_COMPILE_STATUS, &success));
 	    if(!success) {
             GLint logLen;
             GLCall(glGetShaderiv(shader_descriptor, GL_INFO_LOG_LENGTH, &logLen));
             if (logLen > 0) {
                char *infoLog = new char[logLen];
                GLCall(glGetShaderInfoLog(shader_descriptor, logLen, NULL, infoLog));
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
                delete[] infoLog;
            }
 	    }
    }
}

void ShaderProgram::Link() {
    // создание программы 
 	descriptor = glCreateProgram();

    for (auto& element: mapShaders) {
        GLCall(glAttachShader(descriptor, element.second));
    }
 	GLCall(glLinkProgram(descriptor));

 	// проверка на ошибки при связывании
 	GLint success;
 	GLCall(glGetProgramiv(descriptor, GL_LINK_STATUS, &success));
 	if (!success) {
        GLint logLen;
        GLCall(glGetProgramiv(descriptor, GL_INFO_LOG_LENGTH, &logLen));
        if (logLen > 0) {
            char *infoLog = new char[logLen];
 		    GLCall(glGetProgramInfoLog(descriptor, logLen, NULL, infoLog));
            std::cout << "ERROR::SHADER::LINKING_FAILED\n" << infoLog << std::endl;
            delete[] infoLog;
        }
 	}
}

void ShaderProgram::Run() {
    GLCall(glUseProgram(descriptor));
}

void ShaderProgram::DeleteShaders() {
    for (auto& element: mapShaders) {
	    GLCall(glDeleteShader(element.second));
    }
}

/*
ShaderProgram::~ShaderProgram(){
    GLCall(glDeleteProgram(descriptor));
}
*/

void ShaderProgram::Delete() {
    //for (auto& element: mapShaders) {
 	//    GLCall(glDeleteShader(element.second));
    //}
    GLCall(glDeleteProgram(descriptor));
}

void ShaderProgram::SetUniform(const char* name, const glm::vec2& vector) {
    GLCall(GLint location = glGetUniformLocation(descriptor, name));
    GLCall(glUniform2f(location, vector.x, vector.y));
    if (location == -1) {
        std::cerr << "Uniform  " << std::string(name) + " " <<  location << " not found" << std::endl;
        exit(EXIT_FAILURE);
    }
    // обработка ошибок!!!
}

void ShaderProgram::SetUniform(const char* name, const glm::vec3& vector) {
    GLCall(GLint location = glGetUniformLocation(descriptor, name));
    GLCall(glUniform3f(location, vector.x, vector.y, vector.z));
    if (location == -1) {
        std::cerr << "Uniform  " << std::string(name) + " " <<  location << " not found" << std::endl;
        exit(EXIT_FAILURE);
    }
    // обработка ошибок!!!
}

void ShaderProgram::SetUniform(const char* name, const glm::vec4& vector) {
    GLCall(GLint location = glGetUniformLocation(descriptor, name));
    GLCall(glUniform4f(location, vector.x, vector.y, vector.z, vector.w));
    if (location == -1) {
        std::cerr << "Uniform  " << std::string(name) + " " <<  location << " not found" << std::endl;
        exit(EXIT_FAILURE);
    }
    // обработка ошибок!!!
}

void ShaderProgram::SetUniform(const char* name, const glm::mat4& matrix) {
    GLCall(GLint location = glGetUniformLocation(descriptor, name));
    GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix)));
    if (location == -1) {
        std::cerr << "Uniform  " << std::string(name) + " " << location << " not found" << std::endl;
        exit(EXIT_FAILURE);
    }
    // обработка ошибок!!!
}

void ShaderProgram::SetUniform(const char* name, float value) {
    GLCall(GLint location = glGetUniformLocation(descriptor, name));
    GLCall(glUniform1f(location, value));
    if (location == -1) {
        std::cerr << "Uniform  " << std::string(name) + " " <<  location << " not found" << std::endl;
        exit(EXIT_FAILURE);
    }
    // обработка ошибок!!!
}

void ShaderProgram::SetUniform(const char* name, int value) {
    GLCall(GLint location = glGetUniformLocation(descriptor, name));
    GLCall(glUniform1i(location, value));
    if (location == -1) {
        std::cerr << "Uniform  " << std::string(name) + " " <<  location << " not found" << std::endl;
        exit(EXIT_FAILURE);
    }
    // обработка ошибок!!!
}