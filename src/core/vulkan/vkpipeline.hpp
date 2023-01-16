#pragma once

#include "./vkshader.hpp"

#include <vector>

namespace core {
    class vkpipeline {
    public:
        vkpipeline();
        vkpipeline(std::vector<vkshader> shaders);

        void create_shader_stage(std::vector<vkshader> shaders);     

        void destroy();
    };
}