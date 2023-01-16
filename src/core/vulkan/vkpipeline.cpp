#include "./vkpipeline.hpp"

using namespace core;

vkpipeline::vkpipeline() {}

vkpipeline::vkpipeline(
    std::vector<vkshader> shaders)
{
    this->create_shader_stage(shaders);
}

void vkpipeline::create_shader_stage(
    std::vector<vkshader> shaders)
{
    std::vector<VkPipelineShaderStageCreateInfo> shader_stages_create_info(shaders.size());

    for (auto &shader : shaders)
    {
        VkPipelineShaderStageCreateInfo shader_stage_info{};
        shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stage_info.stage = static_cast<VkShaderStageFlagBits>(shader.type);
        shader_stage_info.module = shader.handle;
        shader_stage_info.pName = "main";

        shader_stages_create_info.push_back(shader_stage_info);
    }
}

void vkpipeline::destroy() {
}