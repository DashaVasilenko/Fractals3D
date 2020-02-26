#include "shaderProgram.h"
#include <iostream>
#include <fstream>
#include <streambuf>

void ShaderProgram::Init(const std::map<GLenum, std::string>& mapSources) {
    this->mapSources = mapSources;
}

void ShaderProgram::Compile() {
    for (auto& element: mapSources) {
        std::ifstream t(element.second);
 	    std::string sourcecpp;
 	    t.seekg(0, std::ios::end);   
 	    sourcecpp.reserve(t.tellg());
 	    t.seekg(0, std::ios::beg);
 	    sourcecpp.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

        mapShaders[element.first] = glCreateShader(element.first);
        GLuint& shader_descriptor = mapShaders[element.first];

 	    const char* source =  sourcecpp.c_str();
 	    // привязываем исходный код шейдера к объекту шейдера (шейдер, кол-во строк, текст шейдера, NULL)
 	    glShaderSource(shader_descriptor, 1, &source, NULL);  
 	    glCompileShader(shader_descriptor); // компилируем шейдер

 	    // проверка на ошибки при сборке шейдера
 	    GLint success;
 	    glGetShaderiv(shader_descriptor, GL_COMPILE_STATUS, &success);
 	    if(!success) {
             GLint logLen;
             glGetShaderiv(shader_descriptor, GL_INFO_LOG_LENGTH, &logLen);
             if (logLen > 0) {
                char *infoLog = new char[logLen];
                glGetShaderInfoLog(shader_descriptor, logLen, NULL, infoLog);
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
        glAttachShader(descriptor, element.second);
    }
 	glLinkProgram(descriptor);

 	// проверка на ошибки при связывании
 	GLint success;
 	glGetProgramiv(descriptor, GL_LINK_STATUS, &success);
 	if (!success) {
        GLint logLen;
        glGetProgramiv(descriptor, GL_INFO_LOG_LENGTH, &logLen);
        if (logLen > 0) {
            char *infoLog = new char[logLen];
 		    glGetProgramInfoLog(descriptor, logLen, NULL, infoLog);
            std::cout << "ERROR::SHADER::LINKING_FAILED\n" << infoLog << std::endl;
            delete[] infoLog;
        }
 	}
}

void ShaderProgram::Run() {
    glUseProgram(descriptor);
}

void ShaderProgram::Delete() {
    for (auto& element: mapShaders) {
 	    glDeleteShader(element.second);
    }
    glDeleteProgram(descriptor);
}

void ShaderProgram::SetUniform(const char* name, const glm::vec2& vector) {
     GLint location = glGetUniformLocation(descriptor, name);
     glUniform2f(location, vector.x, vector.y);
     if (location == -1) {
         std::cerr << "Uniform  " << location << " not found" << std::endl;
         exit(EXIT_FAILURE);
     }
     // обработка ошибок!!!
 }