#ifndef __CAMERA__
#define __CAMERA__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
    Camera(const glm::vec3& = glm::vec3(0.0f, 0.0f, 3.0f) );
    void Update();
    glm::mat4 GetViewMatrix();
    float GetFieldOfView() { return fieldOfView; }
    void SetDeltaTime(float t) { deltaTime = t; }

private:
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

    float speed = 5.0f;
    float mouse_sense = 0.5f;
    float deltaTime = 0.0f;
};

#endif /* End of __CAMERA__ */