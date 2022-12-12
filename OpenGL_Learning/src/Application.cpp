#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>


#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void processInput(GLFWwindow * window);

static void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL error] (" << error << ")" << function <<
            " " << file << " : " <<  line << std::endl;
        return false;
    }
    return true;
}

static int getSign(float val) {
    if (val >= 1.0f || val <= 0.0f) {
        return -1;
    }
    return 1;
}

// stores how much we're seeing of either texture
float mixValue = 0.2f;

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1080, 1080, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW init error";
    }

    std::cout << glGetString(GL_VERSION) << std::endl;
    float vertices[] = {
     // -----poistion----    ------color----- --texture cord---
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 2.0f, 0.0f,  // 右下
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  // 左下
        -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 2.0f,  // 左上
         0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 2.0f, 2.0f   // 右上   
    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };
   
    unsigned int VBO, VAO, EBO;

    GLCall(glGenVertexArrays(1, &VAO));
    GLCall(glGenBuffers(1, &VBO));
    GLCall(glGenBuffers(1, &EBO));
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    GLCall(glBindVertexArray(VAO));

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    // 位置属性
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));
    GLCall(glEnableVertexAttribArray(0));
    // 颜色属性
    GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))));
    glEnableVertexAttribArray(1);
    // texture cord attribute
    GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))));
    glEnableVertexAttribArray(2);

    unsigned int texture1, texture2;

    // load texture1 
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("res/texture/container.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);

    // load texture2
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("res/texture/snowboarding.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);

    GLCall(auto shader = Shader("res/shaders/Basic.shader"));

    // PolygonMode
    // GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

    // Add shader program
    GLCall(shader.use());


    float x_offset = 0.0f;
    float inc4x = 0.001f;
    shader.setFloat("xOffset", x_offset);

    float y_offset = 0.0f;
    float inc4y = 0.002f;
    shader.setFloat("yOffset", y_offset);


    // Keep changing color for the fragment shader
    /*
    int location = glGetUniformLocation(shader, "u_Color");
    ASSERT(location != -1);
    float r = 0.3f, g = 0.8f, b = 0.2f;
    float r_inc = 0.003f, g_inc = 0.003f, b_inc = 0.003f;
    */


    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    shader.use(); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:
    shader.setInt("texture1", 0);
    // or set it via the texture class
    shader.setInt("texture2", 1);

    // Unblind everything
    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // Bind texture
        GLCall(glActiveTexture(GL_TEXTURE0));
        GLCall(glBindTexture(GL_TEXTURE_2D, texture1));
        GLCall(glActiveTexture(GL_TEXTURE1));
        GLCall(glBindTexture(GL_TEXTURE_2D, texture2));

        GLCall(shader.use());
        //GLCall(glUniform4f(location, r, g, b, 1.0f));

        GLCall(glBindVertexArray(VAO));

        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

        x_offset += inc4x;
        if (x_offset > 0.5f || x_offset < -0.5f) {
            inc4x *= -1;
        }
        shader.setFloat("xOffset", x_offset);

        y_offset += inc4y;
        if (y_offset > 0.5f || y_offset < -0.5f) {
            inc4y *= -1;
        }
        shader.setFloat("yOffset", y_offset);

        shader.setFloat("mixValue", mixValue);
        /*
        r_inc *= getSign(r);
        g_inc *= getSign(g);
        b_inc *= getSign(b);
        r += r_inc;
        g += g_inc;
        b += b_inc;
        */

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose
    GLCall(glDeleteVertexArrays(1, &VAO));
    GLCall(glDeleteBuffers(1, &VBO));
    GLCall(glDeleteBuffers(1, &EBO));
    GLCall(glDeleteProgram(shader.ID));

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
static void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        mixValue += 0.01f;
        if (mixValue >= 1.0f)
            mixValue = 1.0f;
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        mixValue -= 0.01f;
        if (mixValue <= 0.0f)
            mixValue = 0.0f;
    }
}