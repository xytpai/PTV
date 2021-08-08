#ifndef CAMERA_H
#define CAMERA_H

#include "../thirdparty/glad/glad.h"
#include "../thirdparty/glm/glm.hpp"
#include "../thirdparty/glm/gtc/matrix_transform.hpp"

#include <vector>

class Camera
{
private:
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    float yaw_;
    float pitch_;
    float roll_;
    void UpdateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front.y = sin(glm::radians(pitch_));
        front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front_ = glm::normalize(front);
        glm::vec3 camera_world_up;
        camera_world_up.x = cos(glm::radians(roll_));
        camera_world_up.y = sin(glm::radians(roll_));
        camera_world_up.z = 0.0f;
        right_ = glm::normalize(glm::cross(front_, camera_world_up));
        up_    = glm::normalize(glm::cross(right_, front_));
    }
public:
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
        float yaw = -90.0f, float pitch = 0.0f, float roll = 90.0f
    ) : position_(position), yaw_(yaw), pitch_(pitch), roll_(roll) {
        UpdateCameraVectors();}
    glm::vec3 position() const {return position_;}
    float yaw() const {return yaw_;}
    float pitch() const {return pitch_;}
    float roll() const {return roll_;}
    glm::vec3 front() const {return front_;}
    glm::vec3 up() const {return up_;}
    glm::vec3 right() const {return right_;}
    void set_position(glm::vec3 position) {position_ = position;}
    void set_yaw(float yaw) {yaw_ = yaw; UpdateCameraVectors();}
    void set_pitch(float pitch) {pitch_ = pitch; UpdateCameraVectors();}
    void set_roll(float roll) {roll_ = roll; UpdateCameraVectors();}
    void set_ypr(float yaw, float pitch, float roll) {
        yaw_ = yaw; pitch_ = pitch; roll_ = roll; UpdateCameraVectors();}
    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(position_, position_ + front_, up_);}
};

class ViewCamera : public Camera
{
private:
    float movement_speed_;
    float mouse_sensitivity_;
    float zoom_;
public:
    ViewCamera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
        float yaw = -90.0f, float pitch = 0.0f, float roll = 90.0f
    ) : Camera(position, yaw, pitch, roll), 
        movement_speed_(2.5f), mouse_sensitivity_(0.03f), zoom_(45.0f) {}
    float movement_speed() const {return movement_speed_;}
    float mouse_sensitivity() const {return mouse_sensitivity_;}
    float zoom() const {return zoom_;}
    void set_movement_speed(float movement_speed) {movement_speed_ = movement_speed;}
    void set_mouse_sensitivity(float mouse_sensitivity) {mouse_sensitivity_ = mouse_sensitivity;}
    void set_zoom(float zoom) {zoom_ = zoom;}
    void ProcessMouseScroll(float yoffset)
    {
        zoom_ -= yoffset;
        if (zoom_ < 1.0f) zoom_ = 1.0f;
        if (zoom_ > 45.0f) zoom_ = 45.0f; 
    }
    void ProcessMouseMovement(float xoffset, float yoffset)
    {
        xoffset *= mouse_sensitivity_;
        yoffset *= mouse_sensitivity_;
        float new_yaw = yaw() + xoffset;
        float new_pitch = pitch() + yoffset;
        if (new_pitch > 89.0f) new_pitch = 89.0f;
        if (new_pitch < -89.0f) new_pitch = -89.0f;
        set_ypr(new_yaw, new_pitch, roll());
    }
    void ProcessKeyboard(char ch, float delta_time)
    {
        float velocity = movement_speed_ * delta_time;
        if (ch == 'w') set_position(position() + front() * velocity);
        else if (ch == 's') set_position(position() - front() * velocity);
        else if (ch == 'a') set_position(position() - right() * velocity);
        else if (ch == 'd') set_position(position() + right() * velocity);
        else if (ch == 'c') set_position(position() - up() * velocity);
        else if (ch == ' ') set_position(position() + up() * velocity);
        else if (ch == 'q') set_roll(roll() + mouse_sensitivity_*20);
        else if (ch == 'e') set_roll(roll() - mouse_sensitivity_*20);
    }
    glm::mat4 GetProjectionMatrix(float win_width, float win_height)
    {
        return glm::perspective(glm::radians(zoom()), 
            win_width / win_height, 0.1f, 100.0f);
    }
};

#endif