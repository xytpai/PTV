#ifndef OBJECT3D
#define OBJECT3D

#include <iostream>
#include <fstream>

#include "../thirdparty/glad/glad.h"
#include "../thirdparty/glm/glm.hpp"
#include "../thirdparty/glm/gtc/matrix_transform.hpp"
#include "../thirdparty/glm/gtc/type_ptr.hpp"

class Object3D
{
private:
    glm::mat4 model_;
    float *data_; // x0,y0,z0,nx0,ny0,nz0,x1,...
    size_t len_;
public:
    Object3D(const float *data, size_t len) {
        data_ = new float[len]; len_ = len; memcpy(data_, data, len*sizeof(float));}
    Object3D(const char *stl_file_path, float scale=1.0) {
        std::ifstream file(stl_file_path, std::ios::binary | std::ios::in);
        if (!file) {data_=NULL; len_=0; return;}
        file.ignore(80);
        int num_tris;
        file.read((char*)(&num_tris), sizeof(int));
        len_ = num_tris*18;
        data_ = new float[len_];
        size_t pos = 0;
        for (int i = 0; i < num_tris; i++) {
            float norm[3];
            float tri[9];
            file.read((char*)norm, 3*sizeof(float));
            file.read((char*)tri, 9*sizeof(float));
            for(int j=0; j<3; j++) data_[pos++] = tri[j]*scale;
            for(int j=0; j<3; j++) data_[pos++] = norm[j]*scale;
            for(int j=3; j<6; j++) data_[pos++] = tri[j]*scale;
            for(int j=0; j<3; j++) data_[pos++] = norm[j]*scale;
            for(int j=6; j<9; j++) data_[pos++] = tri[j]*scale;
            for(int j=0; j<3; j++) data_[pos++] = norm[j]*scale;
            file.ignore(2);
        }
    }
    Object3D() {model_ = glm::mat4(1.0f); data_=nullptr;}
    ~Object3D() {delete []data_;}
    float *data() const {return data_;}
    size_t len() const {return len_;}
    glm::mat4 model() const {return model_;}
    void set_model(const glm::mat4 &model) {model_=model;}
    void TranslateSelf(const glm::vec3 &vec) {model_ = glm::translate(model_, vec);}
    void TranslateTo(const glm::vec3 &vec) {model_ = glm::mat4(1.0f); model_ = glm::translate(model_, vec);}
    void RotateSelf(const float degree, const glm::vec3 &axis) {
        model_ = glm::rotate(model_, glm::radians(degree), axis);}
    void RotateTo(const float degree, const glm::vec3 &axis) {
        model_ = glm::mat4(1.0f); model_ = glm::rotate(model_, glm::radians(degree), axis);}
    int get_num_point() const {return len_/6;}
};

#endif