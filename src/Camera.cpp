#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up) :
        position(position),
        up(up),
        direction(direction),
        movementSpeed(2.5f),
        rotationSensitivity(0.005f),
        yawValue(270.0f),
        pitchValue(0.0f) {

    updateValues();
}

void Camera::moveCamera(const Direction &direction, const GLfloat &delta) {
    GLfloat calculatedOffset = movementSpeed * delta;

    switch (direction) {
        case FORWARD:
            position += calculatedOffset * this->direction;
            break;
        case BACKWARD:
            position -= calculatedOffset * this->direction;
            break;
        case LEFT:
            position += calculatedOffset * this->right;
            break;
        case RIGHT:
            position -= calculatedOffset * this->right;
            break;
        case UP:
            position += calculatedOffset * this->up;
            break;
        case DOWN:
            position -= calculatedOffset * this->up;
            break;
    }

    updateValues();
}

void Camera::rotateCamera(const GLfloat &xOffset, const GLfloat &yOffset) {
    GLfloat yaw = rotationSensitivity * xOffset;
    GLfloat pitch = rotationSensitivity * yOffset;

    yawValue += yaw;
    pitchValue += pitch;

    if (pitchValue > 89.0f) {
        pitchValue = 89.0f;
    } else if (pitchValue < -89.0f) {
        pitchValue = -89.0f;
    }

    yawValue = glm::mod(yawValue, 360.0f);

    updateValues();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + direction, cameraUp);
}

void Camera::updateValues() {

    glm::mat4 matPitch = glm::mat4(1.0f);
    glm::mat4 matYaw   = glm::mat4(1.0f);

    matPitch = glm::rotate(matPitch, pitchValue, glm::vec3(1.0f, 0.0f, 0.0f));
    matYaw = glm::rotate(matYaw, yawValue, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 rotationMatrix = matPitch * matYaw;

    glm::vec4 rightResult = rotationMatrix * glm::vec4(1,0,0,0);
    glm::vec4 upResult = rotationMatrix * glm::vec4(0,-1,0,0);
    glm::vec4 directionResult = rotationMatrix * glm::vec4(0,0,-1,0);

    direction = glm::vec3(directionResult.x, directionResult.y, directionResult.z);
    right = glm::vec3(rightResult.x, rightResult.y, rightResult.z);
    cameraUp = glm::vec3(upResult.x, upResult.y, upResult.z);
}