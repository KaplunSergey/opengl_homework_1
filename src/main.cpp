#include <glwrapper.h>
#include "glfw3.h"
#include <thread>
#include <iostream>
#include <glm/vec2.hpp>
#include "ShadersLoader.h"

void initializeVao(GLuint &vao, GLuint &vbo, GLuint &ebo, GLuint* indx, GLfloat* coor) ;

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

#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error initializing glew. Exiting.\n";
        return -1;
    }
#endif

    auto vertexSource = shaders::loadShaderSourceFromFile("resources/shaders/ver.glsl");
    const char * verShadNative = vertexSource.c_str();
    auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &verShadNative, nullptr);
    glCompileShader(vertexShader);

    GLint vertexShaderSuccess;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexShaderSuccess);
    if (!vertexShaderSuccess) {
        GLchar info[512];
        glGetShaderInfoLog(vertexShader, sizeof(info), NULL, info);
        std::cerr << "ERROR::SHADER LINK_FAILED\n" << info;
        return 0;
    }

    auto fragmentSource = shaders::loadShaderSourceFromFile("resources/shaders/frag.glsl");
    const char * fragShadNative = fragmentSource.c_str();
    auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragShadNative, nullptr);
    glCompileShader(fragmentShader);

    GLint fragmentShaderSuccess;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentShaderSuccess);
    if (!fragmentShaderSuccess) {
        GLchar info[512];
        glGetShaderInfoLog(fragmentShader, sizeof(info), NULL, info);
        std::cerr << "ERROR::SHADER LINK_FAILED\n" << info;
        return 0;
    }

    auto circleFragmentSource = shaders::loadShaderSourceFromFile("resources/shaders/circle_fragment.glsl");
    const char * circleFragmentNative = circleFragmentSource.c_str();
    auto circleFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(circleFragmentShader, 1, &circleFragmentNative, nullptr);
    glCompileShader(circleFragmentShader);

    GLint circleFragmentSuccess;
    glGetShaderiv(circleFragmentShader, GL_COMPILE_STATUS, &circleFragmentSuccess);
    if (!circleFragmentSuccess) {
        GLchar info[512];
        glGetShaderInfoLog(circleFragmentShader, sizeof(info), NULL, info);
        std::cerr << "ERROR::SHADER LINK_FAILED\n" << info;
        return 0;
    }

    auto program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    auto circleProgram = glCreateProgram();
    glAttachShader(circleProgram, vertexShader);
    glAttachShader(circleProgram, circleFragmentShader);
    glLinkProgram(circleProgram);

    GLint programSuccess;
    glGetProgramiv(program, GL_LINK_STATUS, &programSuccess);
    if (!programSuccess) {
        GLchar info[512];
        glGetProgramInfoLog(program, sizeof(info), NULL, info);
        std::cerr << vertexSource << std::endl;
        std::cerr << fragmentSource << std::endl;

        glDeleteProgram(program);
        glfwTerminate();
        return 0;
    }

    GLint circleProgramSuccess;
    glGetProgramiv(circleProgram, GL_LINK_STATUS, &circleProgramSuccess);
    if (!circleProgramSuccess) {
        GLchar info[512];
        glGetProgramInfoLog(circleProgram, sizeof(info), NULL, info);
        std::cerr << vertexSource << std::endl;
        std::cerr << circleFragmentSource << std::endl;

        glDeleteProgram(circleProgram);
        glfwTerminate();
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(circleFragmentShader);
    glDeleteShader(fragmentShader);

    glViewport(0,0, width, height);

    GLuint indx[] = { 0, 1, 2, 0, 2, 3};

    GLfloat body[] = {
            -0.3f, -0.3f,
             0.3f, -0.3f,
             0.3f, 0.5f,
            -0.3f, 0.5f
    };

    GLfloat leftHand[] = {
            -0.5f, -0.1f,
            -0.35f, -0.1f,
            -0.35f, 0.5f,
            -0.5f, 0.5f
    };

    GLfloat rightHand[] = {
            0.35f, -0.1f,
            0.5f, -0.1f,
            0.5f, 0.5f,
            0.35f, 0.5f
    };

    GLfloat leftLeg[] = {
            -0.3f, -0.9f,
            -0.1f, -0.9f,
            -0.1f, -0.3f,
            -0.3f, -0.3f
    };

    GLfloat rightLeg[] = {
            0.1f, -0.9f,
            0.3f, -0.9f,
            0.3f, -0.3f,
            0.1f, -0.3f
    };

    glm::vec2 center = {0.f, 0.7f};
    float radius = 0.2f;

    GLfloat circle[] = {
            center.x - radius, center.y - radius,
            center.x + radius, center.y - radius,
            center.x + radius, center.y + radius,
            center.x - radius, center.y + radius
    };

    GLuint bodyVao;
    GLuint leftHandVao;
    GLuint rightHandVao;
    GLuint leftLegVao;
    GLuint rightLegVao;
    GLuint circleVao;

    GLuint ebo;
    GLuint vbo[6];

    glGenBuffers(6, vbo);
    glGenBuffers(1, &ebo);

    glGenVertexArrays(1, &bodyVao);
    glBindVertexArray(bodyVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(body), body, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indx), indx, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glGenVertexArrays(1, &leftHandVao);
    glBindVertexArray(leftHandVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(leftHand), leftHand, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indx), indx, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glGenVertexArrays(1, &rightHandVao);
    glBindVertexArray(rightHandVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rightHand), rightHand, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indx), indx, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glGenVertexArrays(1, &leftLegVao);
    glBindVertexArray(leftLegVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(leftLeg), leftLeg, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indx), indx, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glGenVertexArrays(1, &rightLegVao);
    glBindVertexArray(rightLegVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rightLeg), rightLeg, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indx), indx, GL_STATIC_DRAW);

    glBindVertexArray(0);

    //initializeVao(circleVao, vbo[5], ebo, indx, circle);

    glGenVertexArrays(1, &circleVao);
    glBindVertexArray(circleVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indx), indx, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLint radiusUniform = glGetUniformLocation(circleProgram, "radius");
    GLint size = glGetUniformLocation(circleProgram, "windowSize");
    GLint circleColor = glGetUniformLocation(circleProgram, "color");
    GLint color = glGetUniformLocation(program, "outColor");
    GLint circleCenter = glGetUniformLocation(circleProgram, "center");


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(1.f, 0.5f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);

        glBindVertexArray(bodyVao);
        glUniform4f(color, 0.0, 0.0, 0.0, 1.0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glBindVertexArray(leftHandVao);
        glUniform4f(color, 1.0, 1.0, 1.0, 1.0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glBindVertexArray(rightHandVao);
        glUniform4f(color, 1.0, 1.0, 1.0, 1.0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glBindVertexArray(leftLegVao);
        glUniform4f(color, 0.0, 0.0, 1.0, 1.0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glBindVertexArray(rightLegVao);
        glUniform4f(color, 0.0, 0.0, 1.0, 1.0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glUseProgram(circleProgram);

        glUniform2f(size, width, height);
        glUniform1f(radiusUniform, 0.2f);
        glUniform4f(circleColor, 0.0, 1.0, 0.0, 1.0);
        glUniform2f(circleCenter, center.x, center.y);

        glBindVertexArray(circleVao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        std::this_thread::sleep_for(std::chrono::nanoseconds(15000));
    }

    glfwTerminate();
    return 0;
}

void initializeVao(GLuint &vao, GLuint &vbo, GLuint &ebo, GLuint* indx, GLfloat* coor) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coor), coor, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indx), indx, GL_STATIC_DRAW);

    glBindVertexArray(0);
}