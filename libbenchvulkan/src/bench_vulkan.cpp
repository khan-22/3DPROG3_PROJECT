#include "bench_vulkan.hpp"

#include <iostream>

#include <shaderc/shaderc.hpp>

//
BenchVulkan::BenchVulkan(int numberOfThreads, int N, int M)
    : BenchTemplate(numberOfThreads, N, M) {
  //
  std::cout << "Started Vulkan Benchmark... (" << numberOfThreads << ", " << N
            << ", " << M << ")" << std::endl;
}

BenchVulkan::~BenchVulkan() {
  std::cout << "Finished Vulkan Benchmark" << std::endl;
}

void BenchVulkan::initialize(ResultCollection& resultCollection) {
  srand(0);

  if (!glfwInit()) {
    std::exit(EXIT_FAILURE);
  }

  Timer t;
  t.start("Window");
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  _window = glfwCreateWindow(_WIDTH, _HEIGHT, _TITLE.c_str(), nullptr, nullptr);
  glfwMakeContextCurrent(_window);
  t.stop();
  resultCollection.addResult(t);

  t.start("Instance");
  makeInstance();
  t.stop();
  resultCollection.addResult(t);

  t.start("DebugCallback");
  makeDebugCallback();
  t.stop();
  resultCollection.addResult(t);

  t.start("Surface");
  makeSurface();
  t.stop();
  resultCollection.addResult(t);

  t.start("PhysicalDevice");
  makePhysicalDevice();
  t.stop();
  resultCollection.addResult(t);

  t.start("Device");
  makeDevice();
  t.stop();
  resultCollection.addResult(t);

  t.start("Swapchain");
  makeSwapchain();
  t.stop();
  resultCollection.addResult(t);

  t.start("RenderPass");
  makeRenderPass();
  t.stop();
  resultCollection.addResult(t);

  t.start("Framebuffers");
  makeFramebuffers();
  t.stop();
  resultCollection.addResult(t);

  t.start("CommandPool");
  makeCommandPool();
  t.stop();
  resultCollection.addResult(t);

  t.start("CommandBuffers");
  makeCommandBuffers();
  t.stop();
  resultCollection.addResult(t);
}

void BenchVulkan::createShaderModules(ResultCollection& resultCollection) {
  Timer t;
  t.start("Loading");
  auto sourcePair = loadShaderSource();
  t.stop();
  resultCollection.addResult(t);

  shaderc::Compiler       compiler;
  shaderc::CompileOptions options;

  for (auto& shaderPair : _shaderModules) {
    auto sourcePairCopy = sourcePair;

    auto header          = getNextDefine();
    sourcePairCopy.first = header + sourcePair.first;

    auto vPreResult =
        compiler.PreprocessGlsl(sourcePairCopy.first,
                                shaderc_shader_kind::shaderc_vertex_shader,
                                "vertex",
                                options);

    if (vPreResult.GetCompilationStatus() !=
        shaderc_compilation_status_success) {
      std::cerr << "ERROR PREPROCESSING SHADER\n" << vPreResult.GetErrorMessage() << std::endl;
      std::exit(EXIT_FAILURE);
    }

    // std::cout << std::string{vPreResult.cbegin(), vPreResult.cend()}
    //           << std::endl;

    auto fPreResult =
        compiler.PreprocessGlsl(sourcePairCopy.second,
                                shaderc_shader_kind::shaderc_fragment_shader,
                                "fragment",
                                options);

    if (fPreResult.GetCompilationStatus() !=
        shaderc_compilation_status_success) {
      std::cerr << "ERROR PREPROCESSING SHADER\n" << fPreResult.GetErrorMessage() << std::endl;
      std::exit(EXIT_FAILURE);
    }

    auto vSpirvResult =
        compiler.CompileGlslToSpv({vPreResult.cbegin(), vPreResult.cend()},
                                  shaderc_shader_kind::shaderc_vertex_shader,
                                  "vertex",
                                  options);

    if (vSpirvResult.GetCompilationStatus() !=
        shaderc_compilation_status_success) {
			std::cerr << "ERROR COMPILING SHADER\n" << vSpirvResult.GetErrorMessage() << std::endl;
      std::exit(EXIT_FAILURE);
    }

    auto fSpirvResult =
        compiler.CompileGlslToSpv({fPreResult.cbegin(), fPreResult.cend()},
                                  shaderc_shader_kind::shaderc_fragment_shader,
                                  "fragment",
                                  options);

    if (fSpirvResult.GetCompilationStatus() !=
        shaderc_compilation_status_success) {
      std::cerr << "ERROR COMPILING SHADER\n" << fSpirvResult.GetErrorMessage() << std::endl;
      std::exit(EXIT_FAILURE);
    }

    std::pair<std::vector<uint32_t>, std::vector<uint32_t>> spirvPair = {
        {vSpirvResult.cbegin(), vSpirvResult.cend()},
        {fSpirvResult.cbegin(), fSpirvResult.cend()}};

    vk::ShaderModuleCreateInfo vShaderInfo;
    vShaderInfo.codeSize = spirvPair.first.size() * sizeof(spirvPair.first[0]);
    vShaderInfo.pCode =
        reinterpret_cast<const uint32_t*>(spirvPair.first.data());

    vk::ShaderModuleCreateInfo fShaderInfo;
    fShaderInfo.codeSize =
        spirvPair.second.size() * sizeof(spirvPair.second[0]);
    fShaderInfo.pCode =
        reinterpret_cast<const uint32_t*>(spirvPair.second.data());

    CRITICAL(_deviceContext.device.createShaderModule(
                 &vShaderInfo, nullptr, &shaderPair.first),
             "createShaderModule");
    CRITICAL(_deviceContext.device.createShaderModule(
                 &fShaderInfo, nullptr, &shaderPair.second),
             "createShaderModule");
  }
}

