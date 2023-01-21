#include "./vkpipeline.hpp"

#include "utils/assert.hpp"

#include <iostream>

using namespace core;

vkpipeline::vkpipeline() {}

vkpipeline::vkpipeline(
    VkDevice device,
    vkswapchain& swapchain,
    std::vector<vkshader>& shaders
) {
    std::vector<VkDynamicState> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    this->create_layout(device);
    this->create_render_pass(device, swapchain);

    VkRect2D scissor;
    scissor.extent = swapchain.extent;
    scissor.offset = {};

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width  = swapchain.extent.width;
    viewport.height = swapchain.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    auto shader_stages_info     = get_shader_stage_infos(shaders);
    auto dynamic_state_info     = get_dynamic_state_info(dynamic_states);
    auto vertex_input_info      = get_vertex_input_info();
    auto input_assembly_info    = get_input_assembly_info();
    auto rasterization_info     = get_rasterization_state_info();    
    auto multisample_info       = get_multisample_state_info();
    auto viewport_state_info    = get_viewport_state_info(&scissor, &viewport);

    auto color_blend_attachment_info = create_color_blend_attachment_state();
    auto color_blend_info = get_color_blend_state_info(&color_blend_attachment_info);

    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = shader_stages_info.size();
    pipeline_info.pStages = shader_stages_info.data();
    pipeline_info.layout = layout;
    pipeline_info.renderPass = render_pass;
    pipeline_info.subpass = 0;

    pipeline_info.pDynamicState = &dynamic_state_info;
    pipeline_info.pViewportState = &viewport_state_info;
    pipeline_info.pColorBlendState = &color_blend_info;
    pipeline_info.pMultisampleState = &multisample_info;
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pRasterizationState = &rasterization_info;
    pipeline_info.pInputAssemblyState = &input_assembly_info;
    pipeline_info.pDepthStencilState = nullptr;

    VK_ASSERT(
        vkCreateGraphicsPipelines(
            device,
            VK_NULL_HANDLE, // no cache for now
            1,
            &pipeline_info,
            nullptr,
            &handle
        )
    );
}

void vkpipeline::create_render_pass(
    VkDevice device,
    vkswapchain& swapchain
) {
    VkAttachmentDescription color_attachment{};
    color_attachment.format = swapchain.format.format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

    // clear color attachment when framebuffer is loaded
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    // since we want to see stuff, we store operations
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    // since we aren't using stencil buffers, we don't care
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    // since we're clearing the attachment, we don't care about layout
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // we're going to show the image, so it's final layout must be
    // present
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // we're only using one Attach Descriptor (color_attachment)
    // it's index is 0 and is used for color
    VkAttachmentReference color_attach_ref{};
    color_attach_ref.attachment = 0;
    color_attach_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass_description{};
    subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_description.colorAttachmentCount = 1;
    subpass_description.pColorAttachments = &color_attach_ref;
    // the index of the attachment corresponds to the
    // layout(location=X) index of shader 

    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass_description;

    VK_ASSERT(
        vkCreateRenderPass(
            device,
            &render_pass_info,
            nullptr,
            &render_pass
        )
    );
}

VkPipelineViewportStateCreateInfo vkpipeline::get_viewport_state_info(
    VkRect2D* scissor_pointer, VkViewport* viewport_pointer
) {
    VkPipelineViewportStateCreateInfo viewport_state_info{};

    viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_info.scissorCount = 1;
    viewport_state_info.pScissors    = scissor_pointer;
    viewport_state_info.viewportCount = 1;
    viewport_state_info.pViewports    = viewport_pointer;

    return viewport_state_info;
}

void vkpipeline::create_layout(VkDevice device) {
    VkPipelineLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.setLayoutCount = 0;
    layout_info.pSetLayouts = nullptr;
    layout_info.pushConstantRangeCount = 0;
    layout_info.pPushConstantRanges = nullptr;

    VK_ASSERT(
        vkCreatePipelineLayout(
            device,
            &layout_info,
            nullptr,
            &layout 
        )
    );
}

VkPipelineColorBlendStateCreateInfo vkpipeline::get_color_blend_state_info(
    VkPipelineColorBlendAttachmentState* color_blend_attachment_state
) {
    VkPipelineColorBlendStateCreateInfo color_blend_info{};

    color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_info.attachmentCount = 1;
    color_blend_info.pAttachments = color_blend_attachment_state;
    
    // disable logicOp as it overwrites blending specified in color blend attachment
    color_blend_info.logicOpEnable = VK_FALSE;
    color_blend_info.logicOp = VK_LOGIC_OP_COPY;
    color_blend_info.blendConstants[0] = 0.0f;
    color_blend_info.blendConstants[1] = 0.0f;
    color_blend_info.blendConstants[2] = 0.0f;
    color_blend_info.blendConstants[3] = 0.0f;

    return color_blend_info;
}

