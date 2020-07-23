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

    //void SetColor(bool c) { program->SetColor(c); }
    void SetFractalType(FractalType c) { program->SetFractalType(c); renderer->currentFractalType = c; }
    void SetMandelbulbIterations(int c) { renderer->fractalsParameters.mandelbulb_iterations = c;  }
    void SetMandelbulbBailout(float c) { renderer->fractalsParameters.mandelbulb_bailout = c; }
    void SetMandelbulbPower(float c) { renderer-> fractalsParameters.mandelbulb_power = c; }

private:
    Renderer* renderer;
    ShaderProgram* program;

};


#endif