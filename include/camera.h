#ifndef __CAMERA__
#define __CAMERA__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
    Camera(const glm::vec3& = glm::vec3(0.0f, 0.0f, 3.0f) );
    void Update(const float&);
    glm::mat4 GetViewMatrix();
private:
    void UpdatePosition(const float&);
    void UpdateVectors();

    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 front;

    float pitch = 0.0f;
    float roll = 0.0f;
    float yaw = -90.0f;

    glm::vec3 position;
    float speed = 5.0f;
    float mouse_sense = 0.1f;
};

#endif /* End of __CAMERA__ */