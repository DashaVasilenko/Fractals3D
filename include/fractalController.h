#ifndef __FRACTALCONTROLLER__
#define __FRACTALCONTROLLER__

#include "renderer.h"
#include "types.h"

class FractalController {
public:
    void Init(Renderer* r) { renderer = r; program = renderer->GetShaderProgram(); }

    FrameBuffer* GetFBO() { return renderer->GetFBO(); }
    Camera* GetCamera() { return renderer->GetCamera(); }
    ShaderProgram* GetShaderProgram() { return renderer->GetShaderProgram(); }

    void Render(int width, int height) { renderer->Render(width, height); }
    void LoadShaderProgram() { program->Load(); }

    void SetLightDirection(const glm::vec3& c) { renderer->fractalsParameters.light_direction = c; }
    void SetAmbientLightColor(const glm::vec3& c) { renderer->fractalsParameters.ambient_light_color = c; }
    void SetDiffuseLightColor(const glm::vec3& c) { renderer->fractalsParameters.diffuse_light_color = c; }
    void SetSpecularLightColor(const glm::vec3& c) { renderer->fractalsParameters.specular_light_color = c; }

    void SetAmbientFractalColor(const glm::vec3& c) { renderer->fractalsParameters.ambient_fractal_color = c; }
    void SetDiffuseFractalColor(const glm::vec3& c) { renderer->fractalsParameters.diffuse_fractal_color = c; }
    void SetSpecularFractalColor(const glm::vec3& c) { renderer->fractalsParameters.specular_fractal_color = c; }
    void SetFractalShininess(float c) { renderer->fractalsParameters.shininess = c; }

    void SetFractalType(FractalType c) { program->SetFractalType(c); renderer->currentFractalType = c; }
    void SetMandelbulbIterations(const int c) { renderer->fractalsParameters.mandelbulb_iterations = c;  }
    void SetMandelbulbBailout(const float c) { renderer->fractalsParameters.mandelbulb_bailout = c; }
    void SetMandelbulbPower(float c) { renderer-> fractalsParameters.mandelbulb_power = c; }

private:
    Renderer* renderer;
    ShaderProgram* program;

};


#endif