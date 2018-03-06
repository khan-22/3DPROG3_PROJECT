#pragma once

#ifndef BENCH_VULKAN_HPP
#define BENCH_VULKAN_HPP

#include <bench_template.hpp>

#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>

class BenchVulkan : public BenchTemplate {
 public:
  BenchVulkan(int numberOfThreads, int N, int M);
  virtual ~BenchVulkan();

  virtual void initialize() final override;

  virtual void createShaderModules() final override;
  virtual void createPipelines() final override;

  virtual void firstDraw() final override;
  virtual void secondDraw() final override;
  virtual void thirdDraw() final override;

  virtual void clean_up() final override;

 private:  // Functions
 private:  // Variables
  GLFWwindow* _window = nullptr;

  vk::Instance _instance;

  vk::DebugReportCallbackEXT _callback;

  vk::PhysicalDevice _physicalDevice;
  vk::Device         _device;

  // std::array<vk::Queue, QueueType::NUM_QUEUES>  _queues;
  // QueueFamilyIndices                            _queueFamilies;

  vk::SurfaceKHR               _surface;
  vk::SwapchainKHR             _swapChain;
  std::vector<vk::Image>       _swapChainImages;
  std::vector<vk::ImageView>   _swapChainImageViews;
  std::vector<vk::Framebuffer> _swapChainFramebuffers;
  vk::Format                   _swapChainImageFormat;
  vk::Extent2D                 _swapChainExtent;

  vk::RenderPass                 _renderPass;
  vk::CommandPool                _commandPool;
  std::vector<vk::CommandBuffer> _commandBuffers;
  vk::CommandBuffer*             _currentCommandBuffer;
  vk::Semaphore                  _imageAvailableSemaphore;
  vk::Semaphore                  _renderFinishedSemaphore;
  uint32_t                       _currentSwapChainImageIndex;

  // std::unordered_map<int, vk::DescriptorSetLayout> _descriptorSetLayouts;
  // std::unordered_map<int, int> _descriptorLocationToIndexMap;

  vk::PipelineLayout _pipelineLayout;
  vk::DescriptorPool _descriptorPool;
};

#endif  // BENCH_VULKAN_HPP