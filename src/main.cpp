#include <glwrapper.h>
#include "glfw3.h"
#include <thread>
#include <iostream>
#include <glm/vec2.hpp>
#include "ShadersLoader.h"
#include <stb_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include "Camera.h"

GLfloat lastCursorX;
GLfloat lastCursorY;
GLfloat deltaTime;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f));

void cursorPositionCallback(GLFWwindow *window, double x, double y) {
    auto xDelta = static_cast<GLfloat>(lastCursorX - x);
    auto yDelta = static_cast<GLfloat>(lastCursorY - y);

    lastCursorX = static_cast<GLfloat>(x);
    lastCursorY = static_cast<GLfloat>(y);

    camera.rotateCamera(xDelta, yDelta);
}

void processInput(GLFWwindow *window) {

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera.moveCamera(FORWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.moveCamera(BACKWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        camera.moveCamera(LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.moveCamera(RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.moveCamera(DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.moveCamera(UP, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

GLuint initializeVao(GLfloat* coor, GLuint size) {

    GLuint vbo;
    GLuint vao;

    glGenBuffers(1, &vbo);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * size, coor, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, reinterpret_cast<void *>(sizeof(GLfloat) * 0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, reinterpret_cast<void *>(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return vao;
}

GLuint initializeTexture(const char *imagePath) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    GLint textureWidth;
    GLint textureHeight;
    GLint textureChannelsCount;


    auto data = stbi_load(imagePath, &textureWidth, &textureHeight, &textureChannelsCount, STBI_rgb);

    if (data == NULL) {
        std::cerr << "ERROR:\n" << stbi_failure_reason();
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    return texture;
}

GLuint initializeShader(const char *shaderPath, GLenum shadeType) {
    auto vertexSource = shaders::loadShaderSourceFromFile(shaderPath);
    const char * verShadNative = vertexSource.c_str();
    auto shader = glCreateShader(shadeType);
    glShaderSource(shader, 1, &verShadNative, nullptr);
    glCompileShader(shader);

    GLint vertexShaderSuccess;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &vertexShaderSuccess);
    if (!vertexShaderSuccess) {
        GLchar info[512];
        glGetShaderInfoLog(shader, sizeof(info), NULL, info);
        std::cerr << "ERROR::SHADER LINK_FAILED\n" << info;
        return 0;
    }

    return shader;
}

GLuint createProgram(GLuint vertexShader, GLuint fragmentShader) {
    auto program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint programSuccess;
    glGetProgramiv(program, GL_LINK_STATUS, &programSuccess);
    if (!programSuccess) {
        GLchar info[512];
        glGetProgramInfoLog(program, sizeof(info), NULL, info);
        std::cerr << "ERROR::Programm\n" << info;
        glDeleteProgram(program);
        glfwTerminate();
        return 0;
    }

    return program;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    auto window = glfwCreateWindow(640, 480, "OpenGL", NULL, NULL);

    if (window == NULL) {
        std::cerr << "Can not create glfw window! \n";
        glfwTerminate();
        return -1;
    }

    int width;
    int height;
    glfwGetFramebufferSize(window, &width, &height);
    glfwMakeContextCurrent(window);

    lastCursorX = width / 2.0f;
    lastCursorY = height / 2.0f;

    glfwSetCursorPos(window, lastCursorX, lastCursorY);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursorPositionCallback);

#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error initializing glew. Exiting.\n";
        return -1;
    }
#endif


    auto vertexShader = initializeShader("resources/shaders/ver.glsl", GL_VERTEX_SHADER);
    auto fragmentShader = initializeShader("resources/shaders/frag.glsl", GL_FRAGMENT_SHADER);

    auto program = createProgram(vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glViewport(0,0, width, height);

    GLfloat coordinates[] = {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
    };

    //Textures

    GLuint texture = initializeTexture("resources/background.jpg");

    //Vao

   GLuint cubeVao = initializeVao(coordinates, 180);

    auto projectionMatrix = glm::perspective(glm::radians(45.0f), ((float) width / (float) height), 0.01f, 100.0f);
    glm::mat4 modelMatrix(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, -5.0f));

    while (!glfwWindowShouldClose(window)) {

        auto startTime = glfwGetTime();

        glfwPollEvents();

        processInput(window);

        glEnable(GL_DEPTH_TEST);

        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto tmp = glm::rotate(modelMatrix, (float) (glm::radians(45.0f) * glfwGetTime()), glm::vec3(0.6f, 0.6f, 0.2f));

        glUseProgram(program);

        glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_FALSE,
                           glm::value_ptr(projectionMatrix * camera.getViewMatrix()));
        glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(tmp));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(program, "image"), 0);

        glBindVertexArray(cubeVao);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        std::this_thread::sleep_for(std::chrono::nanoseconds(15000));
        deltaTime = static_cast<GLfloat>(glfwGetTime() - startTime);
    }

    glfwTerminate();
    return 0;
}