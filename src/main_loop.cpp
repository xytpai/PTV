#include "../thirdparty/glad/glad.h"
#include "../thirdparty/glm/glm.hpp"
#include "../thirdparty/glm/gtc/matrix_transform.hpp"
#include "../thirdparty/glm/gtc/type_ptr.hpp"
#include "../include/shader.h"
#include "../include/camera.h"
#include "../include/object3d.h"

#include <GLFW/glfw3.h>
#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
int win_width = 1920;
int win_height = 1080;
ViewCamera view_camera;
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float lastX = win_width / 2.0f;
float lastY = win_height / 2.0f;
bool firstMouse = false;


int main()
{
    view_camera.set_position(glm::vec3(0, 0, 1.0));
    // view_camera.set_movement_speed(20);
    
    Object3D cube("model3d/SHL_2pcs.stl", 0.01);

    
    cube.TranslateTo(glm::vec3(0.0f, 0.0f, 0.0f));
    cube.RotateTo(-80, glm::vec3(0,1,0));
    // cube.TranslateTo(glm::vec3(0.0f, 0.0f, 0.0f));
    
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // opencl version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(win_width, win_height, "Demo", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // build and compile our shader program
    Shader vf_shader("shaders/vertex_base.glsl", "shaders/fragment_base.glsl");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, cube.len()*sizeof(cube.data()[0]), cube.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);  
    glBindVertexArray(0);

    // make sure to initialize matrix to identity matrix first
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // glPolygonMode(GL_FRONT, GL_FILL);
    // glPolygonMode(GL_BACK, GL_LINE);
    
    

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        // std::cout<<currentFrame<<std::endl;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        // activate shader
        vf_shader.Use();

        glm::mat4 view = view_camera.GetViewMatrix();
        // glm::mat4 projection = glm::perspective(glm::radians(view_camera.zoom()), (float)win_width / (float)win_height, 0.1f, 100.0f);
        glm::mat4 projection = view_camera.GetProjectionMatrix(win_width, win_height);

        // pass them to the shaders
        vf_shader.SetMat4("model", cube.model());
        vf_shader.SetMat4("view", view);
        vf_shader.SetMat4("projection", projection);
        vf_shader.SetVec3("object_color", glm::vec3(0.1f, 0.4f, 0.0f));
        vf_shader.SetVec3("light_color", glm::vec3(1.0, 1.0f, 1.0f));
        vf_shader.SetVec3("light_direction", glm::vec3(-1.0f, 1.0f, -1.0f));
        vf_shader.SetVec3("camera_position", view_camera.position());
        vf_shader.SetVec3("object_f0", 1.00, 0.71, 0.29);
        vf_shader.SetFloat("ao", 0.4f);
        vf_shader.SetFloat("roughness", 0.4f);
        vf_shader.SetFloat("metallic", 0.4f);




        // render object
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, cube.get_num_point());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        view_camera.ProcessKeyboard('w', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        view_camera.ProcessKeyboard('s', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        view_camera.ProcessKeyboard('a', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        view_camera.ProcessKeyboard('d', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        view_camera.ProcessKeyboard('c', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        view_camera.ProcessKeyboard(' ', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        view_camera.ProcessKeyboard('q', deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        view_camera.ProcessKeyboard('e', deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    win_width = width;
    win_height = height;
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    view_camera.ProcessMouseScroll(yoffset);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    view_camera.ProcessMouseMovement(xoffset, yoffset);
}
