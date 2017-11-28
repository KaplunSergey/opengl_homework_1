#include <glwrapper.h>
#include "glfw3.h"
#include <thread>
#include <iostream>
#include <glm/vec2.hpp>
#include "ShadersLoader.h"
#include <stb_image.h>

void initializeVao(GLuint vao, GLuint vbo, GLuint ebo, GLuint* indx, GLfloat* coor) {
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

GLuint initializeShaders(const std::string source) {
    auto vertexSource = shaders::loadShaderSourceFromFile(source.c_str());
    const char * verShadNative = vertexSource.c_str();
    auto shader = glCreateShader(GL_VERTEX_SHADER);
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

    auto vertexSource = shaders::loadShaderSourceFromFile("resources/shaders/vertex.glsl");
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

    auto fragmentSource = shaders::loadShaderSourceFromFile("resources/shaders/fragment.glsl");
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

        glDeleteProgram(program);
        glfwTerminate();
        return 0;
    }

    GLint circleProgramSuccess;
    glGetProgramiv(circleProgram, GL_LINK_STATUS, &circleProgramSuccess);
    if (!circleProgramSuccess) {
        GLchar info[512];
        glGetProgramInfoLog(circleProgram, sizeof(info), NULL, info);

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
             -0.3f, -0.3f, 0.0f, 1.0f,
             0.3f, -0.3f, 1.0f, 1.0f,
             0.3f, 0.5f,  1.0f, 0.0f,
             -0.3f, 0.5f,  0.0f, 0.0f
    };

    GLfloat leftHand[] = {
             -0.5f, -0.1f,  0.0f, 1.0f,
            -0.35f, -0.1f, 1.0f, 1.0f,
             -0.35f, 0.5f, 1.0f, 0.0f,
              -0.5f, 0.5f,  0.0f, 0.0f
    };

    GLfloat rightHand[] = {
            0.35f, -0.1f,  0.0f, 1.0f,
             0.5f, -0.1f, 1.0f, 1.0f,
              0.5f, 0.5f, 1.0f, 0.0f,
             0.35f, 0.5f,  0.0f, 0.0f
    };

    GLfloat leftLeg[] = {
            -0.3f, -0.9f,  0.0f, 1.0f,
            -0.1f, -0.9f, 1.0f, 1.0f,
            -0.1f, -0.3f, 1.0f, 0.0f,
            -0.3f, -0.3f,  0.0f, 0.0f
    };

    GLfloat rightLeg[] = {
            0.1f, -0.9f,  0.0f, 1.0f,
            0.3f, -0.9f, 1.0f, 1.0f,
            0.3f, -0.3f, 1.0f, 0.0f,
            0.1f, -0.3f,  0.0f, 0.0f
    };

    glm::vec2 center = {0.f, 0.7f};
    float radius = 0.2f;

    GLfloat circle[] = {
            center.x - radius, center.y - radius, 0.0f, 1.0f,
            center.x + radius, center.y - radius, 1.0f, 1.0f,
            center.x + radius, center.y + radius, 1.0f, 0.0f,
            center.x - radius, center.y + radius,  0.0f, 0.0f
    };

    GLfloat background[] = {
            -1.f, -1.f,  0.0f, 1.0f,
            1.f, -1.f,  1.0f, 1.0f,
            1.f, 1.f,  1.0f, 0.0f,
            -1.f, 1.f,  0.0f, 0.0f,
    };

    GLuint backgroundVao;
    GLuint bodyVao;
    GLuint leftHandVao;
    GLuint rightHandVao;
    GLuint leftLegVao;
    GLuint rightLegVao;
    GLuint circleVao;

    GLuint ebo;
    GLuint vbo[7];

    glGenBuffers(7, vbo);
    glGenBuffers(1, &ebo);

    //Textures

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
    auto data = stbi_load("resources/soad.jpg", &textureWidth, &textureHeight, &textureChannelsCount, STBI_rgb);

    if (data == NULL) {
        std::cerr << "ERROR:\n" << stbi_failure_reason();
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    GLuint texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    textureWidth;
    textureHeight;
    textureChannelsCount;
    data = stbi_load("resources/jeans.jpg", &textureWidth, &textureHeight, &textureChannelsCount, STBI_rgb);

    if (data == NULL) {
        std::cerr << "ERROR:\n" << stbi_failure_reason();
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    GLuint texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    textureWidth;
    textureHeight;
    textureChannelsCount;
    data = stbi_load("resources/hand1.jpg", &textureWidth, &textureHeight, &textureChannelsCount, STBI_rgb);

    if (data == NULL) {
        std::cerr << "ERROR:\n" << stbi_failure_reason();
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    GLuint texture3;
    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    textureWidth;
    textureHeight;
    textureChannelsCount;
    data = stbi_load("resources/kenny.jpg", &textureWidth, &textureHeight, &textureChannelsCount, STBI_rgb);

    if (data == NULL) {
        std::cerr << "ERROR:\n" << stbi_failure_reason();
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    GLuint texture4;
    glGenTextures(1, &texture4);
    glBindTexture(GL_TEXTURE_2D, texture4);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    textureWidth;
    textureHeight;
    textureChannelsCount;
    data = stbi_load("resources/background.jpg", &textureWidth, &textureHeight, &textureChannelsCount, STBI_rgb);

    if (data == NULL) {
        std::cerr << "ERROR:\n" << stbi_failure_reason();
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    //Vao

    glGenVertexArrays(1, &backgroundVao);
    glBindVertexArray(backgroundVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(background), background, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, reinterpret_cast<void *>(sizeof(GLfloat) * 0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, reinterpret_cast<void *>(sizeof(GLfloat) * 2));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indx), indx, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glGenVertexArrays(1, &bodyVao);
    glBindVertexArray(bodyVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(body), body, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, reinterpret_cast<void *>(sizeof(GLfloat) * 0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, reinterpret_cast<void *>(sizeof(GLfloat) * 2));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indx), indx, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glGenVertexArrays(1, &leftHandVao);
    glBindVertexArray(leftHandVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(leftHand), leftHand, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, reinterpret_cast<void *>(sizeof(GLfloat) * 0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, reinterpret_cast<void *>(sizeof(GLfloat) * 2));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indx), indx, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glGenVertexArrays(1, &rightHandVao);
    glBindVertexArray(rightHandVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rightHand), rightHand, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, reinterpret_cast<void *>(sizeof(GLfloat) * 0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, reinterpret_cast<void *>(sizeof(GLfloat) * 2));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indx), indx, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glGenVertexArrays(1, &leftLegVao);
    glBindVertexArray(leftLegVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(leftLeg), leftLeg, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, reinterpret_cast<void *>(sizeof(GLfloat) * 0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, reinterpret_cast<void *>(sizeof(GLfloat) * 2));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indx), indx, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glGenVertexArrays(1, &rightLegVao);
    glBindVertexArray(rightLegVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rightLeg), rightLeg, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, reinterpret_cast<void *>(sizeof(GLfloat) * 0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, reinterpret_cast<void *>(sizeof(GLfloat) * 2));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indx), indx, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glGenVertexArrays(1, &circleVao);
    glBindVertexArray(circleVao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, reinterpret_cast<void *>(sizeof(GLfloat) * 0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, reinterpret_cast<void *>(sizeof(GLfloat) * 2));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture4);
        glUniform1i(glGetUniformLocation(program, "image"), 0);
        glBindVertexArray(backgroundVao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(program, "image"), 0);
        glBindVertexArray(bodyVao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(program, "image"), 0);
        glBindVertexArray(leftLegVao);
        glUniform4f(color, 0.0, 0.0, 1.0, 1.0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glBindVertexArray(rightLegVao);
        glUniform4f(color, 0.0, 0.0, 1.0, 1.0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1i(glGetUniformLocation(program, "image"), 0);

        glBindVertexArray(leftHandVao);
        glUniform4f(color, 1.0, 1.0, 1.0, 1.0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glBindVertexArray(rightHandVao);
        glUniform4f(color, 1.0, 1.0, 1.0, 1.0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


        glUseProgram(circleProgram);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glUniform1i(glGetUniformLocation(program, "image"), 0);

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