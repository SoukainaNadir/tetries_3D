#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;

    Camera(glm::vec3 pos = {0.0f, 10.0f, 15.0f},
           glm::vec3 tgt = {0.0f, 0.0f, 0.0f},
           glm::vec3 upv = {0.0f, 1.0f, 0.0f})
        : position(pos), target(tgt), up(upv) {}

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, target, up);
    }

    glm::mat4 getProjectionMatrix(float aspect) const {
        return glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
    }
};
