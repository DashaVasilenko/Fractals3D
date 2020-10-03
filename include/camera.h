#ifndef __CAMERA__
#define __CAMERA__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "types.h"

class Camera {
public:
    Camera(const glm::vec3& = glm::vec3(0.0f, 0.0f, 3.0f) );
    void Update();

    glm::mat4 GetViewMatrix();
    glm::vec3 GetPosition() { return position; }
    glm::vec3 GetUp() { return up; }
    glm::vec3 GetRight() { return right; }
    glm::vec3 GetFront() { return front;}
    float GetFieldOfView() { return fieldOfView; }
    CameraType GetCameraType() { return currentCameraType; } 
    float GetPitch() { return pitch; }
    float GetYaw() { return yaw; }
    float GetPhi() { return phi; }
    float GetTheta() { return theta; }
    float GetR() { return r; } 
     
    void SetDeltaTime(float t) { deltaTime = t; }
    void SetViewMatrix(glm::mat4 m) { right = glm::vec3(m[0]); up = glm::vec3(m[1]); front = glm::vec3(m[2]); position = glm::vec3(m[3]); }
    void SetPosition(glm::vec3 c) { position = c; }
    void SetUp(glm::vec3 c) { up = c; }
    void SetRight(glm::vec3 c) { right = c; }
    void SetFront(glm::vec3 c) { front = c;}
    void SetFieldOfView(float c) { fieldOfView = c; }
    void SetPitch(float c) { pitch = c; }
    void SetYaw(float c) { yaw = c; }
    void SetPhi(float c) { phi = c; }
    void SetTheta(float c) { theta = c; }
    void SetR(float c) { r = c; } 
    void SetCameraType(CameraType t);

private:
    CameraType currentCameraType = CameraType::SphericalCamera;

    void UpdatePosition(const float&);
    void UpdateVectors(const float&);

    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 front;

    float pitch = 0.0f; // тангаж (наклон вверх вниз)
    //float roll = 0.0f; // крен
    float yaw = -90.0f; // рыскание (поворот влево вправо)
    float fieldOfView = 90.0f;

    glm::vec3 position;
    float phi;
    float theta;
    float r;

    float speed = 15.0f; //5.0f
    float mouse_sense = 0.5f;
    float deltaTime = 0.0f;
};

#endif /* End of __CAMERA__ */