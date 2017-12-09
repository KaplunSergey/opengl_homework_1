#pragma once

#include <glm.hpp>
#include "glwrapper.h"

enum Direction {
    FORWARD,
    BACKWARD,
    RIGHT,
    LEFT,
    UP,
    DOWN
};

class Camera {

public:
    Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));

    void moveCamera(const Direction &direction, const GLfloat &delta);

    void rotateCamera(const GLfloat &xOffset, const GLfloat &yOffset);

    glm::mat4 getViewMatrix() const;

private:

    void updateValues();

private:

    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 cameraUp;
    glm::vec3 direction;
    glm::vec3 right;

    GLfloat yawValue;
    GLfloat pitchValue;

    GLfloat movementSpeed;
    GLfloat rotationSensitivity;

    glm::mat4 viewMatrix;
};