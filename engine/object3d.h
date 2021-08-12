#pragma once

#include "../thirdparty/glad/glad.h"
#include "../thirdparty/glm/glm.hpp"
#include "../thirdparty/glm/gtc/matrix_transform.hpp"
#include "../thirdparty/glm/gtc/type_ptr.hpp"
#include "../thirdparty/glfw/include/GLFW/glfw3.h"

#include "shader.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>


struct ObjectInfo {
    glm::mat4 model;
    bool is_glbuffer;
    bool is_cuda;
    float *pdata;
    float *pdata_cuda;
    size_t len;
    glm::vec3 color;
    glm::vec3 f0;
    float ao;
    float roughness;
    float metallic;
    unsigned int vbo;
    ObjectInfo() {}
    ObjectInfo(glm::mat4 &model_, float *pdata_, size_t len_) : model(model_), 
        is_glbuffer(false), is_cuda(false), pdata(pdata_), pdata_cuda(nullptr), 
        len(len_), color(glm::vec3(0.1f, 0.4f, 0.4f)), f0(glm::vec3(1.00, 0.71, 0.29)), 
        ao(0.4f), roughness(0.4f), metallic(0.4f) {}
};


class Object3DContainer
{
private:
    std::vector<unsigned int> vaos_;
    std::unordered_map<unsigned int, ObjectInfo> map_info_;
public:
    Object3DContainer() {}
    ~Object3DContainer() {
        while(!vaos_.empty()) {
            unsigned int back_index = vaos_.size() - 1;
            DeleteObjectByIndex(back_index, false);
        }
    }
    void DeleteObjectByIndex(int idx, bool del_gl=true) {
        unsigned int vao = vaos_[idx];
        delete[] map_info_[vao].pdata;
        if (del_gl) glDeleteBuffers(1, &map_info_[vao].vbo);
        vaos_.erase(vaos_.begin() + idx);
        map_info_.erase(vao);
        if (del_gl) glDeleteVertexArrays(1, &vao);
    }
    int AddObjectBySTLFIle(const char *stl_file_path, float scale=1.0) {
        std::ifstream file(stl_file_path, std::ios::binary | std::ios::in);
        if (!file) return -1;
        file.ignore(80);
        int num_tris;
        file.read((char*)(&num_tris), sizeof(int));
        unsigned int len = num_tris*18;
        float *pdata = new float[len];
        size_t pos = 0;
        for (int i = 0; i < num_tris; i++) {
            float norm[3];
            float tri[9];
            file.read((char*)norm, 3*sizeof(float));
            file.read((char*)tri, 9*sizeof(float));
            for(int j=0; j<3; j++) pdata[pos++] = tri[j]*scale;
            for(int j=0; j<3; j++) pdata[pos++] = norm[j]*scale;
            for(int j=3; j<6; j++) pdata[pos++] = tri[j]*scale;
            for(int j=0; j<3; j++) pdata[pos++] = norm[j]*scale;
            for(int j=6; j<9; j++) pdata[pos++] = tri[j]*scale;
            for(int j=0; j<3; j++) pdata[pos++] = norm[j]*scale;
            file.ignore(2);
        }
        glm::mat4 model = glm::mat4(1.0f);
        unsigned int vao;
        glGenVertexArrays(1, &vao);
        vaos_.push_back(vao);
        map_info_[vao] = ObjectInfo(model, pdata, len);
        return 0;
    }

    void RenderGL(Shader &shader) {
        shader.Use();
        for (auto vao : vaos_) {
            glBindVertexArray(vao);
            auto &map_info_vao = map_info_[vao];
            if (!map_info_vao.is_glbuffer) {
                glGenBuffers(1, &map_info_vao.vbo);
                glBindBuffer(GL_ARRAY_BUFFER, map_info_vao.vbo);
                glBufferData(GL_ARRAY_BUFFER, map_info_vao.len*sizeof(map_info_vao.pdata[0]), 
                    map_info_vao.pdata, GL_STATIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
                glEnableVertexAttribArray(1);
                map_info_vao.is_glbuffer = true;
            }
            shader.SetMat4("model", map_info_vao.model);
            shader.SetVec3("object_color", map_info_vao.color);
            shader.SetVec3("object_f0", map_info_vao.f0);
            shader.SetFloat("ao", map_info_vao.ao);
            shader.SetFloat("roughness", map_info_vao.roughness);
            shader.SetFloat("metallic", map_info_vao.metallic);
            glDrawArrays(GL_TRIANGLES, 0, map_info_vao.len/6);
        }
        glBindVertexArray(0);
    }

    void TranslateToByIndex(const int idx, const glm::vec3 &vec) {
        auto &map_info_vao = map_info_[vaos_[idx]];
        auto model = glm::mat4(1.0f); 
        model = glm::translate(model, vec);
        map_info_vao.model = model;
    }

    void RotateToByIndex(const int idx, const float degree, const glm::vec3 &axis) {
        auto &map_info_vao = map_info_[vaos_[idx]];
        auto model = glm::mat4(1.0f); 
        model = glm::rotate(model, glm::radians(degree), axis);
        map_info_vao.model = model;
    }
};