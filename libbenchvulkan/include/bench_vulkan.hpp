#pragma once

#ifndef BENCH_VULKAN_HPP
#define BENCH_VULKAN_HPP

#include <bench_template.hpp>

// #define VULKAN_HPP_NO_EXCEPTIONS
// #define VULKAN_HPP_NO_SMART_HANDLE

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <array>

#define USE_VALIDATION_LAYERS false

class BenchVulkan : public BenchTemplate {
 private:
  struct Triangle {
    vk::Buffer       buffer;
    vk::DeviceMemory memory;
  };

 public:
  BenchVulkan(int numberOfThreads, int N, int M);
  virtual ~BenchVulkan();

  virtual void initialize(ResultCollection& resultCollection) final override;

  virtual void createTrianglesHost(
      ResultCollection& resultCollection) final override;
  virtual void createTrianglesSlow(
      ResultCollection& resultCollection) final override;
  virtual void createTrianglesSmart(
      ResultCollection& resultCollection) final override;
  virtual void createTrianglesFast(
      ResultCollection& resultCollection) final override;
  virtual void intermediateTriangleCleanUp() final override;

  virtual void createShaderModules(
      ResultCollection& resultCollection) final override;
  virtual void createPipelines(
      ResultCollection& resultCollection) final override;

  virtual void singleTriangleDraw(ResultCollection& resultCollection,
                                  bool              device) final override;
  virtual void optimalMultipleTriangleDraw(ResultCollection& resultCollection,
                                           bool device) final override;
  virtual void badMultipleTriangleDraw(ResultCollection& resultCollection,
                                       bool              device) final override;

  virtual void clean_up(ResultCollection& resultCollection) final override;

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
  void makePipelineLayout();
  void makeSemaphores();

  void thread_createTrianglesHost(int startIndex,
                                  int endIndex,
                                  int threadIndex);
  void thread_createTrianglesSlow(int startIndex,
                                  int endIndex,
                                  int threadIndex);
  void thread_createTrianglesSmart(int startIndex,
                                   int endIndex,
                                   int threadIndex);
  void thread_createTrianglesFast(int startIndex,
                                  int endIndex,
                                  int threadIndex);

  void thread_createShaderModules(
      int                                        startIndex,
      int                                        endIndex,
      int                                        threadIndex,
      const std::pair<std::string, std::string>& sourcePair);

  void thread_createPipelines(int startIndex, int endIndex, int threadIndex);

  void thread_singleTriangleDraw(int                                startIndex,
                                 int                                endIndex,
                                 int                                threadIndex,
                                 std::array<Triangle, BENCHMARK_N>* triangles);
  void thread_optimalMultiTriangleDraw(
      int                                startIndex,
      int                                endIndex,
      int                                threadIndex,
      std::array<Triangle, BENCHMARK_N>* triangles);
  void thread_badMultipleTriangleDraw(
      int                                startIndex,
      int                                endIndex,
      int                                threadIndex,
      std::array<Triangle, BENCHMARK_N>* triangles);

  void drawSubmission();

  uint32_t findMemoryType(uint32_t                typeFilter,
                          vk::MemoryPropertyFlags properties);

  std::pair<vk::Buffer, vk::DeviceMemory> getNewBuffer(
      vk::DeviceSize          size,
      vk::BufferUsageFlags    usage,
      vk::MemoryPropertyFlags properties);

  vk::CommandBuffer getTransferCommandBuffer(int threadIndex);
  vk::CommandBuffer getGraphicsCommandBuffer(int threadIndex);

  void copyBufferOnce(vk::Buffer     srcBuffer,
                      vk::Buffer     dstBuffer,
                      vk::DeviceSize size);

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
    vk::PhysicalDevice                     physicalDevice;
    vk::Device                             device;
    std::array<int, QueueType::NUM_QUEUES> queueIndices;

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

  struct ThreadContext {
    // std::array<vk::CommandPool, QueueType::NUM_QUEUES> commandPool;
    vk::CommandPool                commandPool;
    std::vector<vk::CommandBuffer> commandBuffers;
    vk::CommandBuffer*             currentCommandBuffer;
    vk::Queue                      queue;
  };
  std::vector<ThreadContext> _threadContexts;

  struct RenderContext {
    vk::RenderPass renderPass;
    vk::Semaphore  imageAvailableSemaphore;
    vk::Semaphore  renderFinishedSemaphore;
    uint32_t       currentSwapChainImageIndex;
  } _renderContext;

  // std::unordered_map<int, vk::DescriptorSetLayout> _descriptorSetLayouts;
  // std::unordered_map<int, int> _descriptorLocationToIndexMap;

  // vk::Queue          _presentQueue;
  vk::PipelineLayout _pipelineLayout;
  vk::DescriptorPool _descriptorPool;

  std::array<Triangle, BENCHMARK_N> _trianglesHost;
  std::array<Triangle, BENCHMARK_N> _trianglesDevice;

  std::array<std::pair<vk::ShaderModule, vk::ShaderModule>, BENCHMARK_N>
      _shaderModules;

  std::array<vk::Pipeline, BENCHMARK_N> _pipelines;

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