void BenchVulkan::createPipelines(ResultCollection& resultCollection) {
}

void BenchVulkan::firstDraw(ResultCollection& resultCollection) {
}

void BenchVulkan::secondDraw(ResultCollection& resultCollection) {
}

void BenchVulkan::thirdDraw(ResultCollection& resultCollection) {
}

void BenchVulkan::clean_up(ResultCollection& resultCollection) {
  for (auto& shaderModule : _shaderModules) {
    _deviceContext.device.destroyShaderModule(shaderModule.first);
    _deviceContext.device.destroyShaderModule(shaderModule.second);
  }

  _deviceContext.device.freeCommandBuffers(_renderContext.commandPool,
                                           _renderContext.commandBuffers);

  _deviceContext.device.destroyCommandPool(_renderContext.commandPool);

  for (auto& framebuffer : _swapchainContext.framebuffers) {
    _deviceContext.device.destroyFramebuffer(framebuffer);
  }

  _deviceContext.device.destroyRenderPass(_renderContext.renderPass);

  for (auto& imageView : _swapchainContext.imageViews) {
    _deviceContext.device.destroyImageView(imageView);
  }
  _swapchainContext.imageViews.clear();

  _deviceContext.device.destroySwapchainKHR(_swapchainContext.swapchain);

  _deviceContext.device.destroy();

  _instance.destroySurfaceKHR(_deviceContext.surface);

  auto destroyDebugCallback = GET_PROC(vkDestroyDebugReportCallbackEXT);
  destroyDebugCallback(
      _instance, static_cast<VkDebugReportCallbackEXT>(_callback), nullptr);

  _instance.destroy();

  glfwDestroyWindow(_window);
  glfwTerminate();  //
}

// // // // // // // // // // // // // // // // // // //
// // // // // // // // // // // // // // // // // // //
// // // // // // // // // // // // // // // // // // //

//
//
//
void BenchVulkan::makeInstance() {
  vk::ApplicationInfo appInfo;
  appInfo.pNext            = nullptr;
  appInfo.pApplicationName = "benchmark";
  appInfo.pEngineName      = nullptr;
  appInfo.engineVersion    = 1;
  appInfo.apiVersion       = VK_API_VERSION_1_0;

  uint32_t     glfwExtensionCount = 0;
  const char** glfwExtensions =
      glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  _instanceExtensions.insert(_instanceExtensions.end(),
                             glfwExtensions,
                             glfwExtensions + glfwExtensionCount);

  vk::InstanceCreateInfo instanceInfo;
  instanceInfo.pNext                   = nullptr;
  instanceInfo.pApplicationInfo        = &appInfo;
  instanceInfo.ppEnabledLayerNames     = _validationLayers.data();
  instanceInfo.ppEnabledExtensionNames = _instanceExtensions.data();
  instanceInfo.enabledLayerCount =
      static_cast<uint32_t>(_validationLayers.size());
  instanceInfo.enabledExtensionCount =
      static_cast<uint32_t>(_instanceExtensions.size());

  CRITICAL(vk::createInstance(&instanceInfo, nullptr, &_instance),
           "createInstance");
}

