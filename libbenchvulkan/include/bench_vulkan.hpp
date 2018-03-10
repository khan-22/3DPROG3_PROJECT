#pragma once

#ifndef BENCH_VULKAN_HPP
#define BENCH_VULKAN_HPP

#include <bench_template.hpp>

// #define VULKAN_HPP_NO_EXCEPTIONS
// #define VULKAN_HPP_NO_SMART_HANDLE

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <array>

#define USE_VALIDATION_LAYERS true

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
  void makeInstance();
  void makeDebugCallback();
  void makeSurface();
  void makePhysicalDevice();
  void makeDevice();
  void makeSwapchain();
  void makeRenderPass();
  void makeFramebuffers();
  void makeCommandPool();
  void makeCommandBuffers();

  static VKAPI_ATTR VkBool32 VKAPI_CALL
                             debugCallback(VkDebugReportFlagsEXT      flags,
                                           VkDebugReportObjectTypeEXT objType,
                                           uint64_t                   obj,
                                           size_t                     location,
                                           int32_t                    code,
                                           const char*                layerPrefix,
                                           const char*                msg,
                                           void*                      userData);

 private:  // Variables
  enum QueueType { GRAPHICS, TRANSFER, PRESENT, NUM_QUEUES };

  GLFWwindow* _window = nullptr;

  vk::Instance               _instance;
  vk::DebugReportCallbackEXT _callback;

  struct DeviceContext {
    vk::PhysicalDevice                           physicalDevice;
    vk::Device                                   device;
    std::array<vk::Queue, QueueType::NUM_QUEUES> queues;
    std::array<int, QueueType::NUM_QUEUES>       queueIndices;

    vk::SurfaceKHR                    surface;
    vk::SurfaceCapabilitiesKHR        surfaceCapabilities;
    std::vector<vk::SurfaceFormatKHR> surfaceFormats;
    std::vector<vk::PresentModeKHR>   surfacePresentModes;
  } _deviceContext;

  struct SwapchainContext {
    vk::SwapchainKHR             swapchain;
    std::vector<vk::Image>       images;
    std::vector<vk::ImageView>   imageViews;
    std::vector<vk::Framebuffer> framebuffers;
    vk::Format                   imageFormat;
    vk::Extent2D                 extent;
  } _swapchainContext;

  struct RenderContext {
    vk::RenderPass                 renderPass;
    vk::CommandPool                commandPool;
    std::vector<vk::CommandBuffer> commandBuffers;
    vk::CommandBuffer*             currentCommandBuffer;
    vk::Semaphore                  imageAvailableSemaphore;
    vk::Semaphore                  renderFinishedSemaphore;
    uint32_t                       currentSwapChainImageIndex;
  } _renderContext;

  // std::unordered_map<int, vk::DescriptorSetLayout> _descriptorSetLayouts;
  // std::unordered_map<int, int> _descriptorLocationToIndexMap;

  vk::PipelineLayout _pipelineLayout;
  vk::DescriptorPool _descriptorPool;

  std::vector<const char*> _instanceExtensions = {
#if USE_VALIDATION_LAYERS
    VK_EXT_DEBUG_REPORT_EXTENSION_NAME  // Remove when not in debug later
#endif
  };

  std::vector<const char*> _deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };

  std::vector<const char*> _validationLayers = {
#if USE_VALIDATION_LAYERS
    "VK_LAYER_LUNARG_standard_validation"
#endif
  };
};

//
//
//

#define GET_PROC(name)                                                        \
  [&]() {                                                                     \
    auto ______pf = (PFN_##name)_instance.getProcAddr((#name));               \
    if (______pf == nullptr) {                                                \
      fprintf(stderr, "%s (%s)\n", "ERROR: Failed to load procedure", #name); \
      std::exit(EXIT_FAILURE);                                                \
    }                                                                         \
    return ______pf;                                                          \
  }()

#if 1
#define CRITICAL(call, msg)                                        \
  {                                                                \
    if (static_cast<vk::Result>((call)) != vk::Result::eSuccess) { \
      fprintf(stderr, "%s %s\n", "ERROR at ", msg);                \
      std::exit(EXIT_FAILURE);                                     \
    }                                                              \
  }
#else
#define CRITICAL(call, msg) \
  { (call) }
#endif

//
//
//

#endif  // BENCH_VULKAN_HPP