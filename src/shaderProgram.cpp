#include "shaderProgram.h"
#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>

void ShaderProgram::Init(const std::map<GLenum, std::string>& mapSources) {
    this->mapSources = mapSources;
}

void ShaderProgram::Load() {
    //bool flag = false;

    // разбираемся с дефайнами до компиляции
    if (color) 
        defines = "#define COLOR" + std::string("\n");
    else
        defines = "\n";
    /*
    auto it = shaderData.find("defines");
    if (it != shaderData.end()) {
        for (json::iterator iter = it->begin(); iter != it->end(); ++iter) {
            defines_source += "#define " + iter->get<std::string>() + std::string("\n");
        }
    }
    */

    //if (flag) {
        Compile();
        Link();
        DeleteShaders();
    //}
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
        //const char *sources[3] = { version.c_str(), "#define COLOR\n", source_cpp.c_str() };
	    GLCall(glShaderSource(shader_descriptor, 3, sources, NULL));  
	    GLCall(glCompileShader(shader_descriptor)); // компилируем шейдер

/*
 	    const char* source =  source_cpp.c_str();
 	    // привязываем исходный код шейдера к объекту шейдера (шейдер, кол-во строк, текст шейдера, NULL)
 	    GLCall(glShaderSource(shader_descriptor, 1, &source, NULL));  
 	    GLCall(glCompileShader(shader_descriptor)); // компилируем шейдер
*/

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
    //DeleteShaders();
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