//
//
//
void BenchVulkan::makeDebugCallback() {
  vk::DebugReportCallbackCreateInfoEXT debugInfo;
  debugInfo.flags = vk::DebugReportFlagBitsEXT::eError |  //
                    vk::DebugReportFlagBitsEXT::eWarning;
  debugInfo.pfnCallback = debugCallback;

  auto createDebugReportCallbackEXT = GET_PROC(vkCreateDebugReportCallbackEXT);
  CRITICAL(
      createDebugReportCallbackEXT(
          _instance,
          reinterpret_cast<VkDebugReportCallbackCreateInfoEXT*>(&debugInfo),
          nullptr,
          reinterpret_cast<VkDebugReportCallbackEXT*>(&_callback)),
      "createDebugReportCallback");
}

//
//
//
void BenchVulkan::makeSurface() {
  CRITICAL(glfwCreateWindowSurface(
               _instance,
               _window,
               nullptr,
               reinterpret_cast<VkSurfaceKHR*>(&_deviceContext.surface)),
           "glfwCreateWindowSurface");
}

//
//
//
void BenchVulkan::makePhysicalDevice() {
  int graphicsQueueIndex = -1;
  int presentQueueIndex  = -1;
  int transferQueueIndex = -1;

  vk::PhysicalDevice availableDevice =
      _instance.enumeratePhysicalDevices().value[0];

  // for (auto& availableDevice : _instance.enumeratePhysicalDevices().value) {
  // std::cout << availableDevice.getProperties().deviceName << std::endl;

  vk::PhysicalDeviceProperties properties = availableDevice.getProperties();

  auto queueFamilies = availableDevice.getQueueFamilyProperties();
  for (int i = 0; i < queueFamilies.size(); i++) {
    if (graphicsQueueIndex == -1 &&
        queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
      graphicsQueueIndex = i;
      if (availableDevice.getSurfaceSupportKHR(i, _deviceContext.surface)
              .value) {
        presentQueueIndex = i;
      }
    }

    if ((transferQueueIndex == -1 ||
         transferQueueIndex == graphicsQueueIndex) &&
        queueFamilies[i].queueFlags & vk::QueueFlagBits::eTransfer) {
      transferQueueIndex = i;
    }
  }

  if (graphicsQueueIndex == -1 || presentQueueIndex == -1 ||
      transferQueueIndex == -1) {
    std::cerr << "Failed to find suitable device" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  _deviceContext.physicalDevice         = availableDevice;
  _deviceContext.queueIndices[GRAPHICS] = graphicsQueueIndex;
  _deviceContext.queueIndices[PRESENT]  = presentQueueIndex;
  _deviceContext.queueIndices[TRANSFER] = transferQueueIndex;

  _deviceContext.surfaceCapabilities =
      availableDevice.getSurfaceCapabilitiesKHR(_deviceContext.surface).value;
  _deviceContext.surfaceFormats =
      availableDevice.getSurfaceFormatsKHR(_deviceContext.surface).value;
  _deviceContext.surfacePresentModes =
      availableDevice.getSurfacePresentModesKHR(_deviceContext.surface).value;
  // }
}

//
//
//
void BenchVulkan::makeDevice() {
  float queuePriorities[QueueType::NUM_QUEUES] = {1.0f};

  struct QueueConstruct {
    std::vector<QueueType> types;
    int                    family = 0;
    int                    count  = 0;
  };
  std::vector<QueueConstruct>            queueConstructs = {};
  std::vector<vk::DeviceQueueCreateInfo> queueInfos      = {};

  for (int i = 0; i < QueueType::NUM_QUEUES; i++) {
    bool already_exists = false;

    for (auto& queueConstruct : queueConstructs) {
      if (queueConstruct.family == _deviceContext.queueIndices[i]) {
        already_exists = true;
        queueConstruct.count++;
        queueConstruct.types.push_back(static_cast<QueueType>(i));
      }
    }
    if (!already_exists) {
      queueConstructs.push_back(
          {{static_cast<QueueType>(i)}, _deviceContext.queueIndices[i], 1});
    }
  }

  for (auto& queueConstruct : queueConstructs) {
    vk::DeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.queueFamilyIndex          = queueConstruct.family;
    queueCreateInfo.queueCount                = queueConstruct.count;
    queueCreateInfo.pQueuePriorities          = queuePriorities;

    queueInfos.push_back(queueCreateInfo);
  }

  vk::PhysicalDeviceFeatures deviceFeatures;
  // deviceFeatures.fillModeNonSolid = true;

  vk::DeviceCreateInfo deviceInfo = {};
  deviceInfo.pQueueCreateInfos    = queueInfos.data();
  deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
  deviceInfo.pEnabledFeatures     = &deviceFeatures;

  deviceInfo.enabledLayerCount =
      static_cast<uint32_t>(_validationLayers.size());
  deviceInfo.ppEnabledLayerNames = _validationLayers.data();

  deviceInfo.enabledExtensionCount =
      static_cast<uint32_t>(_deviceExtensions.size());
  deviceInfo.ppEnabledExtensionNames = _deviceExtensions.data();

  CRITICAL(_deviceContext.physicalDevice.createDevice(
               &deviceInfo, nullptr, &_deviceContext.device),
           "createDevice");

  for (auto& queueConstruct : queueConstructs) {
    for (int i = 0; i < queueConstruct.types.size(); i++) {
      _deviceContext.queues[queueConstruct.types[i]] =
          _deviceContext.device.getQueue(queueConstruct.family, i);
    }
  }
}

//
//
//
void BenchVulkan::makeSwapchain() {
  auto chooseSwapSurfaceFormat = [&]() -> vk::SurfaceFormatKHR {
    const std::vector<vk::SurfaceFormatKHR>& availableFormats =
        _deviceContext.surfaceFormats;

    vk::SurfaceFormatKHR preferredFormat = {vk::Format::eB8G8R8A8Unorm,
                                            vk::ColorSpaceKHR::eSrgbNonlinear};

    // Special case where the GPU has no preferred format, so we are free to
    // choose what we want
    if (availableFormats.size() == 1 &&
        availableFormats[0].format == vk::Format::eUndefined) {
      return preferredFormat;
    }

    // Otherwise we must iterate to check for preferred support
    for (const auto& availableFormat : availableFormats) {
      if (availableFormat.format == preferredFormat.format &&
          availableFormat.colorSpace == preferredFormat.colorSpace) {
        return availableFormat;
      }
    }

    // If all else fails we'll just pick the first one available
    return availableFormats[0];
  };

  auto chooseSwapPresentMode = [&]() -> vk::PresentModeKHR {
    const std::vector<vk::PresentModeKHR>& availablePresentModes =
        _deviceContext.surfacePresentModes;

    // Guaranteed to exist
    vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

    for (const auto& availablePresentMode : availablePresentModes) {
      // We would prefer mailbox if it is available
      if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
        return availablePresentMode;
      } else if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
        bestMode = availablePresentMode;
      }
    }

    return bestMode;
  };

  auto chooseSwapExtent = [&]() {
    const vk::SurfaceCapabilitiesKHR& capabilities =
        _deviceContext.surfaceCapabilities;

    if (capabilities.currentExtent.width !=
        (std::numeric_limits<uint32_t>::max)()) {
      return capabilities.currentExtent;
    } else {
      vk::Extent2D actualExtent = {_WIDTH, _HEIGHT};

      actualExtent.width = (std::max)(
          capabilities.minImageExtent.width,
          (std::min)(capabilities.maxImageExtent.width, actualExtent.width));
      actualExtent.height = (std::max)(
          capabilities.minImageExtent.height,
          (std::min)(capabilities.maxImageExtent.height, actualExtent.height));

      return actualExtent;
    }
  };

  vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat();
  vk::PresentModeKHR   presentMode   = chooseSwapPresentMode();
  vk::Extent2D         extent        = chooseSwapExtent();

  uint32_t imageCount = _deviceContext.surfaceCapabilities.minImageCount + 1;
  if (_deviceContext.surfaceCapabilities.maxImageCount > 0 &&
      imageCount > _deviceContext.surfaceCapabilities.maxImageCount) {
    imageCount = _deviceContext.surfaceCapabilities.maxImageCount;
  }

  vk::SwapchainCreateInfoKHR swapchainInfo = {};
  swapchainInfo.surface                    = _deviceContext.surface;
  swapchainInfo.minImageCount              = imageCount;
  swapchainInfo.imageFormat                = surfaceFormat.format;
  swapchainInfo.imageColorSpace            = surfaceFormat.colorSpace;
  swapchainInfo.imageExtent                = extent;
  swapchainInfo.imageArrayLayers           = 1;
  swapchainInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

  uint32_t queueFamilyIndices[] = {
      static_cast<uint32_t>(_deviceContext.queueIndices[GRAPHICS]),
      static_cast<uint32_t>(_deviceContext.queueIndices[PRESENT]),
  };

  // If the families are separate we can perform actions concurrently.
  // Otherwise, we can't.
  if (_deviceContext.queueIndices[GRAPHICS] !=
      _deviceContext.queueIndices[PRESENT]) {
    swapchainInfo.imageSharingMode      = vk::SharingMode::eConcurrent;
    swapchainInfo.queueFamilyIndexCount = 2;
    swapchainInfo.pQueueFamilyIndices   = queueFamilyIndices;
  } else {
    swapchainInfo.imageSharingMode      = vk::SharingMode::eExclusive;
    swapchainInfo.queueFamilyIndexCount = 0;
    swapchainInfo.pQueueFamilyIndices   = nullptr;
  }

  swapchainInfo.preTransform =
      _deviceContext.surfaceCapabilities.currentTransform;
  swapchainInfo.compositeAlpha =
      vk::CompositeAlphaFlagBitsKHR::eOpaque;  // How the surface should blend
                                               // with other windows in the OS.
  swapchainInfo.presentMode = presentMode;
  swapchainInfo.clipped     = static_cast<vk::Bool32>(true);

  // Swapchain creation
  CRITICAL(_deviceContext.device.createSwapchainKHR(
               &swapchainInfo, nullptr, &_swapchainContext.swapchain),
           "createSwapchainKHR");

  // uint32_t imageCount = 0;
  // _deviceContext.device.getSwapchainImagesKHR(
  // _swapchainContext.swapchain, &imageCount, nullptr);
  // _swapchainContext.images.resize(imageCount);
  _swapchainContext.images =
      _deviceContext.device.getSwapchainImagesKHR(_swapchainContext.swapchain)
          .value;

  _swapchainContext.imageFormat = surfaceFormat.format;
  _swapchainContext.extent      = extent;

  // Create image views
  for (auto& swapchainImage : _swapchainContext.images) {
    vk::ImageViewCreateInfo imageViewInfo     = {};
    imageViewInfo.image                       = swapchainImage;
    imageViewInfo.viewType                    = vk::ImageViewType::e2D;
    imageViewInfo.format                      = _swapchainContext.imageFormat;
    imageViewInfo.components.r                = vk::ComponentSwizzle::eIdentity;
    imageViewInfo.components.g                = vk::ComponentSwizzle::eIdentity;
    imageViewInfo.components.b                = vk::ComponentSwizzle::eIdentity;
    imageViewInfo.components.a                = vk::ComponentSwizzle::eIdentity;
    imageViewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    imageViewInfo.subresourceRange.baseMipLevel   = 0;
    imageViewInfo.subresourceRange.levelCount     = 1;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount     = 1;

    vk::ImageView imageView;
    CRITICAL(_deviceContext.device.createImageView(
                 &imageViewInfo, nullptr, &imageView),
             "createImageView");

    _swapchainContext.imageViews.push_back(imageView);
  }
}

