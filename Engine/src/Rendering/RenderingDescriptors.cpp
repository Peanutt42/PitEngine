#include "pch.hpp"
#include "RenderingDescriptors.hpp"

using namespace Pit::Rendering;

DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::addBinding(uint32 binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32 count) {
    assert(bindings.count(binding) == 0 && "Binding already in use");
    VkDescriptorSetLayoutBinding layoutBinding {
        .binding = binding,
        .descriptorType = descriptorType,
        .descriptorCount = count,
        .stageFlags = stageFlags
    };
    bindings[binding] = layoutBinding;
    return *this;
}

std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::build() const {
    return std::make_unique<DescriptorSetLayout>(device, bindings);
}




DescriptorSetLayout::DescriptorSetLayout(
    Device& device, std::unordered_map<uint32, VkDescriptorSetLayoutBinding> bindings)
    : device{ device }, bindings{ bindings } {
    Array<VkDescriptorSetLayoutBinding> setLayoutBindings{};
    for (auto& kv : bindings)
        setLayoutBindings.push_back(kv.second);

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = static_cast<uint32>(setLayoutBindings.size()),
        .pBindings = setLayoutBindings.data()
    };

    if (vkCreateDescriptorSetLayout(
        device.device(),
        &descriptorSetLayoutInfo,
        nullptr,
        &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

DescriptorSetLayout::~DescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(device.device(), descriptorSetLayout, nullptr);
}




DescriptorPool::Builder& DescriptorPool::Builder::addPoolSize(
    VkDescriptorType descriptorType, uint32 count) {
    poolSizes.push_back({ descriptorType, count });
    return *this;
}

DescriptorPool::Builder& DescriptorPool::Builder::setPoolFlags(
    VkDescriptorPoolCreateFlags flags) {
    poolFlags = flags;
    return *this;
}
DescriptorPool::Builder& DescriptorPool::Builder::setMaxSets(uint32 count) {
    maxSets = count;
    return *this;
}

std::unique_ptr<DescriptorPool> DescriptorPool::Builder::build() const {
    return std::make_unique<DescriptorPool>(device, maxSets, poolFlags, poolSizes);
}




DescriptorPool::DescriptorPool(
    Device& device,
    uint32 maxSets,
    VkDescriptorPoolCreateFlags poolFlags,
    const Array<VkDescriptorPoolSize>& poolSizes)
    : device{ device } {
    VkDescriptorPoolCreateInfo descriptorPoolInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = poolFlags,
        .maxSets = maxSets,
        .poolSizeCount = static_cast<uint32>(poolSizes.size()),
        .pPoolSizes = poolSizes.data()
    };

    if (vkCreateDescriptorPool(device.device(), &descriptorPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
        PIT_ENGINE_FATAL(Log::Rendering, "failed to create descriptor pool!");
}

DescriptorPool::~DescriptorPool() {
    vkDestroyDescriptorPool(device.device(), descriptorPool, nullptr);
}

bool DescriptorPool::allocateDescriptor(
    const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const {
    VkDescriptorSetAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorSetLayout
    };

    return vkAllocateDescriptorSets(device.device(), &allocInfo, &descriptor) == VK_SUCCESS;
}

void DescriptorPool::freeDescriptors(Array<VkDescriptorSet>& descriptors) const {
    vkFreeDescriptorSets(device.device(),
                         descriptorPool,
                         static_cast<uint32>(descriptors.size()),
                         descriptors.data());
}

void DescriptorPool::resetPool() {
    vkResetDescriptorPool(device.device(), descriptorPool, 0);
}




DescriptorWriter::DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool)
    : setLayout{ setLayout }, pool{ pool } {}

DescriptorWriter& DescriptorWriter::writeBuffer(
    uint32 binding, VkDescriptorBufferInfo* bufferInfo) {
    assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto& bindingDescription = setLayout.bindings[binding];

    assert(
        bindingDescription.descriptorCount == 1 &&
        "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstBinding = binding,
        .descriptorCount = 1,
        .descriptorType = bindingDescription.descriptorType,
        .pBufferInfo = bufferInfo
    };

    writes.push_back(write);
    return *this;
}

DescriptorWriter& DescriptorWriter::writeImage(
    uint32 binding, VkDescriptorImageInfo* imageInfo) {
    assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

    auto& bindingDescription = setLayout.bindings[binding];

    assert(
        bindingDescription.descriptorCount == 1 &&
        "Binding single descriptor info, but binding expects multiple");

    VkWriteDescriptorSet write {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstBinding = binding,
        .descriptorCount = 1,
        .descriptorType = bindingDescription.descriptorType,
        .pImageInfo = imageInfo
    };

    writes.push_back(write);
    return *this;
}

bool DescriptorWriter::build(VkDescriptorSet& set) {
    bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
    if (!success)
        return false;
    
    overwrite(set);
    return true;
}

void DescriptorWriter::overwrite(VkDescriptorSet& set) {
    for (auto& write : writes)
        write.dstSet = set;
    
    vkUpdateDescriptorSets(pool.device.device(), Cast<uint32>(writes.size()), writes.data(), 0, nullptr);
}