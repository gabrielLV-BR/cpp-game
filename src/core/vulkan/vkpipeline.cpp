#include "./vkpipeline.hpp"

#include "utils/assert.hpp"

using namespace core;

vkpipeline::vkpipeline() {}

vkpipeline::vkpipeline(
    VkDevice device,
    std::vector<vkshader> shaders
) {
    std::vector<VkDynamicState> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    this->create_layout(device);

    auto shader_stages_info     = get_shader_stage_infos(shaders);
    auto dynamic_state_info     = get_dynamic_state_info(dynamic_states);
    auto vertex_input_info      = get_vertex_input_info();
    auto input_assembly_info    = get_input_assembly_info();
    auto rasterization_info     = get_rasterization_state_info();    
    auto multisample_info       = get_multisample_state_info();

    auto color_blend_attachment_info = create_color_blend_attachment_state();
    auto color_blend_info = get_color_blend_state_info(color_blend_attachment_info);
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
    VkPipelineColorBlendAttachmentState color_blend_attachment_state
) {
    VkPipelineColorBlendStateCreateInfo color_blend_info{};

    color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_info.attachmentCount = 1;
    color_blend_info.pAttachments = &color_blend_attachment_state;
    
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
    vkDestroyPipeline(device, handle, nullptr);
}