void BenchVulkan::makeRenderPass() {
  vk::AttachmentDescription colorAttachment = {};
  colorAttachment.format                    = _swapchainContext.imageFormat;
  colorAttachment.samples                   = vk::SampleCountFlagBits::e1;
  colorAttachment.loadOp                    = vk::AttachmentLoadOp::eClear;
  colorAttachment.storeOp                   = vk::AttachmentStoreOp::eStore;
  colorAttachment.stencilLoadOp             = vk::AttachmentLoadOp::eDontCare;
  colorAttachment.stencilStoreOp            = vk::AttachmentStoreOp::eDontCare;
  colorAttachment.initialLayout             = vk::ImageLayout::eUndefined;
  colorAttachment.finalLayout               = vk::ImageLayout::ePresentSrcKHR;

  vk::AttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment              = 0;
  colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

  // Do we need a subpass? Tutorial doesn't say if it is mandatory to have at
  // least one or not.
  vk::SubpassDescription subpass = {};
  subpass.pipelineBindPoint      = vk::PipelineBindPoint::eGraphics;
  subpass.colorAttachmentCount   = 1;
  subpass.pColorAttachments      = &colorAttachmentRef;

  vk::SubpassDependency dependency;
  dependency.srcSubpass =
      VK_SUBPASS_EXTERNAL;       // Refers to the implicit subpass before
                                 // the render pass.
  dependency.dstSubpass    = 0;  // Refers to our subpass
  dependency.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
  dependency.srcAccessMask = vk::AccessFlags();  // None specified
  dependency.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
  dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
                             vk::AccessFlagBits::eColorAttachmentWrite;

  // Finally create the renderpass
  vk::RenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.attachmentCount          = 1;
  renderPassInfo.pAttachments             = &colorAttachment;
  renderPassInfo.subpassCount             = 1;
  renderPassInfo.pSubpasses               = &subpass;
  renderPassInfo.dependencyCount          = 1;
  renderPassInfo.pDependencies            = &dependency;

  CRITICAL(_deviceContext.device.createRenderPass(
               &renderPassInfo, nullptr, &_renderContext.renderPass),
           "createRenderPass");
}

