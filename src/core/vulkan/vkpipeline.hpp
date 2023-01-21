#pragma once

#include "./vkshader.hpp"
#include "./vkswapchain.hpp"

#include <vector>

namespace core {
    class vkpipeline {
    public:
        vkpipeline();
        vkpipeline(VkDevice, core::vkswapchain&, std::vector<vkshader>&);

        VkPipeline handle;
        VkPipelineLayout layout;
        VkRenderPass render_pass;

        void create_layout(VkDevice);
        void destroy(VkDevice);

    private:
        std::vector<VkPipelineShaderStageCreateInfo> get_shader_stage_infos(std::vector<vkshader>);
        VkPipelineDynamicStateCreateInfo get_dynamic_state_info(std::vector<VkDynamicState> &dynamic_states);
        VkPipelineVertexInputStateCreateInfo get_vertex_input_info();
        VkPipelineInputAssemblyStateCreateInfo get_input_assembly_info();
        VkPipelineRasterizationStateCreateInfo get_rasterization_state_info();
        VkPipelineMultisampleStateCreateInfo get_multisample_state_info();
        VkPipelineColorBlendStateCreateInfo get_color_blend_state_info(
            VkPipelineColorBlendAttachmentState*
        );
        VkPipelineColorBlendAttachmentState create_color_blend_attachment_state();
        VkPipelineViewportStateCreateInfo get_viewport_state_info(
            VkRect2D*, VkViewport*
        );

        void create_render_pass(VkDevice, core::vkswapchain&);
        
    };
}