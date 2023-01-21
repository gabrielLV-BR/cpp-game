#include "./vkshader.hpp"

using namespace core;

vkshader::vkshader()
{
    handle = VK_NULL_HANDLE;
}

vkshader::vkshader(
    VkDevice device,
    std::vector<char> source,
    VkShaderStageFlagBits shader_flags) : stage_flags(shader_flags)
{
    VkShaderModuleCreateInfo shader_create_info{};
    shader_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_create_info.pCode = reinterpret_cast<const uint32_t *>(source.data());
    shader_create_info.codeSize = source.size();

    VK_ASSERT(
        vkCreateShaderModule(
            device,
            &shader_create_info,
            nullptr,
            &handle));
}

void vkshader::destroy(VkDevice device)
{
    vkDestroyShaderModule(
        device, handle, nullptr);
}