VkPipelineColorBlendAttachmentState vkpipeline::create_color_blend_attachment_state() {
    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.colorWriteMask = 
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_TRUE;

    // final_color = (in_color * alpha) + (out_color * (1 - alpha))
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;

    // final_alpha = (in_alpha * 1) + (out_alpha * 0) = in_alpha
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

    return color_blend_attachment;
}

VkPipelineMultisampleStateCreateInfo vkpipeline::get_multisample_state_info() {
    VkPipelineMultisampleStateCreateInfo multisample_create_info{};

    multisample_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_create_info.sampleShadingEnable = VK_FALSE;
    multisample_create_info.alphaToCoverageEnable = VK_FALSE;
    multisample_create_info.alphaToOneEnable = VK_FALSE;
    multisample_create_info.minSampleShading = 1.0f;
    multisample_create_info.pSampleMask = nullptr;
    multisample_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    return multisample_create_info;
}

VkPipelineRasterizationStateCreateInfo vkpipeline::get_rasterization_state_info() {
    VkPipelineRasterizationStateCreateInfo rasterization_state_info{};
    rasterization_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    // instead of discarded, too far/close objects get clamped (dumb + needs extension)
    rasterization_state_info.depthClampEnable = VK_FALSE;
    // auto discard fragments
    rasterization_state_info.rasterizerDiscardEnable = VK_FALSE;
    rasterization_state_info.polygonMode = VK_POLYGON_MODE_FILL;
    
    // for drawing lines or points, width above 1.0 requires extension
    rasterization_state_info.lineWidth = 1.0f;
    
    // defines to cull back faces, and determines back faces by vertex specification
    rasterization_state_info.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization_state_info.frontFace = VK_FRONT_FACE_CLOCKWISE;

    // the rasterizer can alter depth values by a constante factor or if in slope
    // won't be using
    rasterization_state_info.depthBiasEnable = VK_FALSE;
    rasterization_state_info.depthBiasConstantFactor = 0.0f;
    rasterization_state_info.depthBiasClamp = 0.0f;
    rasterization_state_info.depthBiasSlopeFactor = 0.0f;

    return rasterization_state_info;
}

VkPipelineInputAssemblyStateCreateInfo vkpipeline::get_input_assembly_info() {
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info{};
    input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_info.primitiveRestartEnable = VK_FALSE;
    return input_assembly_info;
}

//TODO fill in this struct, not doing now as we're hardcoding the
// vertices in shaders
VkPipelineVertexInputStateCreateInfo vkpipeline::get_vertex_input_info() {
    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexAttributeDescriptionCount   = 0;
    vertex_input_info.pVertexAttributeDescriptions      = nullptr;
    vertex_input_info.vertexBindingDescriptionCount     = 0;
    vertex_input_info.pVertexBindingDescriptions        = nullptr;
    return vertex_input_info;
}

std::vector<VkPipelineShaderStageCreateInfo> vkpipeline::get_shader_stage_infos(
    std::vector<vkshader> shaders
) {
    std::vector<VkPipelineShaderStageCreateInfo> shader_stages_create_info;

    for (auto &shader : shaders)
    {
        VkPipelineShaderStageCreateInfo shader_stage_info{};
        shader_stage_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stage_info.stage  = shader.stage_flags;
        shader_stage_info.module = shader.handle;
        shader_stage_info.pName  = "main";

        shader_stages_create_info.push_back(shader_stage_info);
    }

    return shader_stages_create_info;
}

VkPipelineDynamicStateCreateInfo vkpipeline::get_dynamic_state_info(std::vector<VkDynamicState>& dynamic_states) {
    VkPipelineDynamicStateCreateInfo dynamic_state_info{};
    dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_info.dynamicStateCount = dynamic_states.size();
    dynamic_state_info.pDynamicStates = dynamic_states.data();
    return dynamic_state_info;
}

void vkpipeline::destroy(VkDevice device) {
    vkDestroyPipelineLayout(device, layout, nullptr);
    vkDestroyRenderPass(device, render_pass, nullptr);
    vkDestroyPipeline(device, handle, nullptr);
}