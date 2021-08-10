#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/numpy.h"

#include "../thirdparty/glad/glad.h"
#include "../thirdparty/glm/glm.hpp"
#include "../thirdparty/glm/gtc/matrix_transform.hpp"
#include "../thirdparty/glm/gtc/type_ptr.hpp"
#include "../thirdparty/glfw/include/GLFW/glfw3.h"

#include "shader.h"
#include "camera.h"
#include "object3d.h"

#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <unordered_map>

namespace py = pybind11;


GLFWwindow* alloc_window(const char *name, const int h, const int w)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // opencl version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(w, h, name, NULL, NULL);
    glfwMakeContextCurrent(window);
    return window;
}


int check_glad()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    } else return 0;
}


void configure_window(GLFWwindow *window)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetScrollCallback(window, scroll_callback);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void main_loop(GLFWwindow *window)
{
    while (!glfwWindowShouldClose(window)) {
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
}


class Window
{
private:
    GLFWwindow *window;
public:
    Window(const char *name, const int h, const int w) {
        this->window = alloc_window(name, h, w);
        check_glad();
        configure_window(this->window);
    }
    ~Window() {
        glfwTerminate();
    }

    void run() {
        std::thread th(main_loop, this->window);
        th.detach();
    }
};


PYBIND11_MODULE(ptv, m) {
    m.doc() = "PTV is a prototype physics engine for robot auto-design scenario.";
    
    py::class_<Window>(m, "window") 
        .def(py::init<const char *, const int, const int>())
        .def("run", &Window::run);

}

