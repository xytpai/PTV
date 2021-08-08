#ifndef OBJECT3D
#define OBJECT3D

#include "../thirdparty/glad/glad.h"
#include "../thirdparty/glm/glm.hpp"
#include "../thirdparty/glm/gtc/matrix_transform.hpp"
#include "../thirdparty/glm/gtc/type_ptr.hpp"

class Object3D
{
private:
    glm::mat4 model_;
    float *data_;
    int len_;
public:
    Object3D(const float *data, const int len) {
        data_ = new float[len]; len_ = len; memcpy(data_, data, len*sizeof(float));}
    Object3D() {model_ = glm::mat4(1.0f); data_=nullptr;}
    ~Object3D() {delete []data_;}
    float *data() const {return data_;}
    int len() const {return len_;}
    glm::mat4 model() const {return model_;}
    void set_model(const glm::mat4 &model) {model_=model;}
    void TranslateSelf(const glm::vec3 &vec) {model_ = glm::translate(model_, vec);}
    void TranslateTo(const glm::vec3 &vec) {model_ = glm::mat4(1.0f); model_ = glm::translate(model_, vec);}
    void RotateSelf(const float degree, const glm::vec3 &axis) {
        model_ = glm::rotate(model_, glm::radians(degree), axis);}
    void RotateTo(const float degree, const glm::vec3 &axis) {
        model_ = glm::mat4(1.0f); model_ = glm::rotate(model_, glm::radians(degree), axis);}
};

#endif