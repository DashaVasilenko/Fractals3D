#include <iostream>

#include "framebuffer.h"
#include "inputSystem.h"

FrameBuffer::FrameBuffer() {
    GLCall(glGenFramebuffers(1, &descriptor)); // create FBO (frame buffer objects) 1 - number of buffers
}

void FrameBuffer::BufferInit(int width, int  height) {
    Bind();
    this->width = width;
    this->height = height;
    // используем текстурные прикрепления для создания объектра буфера цвета
    tex_color_buf.Init(width, height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR);
    tex_color_buf.CreateAttachment(GL_COLOR_ATTACHMENT0);
    
    // создание объекта рендербуфера для совмещенных буфера глубины и трафарета
    GLCall(glGenRenderbuffers(1, &depth));
    GLCall(glBindRenderbuffer(GL_RENDERBUFFER, depth)); 
    GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height));  
    // присоединяем объект рендербуфера к совмещенной точке прикрепления глубины и трафарета буфера кадра
    GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth));

    //проверяем текущий привязанный кадровый буфер на завершенность
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    Unbind();  // отвязываем объект буфера кадра, чтобы случайно не начать рендер не туда, куда предполагалось
}

void FrameBuffer::Bind() const {
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, descriptor));  // привязываем как текущий активный буфер кадра 
}

void FrameBuffer::Resize(int width, int height) {
    // resize color attachment
    GLCall(glBindTexture(GL_TEXTURE_2D, tex_color_buf.GetDescriptor()));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));

    // resize depth attachment
    GLCall(glBindRenderbuffer(GL_RENDERBUFFER, depth));
    GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height));
    GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));

    // update internal dimensions
    this->width = width;
    this->height = height;
    InputSystem::isWindowSizeChange = false;
}

void FrameBuffer::Unbind() const {
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0)); // отвязываем буфера и возвращаем базовый кадровый буфер на место активного  
}

FrameBuffer::~FrameBuffer() {
    GLCall(glDeleteFramebuffers(1, &descriptor));  // delete buffer
}