#include "../thirdparty/glad/glad.h"
#include "../thirdparty/glm/glm.hpp"
#include "../thirdparty/glm/gtc/matrix_transform.hpp"
#include "../thirdparty/glm/gtc/type_ptr.hpp"
#include "../thirdparty/glfw/include/GLFW/glfw3.h"
#include "../dlancher/dlancher.h"

#include "shader.h"
#include "camera.h"
#include "object3d.h"

#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <unordered_map>

int check_glad() { if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl; return -1; } else return 0;}


int god_win_height = 1080;
int god_win_width = 1920;
ViewCamera god_camera;
GLFWwindow* god_window;
Object3DContainer objects_global;
float delta_time = 0.0f;
float last_frame = 0.0f;


void god_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{ god_win_width = width; god_win_height = height; glViewport(0, 0, width, height); }


void god_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{ god_camera.ProcessMouseScroll(yoffset); }


void god_mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    static bool firstMouse = true;
    static double pre_xpos = god_win_width / 2.0;
    static double pre_ypos = god_win_height / 2.0;
    if (firstMouse)
    {
        pre_xpos = xpos;
        pre_ypos = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - pre_xpos;
    float yoffset = pre_ypos - ypos;
    pre_xpos = xpos;
    pre_ypos = ypos;
    god_camera.ProcessMouseMovement(xoffset, yoffset);
}

void process_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        god_camera.ProcessKeyboard('w', delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        god_camera.ProcessKeyboard('s', delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        god_camera.ProcessKeyboard('a', delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        god_camera.ProcessKeyboard('d', delta_time);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        god_camera.ProcessKeyboard('c', delta_time);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        god_camera.ProcessKeyboard(' ', delta_time);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        god_camera.ProcessKeyboard('q', delta_time);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        god_camera.ProcessKeyboard('e', delta_time);
}

int main()
{
    dlancher::info();
    // init a god window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // opencl version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    god_window = glfwCreateWindow(god_win_width, god_win_height, "god-view", NULL, NULL);
    glfwMakeContextCurrent(god_window);
    check_glad();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    // bind god callback
    glfwSetFramebufferSizeCallback(god_window, god_framebuffer_size_callback);
    glfwSetScrollCallback(god_window, god_scroll_callback);
    glfwSetCursorPosCallback(god_window, god_mouse_callback);
    // set input mode
    glfwSetInputMode(god_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // create god shader
    Shader god_shader("engine/shader/vertex_base.glsl", "engine/shader/fragment_base.glsl");
    // create object
    auto str1 = objects_global.AddBufferBySTLFile("model3d/SHL_2pcs.stl", 0.01);
    auto str2 = objects_global.AddBufferBySTLFile("model3d/pcb_holder.stl", 0.01);
    objects_global.AddObject("11", str1);
    objects_global.AddObject("21", str2);
    objects_global.AddObject("12", str1);
    objects_global.TranslateTo("11", glm::vec3(0,0,-2.4));
    objects_global.TranslateTo("21", glm::vec3(0.1,0.1,-2.8));
    objects_global.TranslateTo("12", glm::vec3(0.1,0.5,-2.8));
    // render loop
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    while (!glfwWindowShouldClose(god_window))
    {
        float current_frame = glfwGetTime();
        // std::cout << currentFrame << std::endl;
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
        // input
        process_input(god_window);
        //render
        glViewport(0, 0, god_win_width, god_win_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        god_shader.SetVec3("light_color", glm::vec3(1.0, 1.0f, 1.0f));
        god_shader.SetVec3("light_direction", -god_camera.front());
        god_camera.RenderGL(god_shader, god_win_height, god_win_width);
        objects_global.RenderGL(god_shader);
        glfwSwapBuffers(god_window);
        glfwPollEvents();
    }
    objects_global.~Object3DContainer();
    glfwTerminate();
    return 0;
}
