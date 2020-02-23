#include "renderer.h"

void Renderer::Init() {
 	glViewport(0, 0, width, height);
}

void Renderer::Update() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
}