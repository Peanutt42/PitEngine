#pragma once

#include "Core/CoreInclude.hpp"
#include "RenderingDevice.hpp"

namespace Pit::Rendering {
    class DescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(Device& device) : device{ device } {}

            Builder& addBinding(
                uint32 binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32 count = 1);
            std::unique_ptr<DescriptorSetLayout> build() const;

        private:
            Device& device;
            std::unordered_map<uint32, VkDescriptorSetLayoutBinding> bindings{};
        };

        DescriptorSetLayout(
            Device& device, std::unordered_map<uint32, VkDescriptorSetLayoutBinding> bindings);
        ~DescriptorSetLayout();
        DescriptorSetLayout(const DescriptorSetLayout&) = delete;
        DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        Device& device;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32, VkDescriptorSetLayoutBinding> bindings;

        friend class DescriptorWriter;
    };

    class DescriptorPool {
    public:
        class Builder {
        public:
            Builder(Device& device) : device{ device } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32 count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32 count);
            std::unique_ptr<DescriptorPool> build() const;

        private:
            Device& device;
            Array<VkDescriptorPoolSize> poolSizes{};
            uint32 maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        DescriptorPool(
            Device& device,
            uint32 maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const Array<VkDescriptorPoolSize>& poolSizes);
        ~DescriptorPool();
        DescriptorPool(const DescriptorPool&) = delete;
        DescriptorPool& operator=(const DescriptorPool&) = delete;

        VkDescriptorPool GetPool() { return descriptorPool; }

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(Array<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        Device& device;
        VkDescriptorPool descriptorPool;

        friend class DescriptorWriter;
    };

    class DescriptorWriter {
    public:
        DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool);

        DescriptorWriter& writeBuffer(uint32 binding, VkDescriptorBufferInfo* bufferInfo);
        DescriptorWriter& writeImage(uint32 binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        DescriptorSetLayout& setLayout;
        DescriptorPool& pool;
        Array<VkWriteDescriptorSet> writes;
    };
}