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


struct BufferInfo {
    unsigned int vao;
    unsigned int vbo;
    float *pdata;
    float *pdata_cuda;
    size_t len;
    bool is_glbuffer;
    bool is_cuda;
    unsigned int count;
    std::string identity;
    BufferInfo() : vao(0), vbo(0), pdata(nullptr), pdata_cuda(nullptr), 
        len(0), is_glbuffer(false), is_cuda(false), count(0) {}
};


struct ObjectInfo {
    glm::mat4 model;
    glm::vec3 color;
    glm::vec3 f0;
    float ao;
    float roughness;
    float metallic;
    BufferInfo *pbuffer;
    ObjectInfo() : model(glm::mat4(1.0f)), color(glm::vec3(0.1f, 0.4f, 0.4f)), 
        f0(glm::vec3(1.00, 0.71, 0.29)), ao(0.4f), roughness(0.4f), metallic(0.4f), 
        pbuffer(nullptr) {}
};


class Object3DContainer
{
private:
    std::unordered_map<std::string, BufferInfo> map_identity2buffer_;
    std::unordered_map<std::string, ObjectInfo> map_name2object_;
public:
    Object3DContainer() {}
    ~Object3DContainer() {
        for (auto & info : map_identity2buffer_) {
            auto &buffer = info.second;
            delete[] buffer.pdata;
            glDeleteVertexArrays(1, &buffer.vao);
            glDeleteBuffers(1, &buffer.vbo);
        }
    }

    void DeleteObject(const std::string name, bool del_gl=true) {
        auto &object = map_name2object_[name];
        auto &pbuffer = object.pbuffer;
        pbuffer->count--;
        if (pbuffer->count <= 0) {
            delete[] pbuffer->pdata;
            if (del_gl) {
                glDeleteBuffers(1, &pbuffer->vbo);
                glDeleteVertexArrays(1, &pbuffer->vao);
            }
            map_identity2buffer_.erase(pbuffer->identity);
        }
        map_name2object_.erase(name);
    }

    std::string AddBufferBySTLFile(const char *stl_file_path, float scale=1.0) {
        std::ifstream file(stl_file_path, std::ios::binary | std::ios::in);
        if (!file) return "";
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
        std::string identity = std::string(stl_file_path);
        BufferInfo info;
        info.len = len;
        info.pdata = pdata;
        info.vao = vao;
        info.identity = identity;
        map_identity2buffer_[identity] = info;
        return identity;
    }

    int AddObject(std::string name, std::string identity) {
        ObjectInfo info;
        info.pbuffer = &map_identity2buffer_[identity];
        map_name2object_[name] = info;
        map_identity2buffer_[identity].count++;
        return 0;
    }

    void RenderGL(Shader &shader) {
        shader.Use();
        for (auto & info : map_name2object_) {
            auto &object = info.second;
            auto &pbuffer = object.pbuffer;
            glBindVertexArray(pbuffer->vao);
            if (!pbuffer->is_glbuffer) {
                glGenBuffers(1, &pbuffer->vbo);
                glBindBuffer(GL_ARRAY_BUFFER, pbuffer->vbo);
                glBufferData(GL_ARRAY_BUFFER, pbuffer->len*sizeof(pbuffer->pdata[0]), 
                    pbuffer->pdata, GL_STATIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
                glEnableVertexAttribArray(1);
                pbuffer->is_glbuffer = true;
            }
            shader.SetMat4("model", object.model);
            shader.SetVec3("object_color", object.color);
            shader.SetVec3("object_f0", object.f0);
            shader.SetFloat("ao", object.ao);
            shader.SetFloat("roughness", object.roughness);
            shader.SetFloat("metallic", object.metallic);
            glDrawArrays(GL_TRIANGLES, 0, pbuffer->len/6);
        }
        glBindVertexArray(0);
    }

    void TranslateTo(std::string name, const glm::vec3 &vec) {
        auto &object = map_name2object_[name];
        auto model = glm::mat4(1.0f); 
        model = glm::translate(model, vec);
        object.model = model;
    }

    void RotateToByIndex(std::string name, const float degree, const glm::vec3 &axis) {
        auto &object = map_name2object_[name];
        auto model = glm::mat4(1.0f); 
        model = glm::rotate(model, glm::radians(degree), axis);
        object.model = model;
    }
};