void BenchVulkan::makeFramebuffers() {
  // Create framebuffers
  _swapchainContext.framebuffers.resize(_swapchainContext.imageViews.size());

  for (size_t i = 0; i < _swapchainContext.imageViews.size(); i++) {
    vk::ImageView attachments[] = {_swapchainContext.imageViews[i]};

    vk::FramebufferCreateInfo framebufferInfo;
    framebufferInfo.renderPass      = _renderContext.renderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments    = attachments;
    framebufferInfo.width           = _swapchainContext.extent.width;
    framebufferInfo.height          = _swapchainContext.extent.height;
    framebufferInfo.layers          = 1;

    CRITICAL(_deviceContext.device.createFramebuffer(
                 &framebufferInfo, nullptr, &_swapchainContext.framebuffers[i]),
             "createFramebuffer");
  }
}

void BenchVulkan::makeCommandPool() {
  vk::CommandPoolCreateInfo poolInfo;
  poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
  poolInfo.queueFamilyIndex = _deviceContext.queueIndices[GRAPHICS];

  CRITICAL(_deviceContext.device.createCommandPool(
               &poolInfo, nullptr, &_renderContext.commandPool),
           "createCommandPool");
}

void BenchVulkan::makeCommandBuffers() {
  vk::CommandBufferAllocateInfo allocInfo;
  allocInfo.commandPool = _renderContext.commandPool;
  allocInfo.level       = vk::CommandBufferLevel::ePrimary;
  allocInfo.commandBufferCount =
      static_cast<uint32_t>(_swapchainContext.framebuffers.size());

  _renderContext.commandBuffers.resize(_swapchainContext.framebuffers.size());

  _deviceContext.device.allocateCommandBuffers(
      &allocInfo, _renderContext.commandBuffers.data());
}

//
//
//

VKAPI_ATTR VkBool32 VKAPI_CALL
                    BenchVulkan::debugCallback(VkDebugReportFlagsEXT      flags,
                               VkDebugReportObjectTypeEXT objType,
                               uint64_t                   obj,
                               size_t                     location,
                               int32_t                    code,
                               const char*                layerPrefix,
                               const char*                msg,
                               void*                      userData) {
  switch (flags) {
    // std::cerr << "\033[31m";
    case VK_DEBUG_REPORT_ERROR_BIT_EXT:
      std::cerr << "Validation ERROR: " << msg << std::endl;
      break;
    case VK_DEBUG_REPORT_WARNING_BIT_EXT:
      std::cerr << "Validation WARNING: " << msg << std::endl;
      break;
    default: std::cerr << "Unknown Problem: " << msg << std::endl; break;
  }
  // std::cerr << "\033[0m";

  return VK_FALSE;
}
