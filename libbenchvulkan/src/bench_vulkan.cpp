#include "bench_vulkan.hpp"

#include <iostream>
#include <thread>

#include <shaderc/shaderc.hpp>

//
BenchVulkan::BenchVulkan(int numberOfThreads, int N, int M)
    : BenchTemplate(numberOfThreads, N, M) {
  //
  std::cout << "Started Vulkan Benchmark... (" << numberOfThreads << ", " << N
            << ", " << M << ")" << std::endl;

  _threadContexts.resize(numberOfThreads);
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
  if (!_window) {
    std::cerr << "Failed to create window" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(_window);
  t.stop();
  resultCollection.addResult(t);

  t.start("Instance");
  makeInstance();
  t.stop();
  resultCollection.addResult(t);

#if USE_VALIDATION_LAYERS
  // t.start("DebugCallback");
  makeDebugCallback();
  // t.stop();
  // resultCollection.addResult(t);
#endif  // USE_VALIDATION_LAYERS

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

  t.start("PipelineLayout");
  makePipelineLayout();
  t.stop();
  resultCollection.addResult(t);

  t.start("Semaphores");
  makeSemaphores();
  t.stop();
  resultCollection.addResult(t);
}

void BenchVulkan::thread_createTrianglesHost(int startIndex,
                                             int endIndex,
                                             int threadIndex) {
  const size_t size = 3 * sizeof(Vertex);

  for (int i = startIndex; i < endIndex; i++) {
    Triangle& triangle = _trianglesHost[i];

    auto bufferAndMemory =
        getNewBuffer(size,
                     vk::BufferUsageFlagBits::eVertexBuffer,
                     vk::MemoryPropertyFlagBits::eHostVisible |
                         vk::MemoryPropertyFlagBits::eHostCoherent);

    triangle.buffer = bufferAndMemory.first;
    triangle.memory = bufferAndMemory.second;

    void* mappedMemory;
    CRITICAL(
        _deviceContext.device.mapMemory(
            triangle.memory, 0, size, vk::MemoryMapFlagBits(), &mappedMemory),
        "mapMemory");
    memcpy(mappedMemory, getNextTriangle().data(), size);
    _deviceContext.device.unmapMemory(triangle.memory);
  }
}

void BenchVulkan::createTrianglesHost(ResultCollection& resultCollection) {
  const int TRI_PER_THREAD = _trianglesHost.size() / _numberOfThreads;
  int       startIndex     = 0;
  int       endIndex       = TRI_PER_THREAD;

  std::vector<std::thread> threads;
  threads.reserve(_numberOfThreads - 1);
  for (int i = 0; i < _numberOfThreads - 1; i++) {
    threads.emplace_back(&BenchVulkan::thread_createTrianglesHost,
                         this,
                         startIndex,
                         endIndex,
                         i);

    startIndex += TRI_PER_THREAD;
    endIndex += TRI_PER_THREAD;
  }

  thread_createTrianglesHost(
      startIndex, _trianglesHost.size(), _numberOfThreads);

  for (auto& thread : threads) {
    thread.join();
  }
}

void BenchVulkan::thread_createTrianglesSlow(int startIndex,
                                             int endIndex,
                                             int threadIndex) {
  const size_t size = 3 * sizeof(Vertex);

  for (int i = startIndex; i < endIndex; i++) {
    Triangle& triangle = _trianglesDevice[i];

    auto stagingBufferAndMemory =
        getNewBuffer(size,
                     vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlagBits::eHostVisible |
                         vk::MemoryPropertyFlagBits::eHostCoherent);

    void* mappedMemory;
    CRITICAL(_deviceContext.device.mapMemory(stagingBufferAndMemory.second,
                                             0,
                                             size,
                                             vk::MemoryMapFlagBits(0),
                                             &mappedMemory),
             "mapMemory");
    memcpy(mappedMemory, getNextTriangle().data(), size);
    _deviceContext.device.unmapMemory(stagingBufferAndMemory.second);

    auto bufferAndMemory =
        getNewBuffer(size,
                     vk::BufferUsageFlagBits::eVertexBuffer |
                         vk::BufferUsageFlagBits::eTransferDst,
                     vk::MemoryPropertyFlagBits::eDeviceLocal);

    triangle.buffer = bufferAndMemory.first;
    triangle.memory = bufferAndMemory.second;

    vk::CommandBuffer commandBuffer = getTransferCommandBuffer(threadIndex);

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    commandBuffer.begin(beginInfo);
    vk::BufferCopy copyRegion;
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size      = size;
    commandBuffer.copyBuffer(
        stagingBufferAndMemory.first, bufferAndMemory.first, copyRegion);
    commandBuffer.end();

    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    _threadContexts[threadIndex].queue.submit(submitInfo, nullptr);
    _threadContexts[threadIndex].queue.waitIdle();

    _deviceContext.device.freeCommandBuffers(
        _threadContexts[threadIndex].commandPool, commandBuffer);

    _deviceContext.device.destroyBuffer(stagingBufferAndMemory.first);
    _deviceContext.device.freeMemory(stagingBufferAndMemory.second);
  }
}

void BenchVulkan::createTrianglesSlow(ResultCollection& resultCollection) {
  const int TRI_PER_THREAD = _trianglesDevice.size() / _numberOfThreads;
  int       startIndex     = 0;
  int       endIndex       = TRI_PER_THREAD;

  std::vector<std::thread> threads;
  threads.reserve(_numberOfThreads - 1);
  for (int i = 0; i < _numberOfThreads - 1; i++) {
    threads.emplace_back(&BenchVulkan::thread_createTrianglesSlow,
                         this,
                         startIndex,
                         endIndex,
                         i);

    startIndex += TRI_PER_THREAD;
    endIndex += TRI_PER_THREAD;
  }

  thread_createTrianglesSlow(
      startIndex, _trianglesDevice.size(), _numberOfThreads - 1);

  for (auto& thread : threads) {
    thread.join();
  }
}

void BenchVulkan::thread_createTrianglesSmart(int    startIndex,
                                              int    endIndex,
                                              int    threadIndex,
                                              Timer* timer) {
  const int CHAIN_SIZE = 5;

  const size_t size = 3 * sizeof(Vertex);

  int                                                             current = 0;
  std::array<vk::CommandBuffer, CHAIN_SIZE>                       commandChain;
  std::array<vk::Fence, CHAIN_SIZE>                               fenceChain;
  std::array<std::pair<vk::Buffer, vk::DeviceMemory>, CHAIN_SIZE> stagingChain;
  for (auto& commandBuffer : commandChain) {
    commandBuffer = getTransferCommandBuffer(threadIndex);
  }

  for (auto& fence : fenceChain) {
    vk::FenceCreateInfo fenceInfo;

    CRITICAL(_deviceContext.device.createFence(&fenceInfo, nullptr, &fence),
             "createFence");

    _deviceContext.device.resetFences(fence);
  }

  for (auto& staging : stagingChain) {
    staging = getNewBuffer(size,
                           vk::BufferUsageFlagBits::eTransferSrc,
                           vk::MemoryPropertyFlagBits::eHostVisible |
                               vk::MemoryPropertyFlagBits::eHostCoherent);
  }

  bool hasRunOnce = false;
  for (int i = startIndex; i < endIndex; i++) {
    current %= commandChain.size();
    if (current == 0 && i != startIndex) {
      hasRunOnce = true;
    }

    vk::Fence&         currentFence         = fenceChain[current];
    vk::CommandBuffer& currentCommandBuffer = commandChain[current];
    vk::Buffer&        currentStagingBuffer = stagingChain[current].first;
    vk::DeviceMemory&  currentStagingMemory = stagingChain[current++].second;
    Triangle&          triangle             = _trianglesDevice[i];

    if (hasRunOnce) {
      _deviceContext.device.waitForFences(
          currentFence, true, std::numeric_limits<uint32_t>::max());

      _deviceContext.device.resetFences(currentFence);
    }

    void* mappedMemory;
    CRITICAL(_deviceContext.device.mapMemory(currentStagingMemory,
                                             0,
                                             size,
                                             vk::MemoryMapFlagBits(0),
                                             &mappedMemory),
             "mapMemory");
    memcpy(mappedMemory, getNextTriangle().data(), size);
    _deviceContext.device.unmapMemory(currentStagingMemory);

    auto bufferAndMemory =
        getNewBuffer(size,
                     vk::BufferUsageFlagBits::eVertexBuffer |
                         vk::BufferUsageFlagBits::eTransferDst,
                     vk::MemoryPropertyFlagBits::eDeviceLocal);

    triangle.buffer = bufferAndMemory.first;
    triangle.memory = bufferAndMemory.second;

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    currentCommandBuffer.begin(beginInfo);
    vk::BufferCopy copyRegion;
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size      = size;
    currentCommandBuffer.copyBuffer(
        currentStagingBuffer, bufferAndMemory.first, copyRegion);
    currentCommandBuffer.end();

    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &currentCommandBuffer;

    _threadContexts[threadIndex].queue.submit(submitInfo, currentFence);
  }

  if (timer) {
    timer->stop();
  }

  _threadContexts[threadIndex].queue.waitIdle();

  for (auto& commandBuffer : commandChain) {
    _deviceContext.device.freeCommandBuffers(
        _threadContexts[threadIndex].commandPool, commandBuffer);
  }

  for (auto& fence : fenceChain) {
    _deviceContext.device.destroyFence(fence);
  }

  for (auto& staging : stagingChain) {
    _deviceContext.device.destroyBuffer(staging.first);
    _deviceContext.device.freeMemory(staging.second);
  }
}

void BenchVulkan::createTrianglesSmart(ResultCollection& resultCollection) {
  const int TRI_PER_THREAD = _trianglesDevice.size() / _numberOfThreads;
  int       startIndex     = 0;
  int       endIndex       = TRI_PER_THREAD;

  std::vector<std::thread> threads;
  threads.reserve(_numberOfThreads - 1);
  for (int i = 0; i < _numberOfThreads - 1; i++) {
    threads.emplace_back(&BenchVulkan::thread_createTrianglesSmart,
                         this,
                         startIndex,
                         endIndex,
                         i,
                         nullptr);

    startIndex += TRI_PER_THREAD;
    endIndex += TRI_PER_THREAD;
  }

  Timer t;
  t.start("Work");
  thread_createTrianglesSmart(
      startIndex, _trianglesDevice.size(), _numberOfThreads - 1, &t);
  resultCollection.addResult(t);

  for (auto& thread : threads) {
    thread.join();
  }
}

void BenchVulkan::thread_createTrianglesFast(int    startIndex,
                                             int    endIndex,
                                             int    threadIndex,
                                             Timer* timer) {
  const size_t size = 3 * sizeof(Vertex);

  std::vector<std::pair<vk::Buffer, vk::DeviceMemory>> stagings;
  stagings.reserve(endIndex - startIndex);

  for (int i = startIndex; i < endIndex; i++) {
    Triangle& triangle = _trianglesDevice[i];

    auto stagingBufferAndMemory =
        getNewBuffer(size,
                     vk::BufferUsageFlagBits::eTransferSrc,
                     vk::MemoryPropertyFlagBits::eHostVisible |
                         vk::MemoryPropertyFlagBits::eHostCoherent);

    void* mappedMemory;
    CRITICAL(_deviceContext.device.mapMemory(stagingBufferAndMemory.second,
                                             0,
                                             size,
                                             vk::MemoryMapFlagBits(0),
                                             &mappedMemory),
             "mapMemory");
    memcpy(mappedMemory, getNextTriangle().data(), size);
    _deviceContext.device.unmapMemory(stagingBufferAndMemory.second);

    stagings.push_back(stagingBufferAndMemory);

    auto bufferAndMemory =
        getNewBuffer(size,
                     vk::BufferUsageFlagBits::eVertexBuffer |
                         vk::BufferUsageFlagBits::eTransferDst,
                     vk::MemoryPropertyFlagBits::eDeviceLocal);

    triangle.buffer = bufferAndMemory.first;
    triangle.memory = bufferAndMemory.second;
  }

  vk::CommandBuffer commandBuffer = getTransferCommandBuffer(threadIndex);

  vk::CommandBufferBeginInfo beginInfo;
  beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

  commandBuffer.begin(beginInfo);
  vk::BufferCopy copyRegion;
  copyRegion.srcOffset = 0;
  copyRegion.dstOffset = 0;
  copyRegion.size      = size;

  for (int i = startIndex; i < endIndex; i++) {
    commandBuffer.copyBuffer(
        stagings[i - startIndex].first, _trianglesDevice[i].buffer, copyRegion);
  }
  commandBuffer.end();

  vk::SubmitInfo submitInfo;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers    = &commandBuffer;

  _threadContexts[threadIndex].queue.submit(submitInfo, nullptr);

  if (timer) {
    timer->stop();
  }

  _threadContexts[threadIndex].queue.waitIdle();

  _deviceContext.device.freeCommandBuffers(
      _threadContexts[threadIndex].commandPool, commandBuffer);

  for (auto& staging : stagings) {
    _deviceContext.device.destroyBuffer(staging.first);
    _deviceContext.device.freeMemory(staging.second);
  }
}

void BenchVulkan::createTrianglesFast(ResultCollection& resultCollection) {
  const int TRI_PER_THREAD = _trianglesDevice.size() / _numberOfThreads;
  int       startIndex     = 0;
  int       endIndex       = TRI_PER_THREAD;

  std::vector<std::thread> threads;
  threads.reserve(_numberOfThreads - 1);
  for (int i = 0; i < _numberOfThreads - 1; i++) {
    threads.emplace_back(&BenchVulkan::thread_createTrianglesFast,
                         this,
                         startIndex,
                         endIndex,
                         i,
                         nullptr);

    startIndex += TRI_PER_THREAD;
    endIndex += TRI_PER_THREAD;
  }

  Timer t;
  t.start("Work");
  thread_createTrianglesFast(
      startIndex, _trianglesDevice.size(), _numberOfThreads - 1, &t);
  resultCollection.addResult(t);

  for (auto& thread : threads) {
    thread.join();
  }
}

void BenchVulkan::intermediateTriangleCleanUp() {
  if (_trianglesDevice[0].buffer != vk::Buffer()) {
    for (auto& triangle : _trianglesDevice) {
      _deviceContext.device.destroyBuffer(triangle.buffer);
      _deviceContext.device.freeMemory(triangle.memory);
      triangle.buffer = nullptr;
      triangle.memory = nullptr;
    }
  }
}

void BenchVulkan::thread_createShaderModules(
    int                                        startIndex,
    int                                        endIndex,
    int                                        threadIndex,
    const std::pair<std::string, std::string>& sourcePair) {
  shaderc::Compiler       compiler;
  shaderc::CompileOptions options;

  for (int i = startIndex; i < endIndex; i++) {
    auto& shaderPair     = _shaderModules[i];
    auto  sourcePairCopy = sourcePair;

    auto header          = getNextDefine();
    sourcePairCopy.first = header + sourcePair.first;

    auto vPreResult =
        compiler.PreprocessGlsl(sourcePairCopy.first,
                                shaderc_shader_kind::shaderc_vertex_shader,
                                "vertex",
                                options);

    if (vPreResult.GetCompilationStatus() !=
        shaderc_compilation_status_success) {
      std::cerr << "ERROR PREPROCESSING SHADER\n"
                << vPreResult.GetErrorMessage() << std::endl;
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
      std::cerr << "ERROR PREPROCESSING SHADER\n"
                << fPreResult.GetErrorMessage() << std::endl;
      std::exit(EXIT_FAILURE);
    }

    auto vSpirvResult =
        compiler.CompileGlslToSpv({vPreResult.cbegin(), vPreResult.cend()},
                                  shaderc_shader_kind::shaderc_vertex_shader,
                                  "vertex",
                                  options);

    if (vSpirvResult.GetCompilationStatus() !=
        shaderc_compilation_status_success) {
      std::cerr << "ERROR COMPILING SHADER\n"
                << vSpirvResult.GetErrorMessage() << std::endl;
      std::exit(EXIT_FAILURE);
    }

    auto fSpirvResult =
        compiler.CompileGlslToSpv({fPreResult.cbegin(), fPreResult.cend()},
                                  shaderc_shader_kind::shaderc_fragment_shader,
                                  "fragment",
                                  options);

    if (fSpirvResult.GetCompilationStatus() !=
        shaderc_compilation_status_success) {
      std::cerr << "ERROR COMPILING SHADER\n"
                << fSpirvResult.GetErrorMessage() << std::endl;
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

void BenchVulkan::createShaderModules(ResultCollection& resultCollection) {
  Timer t;
  t.start("Loading");
  auto sourcePair = loadShaderSource();
  t.stop();
  resultCollection.addResult(t);

  const int SHADER_PER_THREAD = _shaderModules.size() / _numberOfThreads;
  int       startIndex        = 0;
  int       endIndex          = SHADER_PER_THREAD;

  std::vector<std::thread> threads;
  threads.reserve(_numberOfThreads - 1);
  for (int i = 0; i < _numberOfThreads - 1; i++) {
    threads.emplace_back(&BenchVulkan::thread_createShaderModules,
                         this,
                         startIndex,
                         endIndex,
                         i,
                         sourcePair);

    startIndex += SHADER_PER_THREAD;
    endIndex += SHADER_PER_THREAD;
  }

  thread_createShaderModules(
      startIndex, _shaderModules.size(), _numberOfThreads - 1, sourcePair);

  for (auto& thread : threads) {
    thread.join();
  }
}

void BenchVulkan::thread_createPipelines(int startIndex,
                                         int endIndex,
                                         int threadIndex) {
  for (int i = startIndex; i < endIndex; i++) {
    vk::Pipeline& pipeline = _pipelines[i];

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
    vertShaderStageInfo.stage  = vk::ShaderStageFlagBits::eVertex;
    vertShaderStageInfo.module = _shaderModules[i].first;
    vertShaderStageInfo.pName  = "main";

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
    fragShaderStageInfo.stage  = vk::ShaderStageFlagBits::eFragment;
    fragShaderStageInfo.module = _shaderModules[i].second;
    fragShaderStageInfo.pName  = "main";

    vk::PipelineShaderStageCreateInfo shaderStageInfos[] = {
        vertShaderStageInfo, fragShaderStageInfo};

    vk::VertexInputBindingDescription vertexBindingDescriptions[1] = {};
    // Pos
    vertexBindingDescriptions[0].binding   = 0;
    vertexBindingDescriptions[0].stride    = sizeof(Vertex);
    vertexBindingDescriptions[0].inputRate = vk::VertexInputRate::eVertex;

    vk::VertexInputAttributeDescription vertexAttributeDescriptions[1] = {};
    // Pos
    vertexAttributeDescriptions[0].binding  = 0;
    vertexAttributeDescriptions[0].location = 0;
    vertexAttributeDescriptions[0].format   = vk::Format::eR32G32B32Sfloat;
    vertexAttributeDescriptions[0].offset   = 0;

    // Hook up the binding and attribute descriptions
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.vertexBindingDescriptionCount          = 1;
    vertexInputInfo.pVertexBindingDescriptions      = vertexBindingDescriptions;
    vertexInputInfo.vertexAttributeDescriptionCount = 1;
    vertexInputInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions;

    vk::PipelineInputAssemblyStateCreateInfo inputAssemply = {};
    inputAssemply.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssemply.primitiveRestartEnable =
        static_cast<vk::Bool32>(false);  // In lieu of VK_FALSE

    vk::Viewport viewport;
    viewport.x        = 0.f;
    viewport.y        = 0.f;
    viewport.width    = static_cast<float>(_swapchainContext.extent.width);
    viewport.height   = static_cast<float>(_swapchainContext.extent.height);
    viewport.minDepth = 0.f;
    viewport.maxDepth = 0.f;

    vk::Rect2D scissor;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent   = _swapchainContext.extent;

    vk::PipelineViewportStateCreateInfo viewportState = {};
    viewportState.viewportCount                       = 1;
    viewportState.pViewports                          = &viewport;
    viewportState.scissorCount                        = 1;
    viewportState.pScissors                           = &scissor;

    vk::PipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.depthClampEnable        = static_cast<vk::Bool32>(false);
    rasterizer.rasterizerDiscardEnable = static_cast<vk::Bool32>(false);
    rasterizer.polygonMode             = vk::PolygonMode::eFill;
    rasterizer.lineWidth               = 1.0f;
    rasterizer.cullMode                = vk::CullModeFlagBits::eNone;
    rasterizer.frontFace               = vk::FrontFace::eClockwise;
    rasterizer.depthBiasEnable         = static_cast<vk::Bool32>(false);
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp          = 0.0f;
    rasterizer.depthBiasSlopeFactor    = 0.0f;

    vk::PipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sampleShadingEnable   = static_cast<vk::Bool32>(false);
    multisampling.rasterizationSamples  = vk::SampleCountFlagBits::e1;
    multisampling.minSampleShading      = 1.0f;
    multisampling.pSampleMask           = nullptr;
    multisampling.alphaToCoverageEnable = static_cast<vk::Bool32>(false);
    multisampling.alphaToOneEnable      = static_cast<vk::Bool32>(false);

    vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask =
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = static_cast<vk::Bool32>(false);

    vk::PipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.logicOpEnable     = static_cast<vk::Bool32>(false);
    colorBlending.logicOp           = vk::LogicOp::eCopy;
    colorBlending.attachmentCount   = 1;
    colorBlending.pAttachments      = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    vk::GraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.stageCount          = 2;
    pipelineInfo.pStages             = shaderStageInfos;
    pipelineInfo.pVertexInputState   = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssemply;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState   = &multisampling;
    pipelineInfo.pDepthStencilState  = nullptr;
    pipelineInfo.pColorBlendState    = &colorBlending;
    pipelineInfo.pDynamicState       = nullptr;
    pipelineInfo.layout              = _pipelineLayout;
    pipelineInfo.renderPass          = _renderContext.renderPass;
    pipelineInfo.subpass             = 0;

    CRITICAL(_deviceContext.device.createGraphicsPipelines(
                 nullptr, 1, &pipelineInfo, nullptr, &pipeline),
             "createGraphicsPipeline");
  }
}

void BenchVulkan::createPipelines(ResultCollection& resultCollection) {
  const int PIPES_PER_THREAD = _pipelines.size() / _numberOfThreads;
  int       startIndex       = 0;
  int       endIndex         = PIPES_PER_THREAD;

  std::vector<std::thread> threads;
  threads.reserve(_numberOfThreads - 1);
  for (int i = 0; i < _numberOfThreads - 1; i++) {
    threads.emplace_back(
        &BenchVulkan::thread_createPipelines, this, startIndex, endIndex, i);

    startIndex += PIPES_PER_THREAD;
    endIndex += PIPES_PER_THREAD;
  }

  thread_createPipelines(startIndex, _pipelines.size(), _numberOfThreads - 1);

  for (auto& thread : threads) {
    thread.join();
  }
}

void BenchVulkan::thread_singleTriangleDraw(
    int                                startIndex,
    int                                endIndex,
    int                                threadIndex,
    std::array<Triangle, BENCHMARK_N>* triangles) {
  ThreadContext& threadContext = _threadContexts[threadIndex];

  uint32_t currentIndex = _renderContext.currentSwapChainImageIndex;

  vk::CommandBuffer& currentCommandBuffer =
      threadContext.commandBuffers[currentIndex];

  vk::CommandBufferBeginInfo beginInfo;
  beginInfo.flags            = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
  beginInfo.pInheritanceInfo = nullptr;

  vk::ClearValue clearColor;
  clearColor.color.float32[0] = 0.6f;
  clearColor.color.float32[1] = 0.0f;
  clearColor.color.float32[2] = 0.3f;
  clearColor.color.float32[3] = 1.0f;

  vk::RenderPassBeginInfo renderPassBeginInfo;
  renderPassBeginInfo.renderPass = _renderContext.renderPass;
  renderPassBeginInfo.framebuffer =
      _swapchainContext.framebuffers[currentIndex];
  renderPassBeginInfo.renderArea.offset.x = 0;
  renderPassBeginInfo.renderArea.offset.y = 0;
  renderPassBeginInfo.renderArea.extent   = _swapchainContext.extent;
  renderPassBeginInfo.clearValueCount     = 1;
  renderPassBeginInfo.pClearValues        = &clearColor;

  currentCommandBuffer.begin(beginInfo);
  currentCommandBuffer.beginRenderPass(renderPassBeginInfo,
                                       vk::SubpassContents::eInline);

  for (int i = startIndex; i < endIndex; i++) {
    currentCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                      _pipelines[i]);
    currentCommandBuffer.bindVertexBuffers(0, {(*triangles)[i].buffer}, {0});
    currentCommandBuffer.draw(3, 1, 0, 0);
  }
  currentCommandBuffer.endRenderPass();
  currentCommandBuffer.end();
}

void BenchVulkan::singleTriangleDraw(ResultCollection& resultCollection,
                                     bool              device) {
  Timer t;
  t.start("Construction");

  std::array<Triangle, BENCHMARK_N>* triangles;
  if (device) {
    triangles = &_trianglesDevice;
  } else {
    triangles = &_trianglesHost;
  }
  _renderContext.currentSwapChainImageIndex =
      _deviceContext.device
          .acquireNextImageKHR(_swapchainContext.swapchain,
                               (std::numeric_limits<uint64_t>::max)(),
                               _renderContext.imageAvailableSemaphore,
                               nullptr)
          .value;

  const int DRAWS_PER_THREAD = _pipelines.size() / _numberOfThreads;
  int       startIndex       = 0;
  int       endIndex         = DRAWS_PER_THREAD;

  std::vector<std::thread> threads;
  threads.reserve(_numberOfThreads - 1);
  for (int i = 0; i < _numberOfThreads - 1; i++) {
    threads.emplace_back(&BenchVulkan::thread_singleTriangleDraw,
                         this,
                         startIndex,
                         endIndex,
                         i,
                         triangles);

    startIndex += DRAWS_PER_THREAD;
    endIndex += DRAWS_PER_THREAD;
  }

  thread_singleTriangleDraw(
      startIndex, triangles->size(), _numberOfThreads - 1, triangles);

  for (auto& thread : threads) {
    thread.join();
  }

  t.stop();
  resultCollection.addResult(t);

  t.start("Submission");
  // Rendering
  drawSubmission();
  t.stop();
  resultCollection.addResult(t);

  t.start("Waiting");
  _threadContexts[_numberOfThreads - 1].queue.waitIdle();
  t.stop();
  resultCollection.addResult(t);
}

void BenchVulkan::thread_optimalMultiTriangleDraw(
    int                                startIndex,
    int                                endIndex,
    int                                threadIndex,
    std::array<Triangle, BENCHMARK_N>* triangles) {
  ThreadContext& threadContext = _threadContexts[threadIndex];

  uint32_t currentIndex = _renderContext.currentSwapChainImageIndex;

  vk::CommandBuffer& currentCommandBuffer =
      threadContext.commandBuffers[currentIndex];

  vk::CommandBufferBeginInfo beginInfo;
  beginInfo.flags            = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
  beginInfo.pInheritanceInfo = nullptr;

  vk::ClearValue clearColor;
  clearColor.color.float32[0] = 0.6f;
  clearColor.color.float32[1] = 0.0f;
  clearColor.color.float32[2] = 0.3f;
  clearColor.color.float32[3] = 1.0f;

  vk::RenderPassBeginInfo renderPassBeginInfo;
  renderPassBeginInfo.renderPass = _renderContext.renderPass;
  renderPassBeginInfo.framebuffer =
      _swapchainContext.framebuffers[currentIndex];
  renderPassBeginInfo.renderArea.offset.x = 0;
  renderPassBeginInfo.renderArea.offset.y = 0;
  renderPassBeginInfo.renderArea.extent   = _swapchainContext.extent;
  renderPassBeginInfo.clearValueCount     = 1;
  renderPassBeginInfo.pClearValues        = &clearColor;

  currentCommandBuffer.begin(beginInfo);
  currentCommandBuffer.beginRenderPass(renderPassBeginInfo,
                                       vk::SubpassContents::eInline);

  for (int i = startIndex; i < endIndex; i++) {
    currentCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                      _pipelines[i]);
    for (int j = 0; j < BENCHMARK_M; j++) {
      currentCommandBuffer.bindVertexBuffers(0, {(*triangles)[j].buffer}, {0});
      currentCommandBuffer.draw(3, 1, 0, 0);
    }
  }
  currentCommandBuffer.endRenderPass();
  currentCommandBuffer.end();
}

void BenchVulkan::optimalMultipleTriangleDraw(
    ResultCollection& resultCollection, bool device) {
  Timer t;
  t.start("Construction");

  std::array<Triangle, BENCHMARK_N>* triangles;
  if (device) {
    triangles = &_trianglesDevice;
  } else {
    triangles = &_trianglesHost;
  }
  _renderContext.currentSwapChainImageIndex =
      _deviceContext.device
          .acquireNextImageKHR(_swapchainContext.swapchain,
                               (std::numeric_limits<uint64_t>::max)(),
                               _renderContext.imageAvailableSemaphore,
                               nullptr)
          .value;

  const int DRAWS_PER_THREAD = _pipelines.size() / _numberOfThreads;
  int       startIndex       = 0;
  int       endIndex         = DRAWS_PER_THREAD;

  std::vector<std::thread> threads;
  threads.reserve(_numberOfThreads - 1);
  for (int i = 0; i < _numberOfThreads - 1; i++) {
    threads.emplace_back(&BenchVulkan::thread_optimalMultiTriangleDraw,
                         this,
                         startIndex,
                         endIndex,
                         i,
                         triangles);

    startIndex += DRAWS_PER_THREAD;
    endIndex += DRAWS_PER_THREAD;
  }

  thread_optimalMultiTriangleDraw(
      startIndex, triangles->size(), _numberOfThreads - 1, triangles);

  for (auto& thread : threads) {
    thread.join();
  }

  t.stop();
  resultCollection.addResult(t);

  t.start("Submission");
  // Rendering
  drawSubmission();
  t.stop();
  resultCollection.addResult(t);

  t.start("Waiting");
  _threadContexts[_numberOfThreads - 1].queue.waitIdle();
  t.stop();
  resultCollection.addResult(t);
}

void BenchVulkan::thread_badMultipleTriangleDraw(
    int                                startIndex,
    int                                endIndex,
    int                                threadIndex,
    std::array<Triangle, BENCHMARK_N>* triangles) {
  ThreadContext& threadContext = _threadContexts[threadIndex];

  uint32_t currentIndex = _renderContext.currentSwapChainImageIndex;

  vk::CommandBuffer& currentCommandBuffer =
      threadContext.commandBuffers[currentIndex];

  vk::CommandBufferBeginInfo beginInfo;
  beginInfo.flags            = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
  beginInfo.pInheritanceInfo = nullptr;

  vk::ClearValue clearColor;
  clearColor.color.float32[0] = 0.6f;
  clearColor.color.float32[1] = 0.0f;
  clearColor.color.float32[2] = 0.3f;
  clearColor.color.float32[3] = 1.0f;

  vk::RenderPassBeginInfo renderPassBeginInfo;
  renderPassBeginInfo.renderPass = _renderContext.renderPass;
  renderPassBeginInfo.framebuffer =
      _swapchainContext.framebuffers[currentIndex];
  renderPassBeginInfo.renderArea.offset.x = 0;
  renderPassBeginInfo.renderArea.offset.y = 0;
  renderPassBeginInfo.renderArea.extent   = _swapchainContext.extent;
  renderPassBeginInfo.clearValueCount     = 1;
  renderPassBeginInfo.pClearValues        = &clearColor;

  currentCommandBuffer.begin(beginInfo);
  currentCommandBuffer.beginRenderPass(renderPassBeginInfo,
                                       vk::SubpassContents::eInline);

  for (int j = 0; j < BENCHMARK_M; j++) {
    for (int i = startIndex; i < endIndex; i++) {
      currentCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                        _pipelines[i]);
      currentCommandBuffer.bindVertexBuffers(0, {(*triangles)[j].buffer}, {0});
      currentCommandBuffer.draw(3, 1, 0, 0);
    }
  }
  currentCommandBuffer.endRenderPass();
  currentCommandBuffer.end();
}

void BenchVulkan::badMultipleTriangleDraw(ResultCollection& resultCollection,
                                          bool              device) {
  Timer t;
  t.start("Construction");

  std::array<Triangle, BENCHMARK_N>* triangles;
  if (device) {
    triangles = &_trianglesDevice;
  } else {
    triangles = &_trianglesHost;
  }
  _renderContext.currentSwapChainImageIndex =
      _deviceContext.device
          .acquireNextImageKHR(_swapchainContext.swapchain,
                               (std::numeric_limits<uint64_t>::max)(),
                               _renderContext.imageAvailableSemaphore,
                               nullptr)
          .value;

  const int DRAWS_PER_THREAD = _pipelines.size() / _numberOfThreads;
  int       startIndex       = 0;
  int       endIndex         = DRAWS_PER_THREAD;

  std::vector<std::thread> threads;
  threads.reserve(_numberOfThreads - 1);
  for (int i = 0; i < _numberOfThreads - 1; i++) {
    threads.emplace_back(&BenchVulkan::thread_badMultipleTriangleDraw,
                         this,
                         startIndex,
                         endIndex,
                         i,
                         triangles);

    startIndex += DRAWS_PER_THREAD;
    endIndex += DRAWS_PER_THREAD;
  }

  thread_badMultipleTriangleDraw(
      startIndex, triangles->size(), _numberOfThreads - 1, triangles);

  for (auto& thread : threads) {
    thread.join();
  }

  t.stop();
  resultCollection.addResult(t);

  t.start("Submission");
  // Rendering
  drawSubmission();
  t.stop();
  resultCollection.addResult(t);

  t.start("Waiting");
  _threadContexts[_numberOfThreads - 1].queue.waitIdle();
  t.stop();
  resultCollection.addResult(t);
}

void BenchVulkan::drawSubmission() {
  {
    std::vector<vk::CommandBuffer> commandBuffers;
    commandBuffers.reserve(_numberOfThreads);
    for (auto& threadContext : _threadContexts) {
      commandBuffers.push_back(
          threadContext
              .commandBuffers[_renderContext.currentSwapChainImageIndex]);
    }
    vk::Semaphore waitSemaphores[]   = {_renderContext.imageAvailableSemaphore};
    vk::Semaphore signalSemaphores[] = {_renderContext.renderFinishedSemaphore};
    vk::PipelineStageFlags waitStages[] = {
        vk::PipelineStageFlagBits::eColorAttachmentOutput};

    vk::SubmitInfo submitInfo;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = waitSemaphores;
    submitInfo.pWaitDstStageMask    = waitStages;
    submitInfo.commandBufferCount   = commandBuffers.size();
    submitInfo.pCommandBuffers      = commandBuffers.data();
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores;

    _threadContexts[_numberOfThreads - 1].queue.submit(submitInfo, nullptr);
  }

  // Presentation
  {
    vk::Semaphore waitSemaphores[] = {
        _renderContext.renderFinishedSemaphore};  // Wait for this...
    vk::Semaphore signalSemaphores[] = {
        _renderContext.imageAvailableSemaphore};  // Signal this when finished.

    vk::SwapchainKHR swapChains[] = {_swapchainContext.swapchain};

    vk::PresentInfoKHR presentInfo;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = waitSemaphores;
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = swapChains;
    presentInfo.pImageIndices      = &_renderContext.currentSwapChainImageIndex;
    presentInfo.pResults           = nullptr;

    _threadContexts[_numberOfThreads - 1].queue.presentKHR(presentInfo);
  }
}

void BenchVulkan::clean_up(ResultCollection& resultCollection) {
  _deviceContext.device.waitIdle();

  for (auto& pipeline : _pipelines) {
    _deviceContext.device.destroyPipeline(pipeline);
  }

  for (auto& shaderModule : _shaderModules) {
    _deviceContext.device.destroyShaderModule(shaderModule.first);
    _deviceContext.device.destroyShaderModule(shaderModule.second);
  }

  for (auto& triangle : _trianglesDevice) {
    _deviceContext.device.destroyBuffer(triangle.buffer, nullptr);
    _deviceContext.device.freeMemory(triangle.memory, nullptr);
    triangle.buffer = nullptr;
    triangle.memory = nullptr;
  }
  for (auto& triangle : _trianglesHost) {
    _deviceContext.device.destroyBuffer(triangle.buffer, nullptr);
    _deviceContext.device.freeMemory(triangle.memory, nullptr);
  }

  _deviceContext.device.destroySemaphore(
      _renderContext.renderFinishedSemaphore);
  _deviceContext.device.destroySemaphore(
      _renderContext.imageAvailableSemaphore);

  _deviceContext.device.destroyPipelineLayout(_pipelineLayout);

  for (auto& threadContext : _threadContexts) {
    _deviceContext.device.freeCommandBuffers(threadContext.commandPool,
                                             threadContext.commandBuffers);

    _deviceContext.device.destroyCommandPool(threadContext.commandPool);
    // _deviceContext.device.destroyCommandPool(
    //     threadContext.commandPool[TRANSFER]);
  }

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

#if USE_VALIDATION_LAYERS
  auto destroyDebugCallback = GET_PROC(vkDestroyDebugReportCallbackEXT);
  destroyDebugCallback(
      _instance, static_cast<VkDebugReportCallbackEXT>(_callback), nullptr);
#endif  // USE_VALIDATION_LAYERS

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
  if (queueFamilies[0].queueFlags == vk::QueueFlagBits(15) &&
      queueFamilies[0].queueCount == 16) {
    graphicsQueueIndex = 0;
    presentQueueIndex  = 0;
    transferQueueIndex = 0;

    // We need to call this...
    availableDevice.getSurfaceSupportKHR(0, _deviceContext.surface);

  } else {
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
  float queuePriorities[QueueType::NUM_QUEUES * 32] = {1.0f};

  struct QueueConstruct {
    std::vector<QueueType> types;
    int                    family = 0;
    int                    count  = 0;
  };
  std::array<QueueConstruct, 1> queueConstructs;
  queueConstructs[0].family = 0;
  queueConstructs[0].count  = _numberOfThreads;
  // queueConstructs[1].family = 1;
  // queueConstructs[1].count  = 1;

  // std::vector<QueueConstruct>            queueConstructs = {};
  std::vector<vk::DeviceQueueCreateInfo> queueInfos = {};

  // for (int i = 0; i < QueueType::NUM_QUEUES; i++) {
  //   bool already_exists = false;

  //   for (auto& queueConstruct : queueConstructs) {
  //     if (queueConstruct.family == _deviceContext.queueIndices[i]) {
  //       already_exists = true;
  //       queueConstruct.count++;
  //       queueConstruct.types.push_back(static_cast<QueueType>(i));
  //     }
  //   }
  //   if (!already_exists) {
  //     queueConstructs.push_back(
  //         {{static_cast<QueueType>(i)}, _deviceContext.queueIndices[i], 1});
  //   }
  // }

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

  for (int i = 0; i < _threadContexts.size(); i++) {
    _threadContexts[i].queue = _deviceContext.device.getQueue(0, i);
  }

  // _presentQueue = _deviceContext.device.getQueue(1, 0);

  // for (auto& threadContext : _threadContexts) {
  //   int t = 0;
  //   for (auto& queueConstruct : queueConstructs) {
  //     for (int i = 0; i < queueConstruct.types.size(); i++) {
  //       threadContext.queues[queueConstruct.types[i]] =
  //           _deviceContext.device.getQueue(queueConstruct.family,
  //                                          i + queueConstruct.types.size() *
  //                                          t);
  //     }
  //   }
  //   t++;
  // }
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
  for (auto& threadContext : _threadContexts) {
    vk::CommandPoolCreateInfo poolInfo;
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = 0;

    CRITICAL(_deviceContext.device.createCommandPool(
                 &poolInfo, nullptr, &threadContext.commandPool),
             "createCommandPool");

    // vk::CommandPoolCreateInfo poolInfo;
    // poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    // poolInfo.queueFamilyIndex = _deviceContext.queueIndices[GRAPHICS];

    // CRITICAL(_deviceContext.device.createCommandPool(
    //              &poolInfo, nullptr, &threadContext.commandPool[GRAPHICS]),
    //          "createCommandPool");

    // vk::CommandPoolCreateInfo transferPoolInfo;
    // transferPoolInfo.flags =
    // vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    // transferPoolInfo.queueFamilyIndex =
    // _deviceContext.queueIndices[TRANSFER];

    // CRITICAL(
    //     _deviceContext.device.createCommandPool(
    //         &transferPoolInfo, nullptr,
    //         &threadContext.commandPool[TRANSFER]),
    //     "createCommandPool");
  }
}

void BenchVulkan::makeCommandBuffers() {
  for (auto& threadContext : _threadContexts) {
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.commandPool = threadContext.commandPool;
    allocInfo.level       = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount =
        static_cast<uint32_t>(_swapchainContext.framebuffers.size());

    threadContext.commandBuffers.resize(_swapchainContext.framebuffers.size());

    _deviceContext.device.allocateCommandBuffers(
        &allocInfo, threadContext.commandBuffers.data());
  }
}

void BenchVulkan::makePipelineLayout() {
  vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.setLayoutCount         = 0;
  pipelineLayoutInfo.pSetLayouts            = nullptr;

  CRITICAL(_deviceContext.device.createPipelineLayout(
               &pipelineLayoutInfo, nullptr, &_pipelineLayout),
           "createPipelineLayout");
}

void BenchVulkan::makeSemaphores() {
  vk::SemaphoreCreateInfo semaphoreInfo;
  CRITICAL(
      _deviceContext.device.createSemaphore(
          &semaphoreInfo, nullptr, &_renderContext.imageAvailableSemaphore),
      "createSemaphore");
  CRITICAL(
      _deviceContext.device.createSemaphore(
          &semaphoreInfo, nullptr, &_renderContext.renderFinishedSemaphore),
      "createSemaphore");
}

uint32_t BenchVulkan::findMemoryType(uint32_t                typeFilter,
                                     vk::MemoryPropertyFlags properties) {
  vk::PhysicalDeviceMemoryProperties memProperties;
  _deviceContext.physicalDevice.getMemoryProperties(&memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      return i;
    }
  }

  // throw std::runtime_error("failed to find suitable memory type!");
  std::exit(EXIT_FAILURE);
  return 0;
}

// std::mutex bufMutex;

std::pair<vk::Buffer, vk::DeviceMemory> BenchVulkan::getNewBuffer(
    vk::DeviceSize          size,
    vk::BufferUsageFlags    usage,
    vk::MemoryPropertyFlags properties) {
  //
  std::pair<vk::Buffer, vk::DeviceMemory> result;

  // bufMutex.lock();

  vk::BufferCreateInfo bufferInfo;
  bufferInfo.size        = size;
  bufferInfo.usage       = usage;  // vk::BufferUsageFlagBits::eVertexBuffer;
  bufferInfo.sharingMode = vk::SharingMode::eExclusive;

  CRITICAL(
      _deviceContext.device.createBuffer(&bufferInfo, nullptr, &result.first),
      "createBuffer");

  vk::MemoryRequirements memReq;
  _deviceContext.device.getBufferMemoryRequirements(result.first, &memReq);

  vk::MemoryAllocateInfo allocInfo;
  allocInfo.allocationSize  = memReq.size;
  allocInfo.memoryTypeIndex = findMemoryType(memReq.memoryTypeBits, properties);

  CRITICAL(
      _deviceContext.device.allocateMemory(&allocInfo, nullptr, &result.second),
      "allocateMemory");
  _deviceContext.device.bindBufferMemory(result.first, result.second, 0);

  // std::cout << "Thread " << std::this_thread::get_id()
  //           << " says Buffer is: " << result.first << std::endl;

  return result;
}

vk::CommandBuffer BenchVulkan::getTransferCommandBuffer(int threadIndex) {
  vk::CommandBufferAllocateInfo allocInfo;
  allocInfo.level              = vk::CommandBufferLevel::ePrimary;
  allocInfo.commandPool        = _threadContexts[threadIndex].commandPool;
  allocInfo.commandBufferCount = 1;

  vk::CommandBuffer commandBuffer;
  _deviceContext.device.allocateCommandBuffers(&allocInfo, &commandBuffer);

  return commandBuffer;
}

vk::CommandBuffer BenchVulkan::getGraphicsCommandBuffer(int threadIndex) {
  vk::CommandBufferAllocateInfo allocInfo;
  allocInfo.level              = vk::CommandBufferLevel::ePrimary;
  allocInfo.commandPool        = _threadContexts[threadIndex].commandPool;
  allocInfo.commandBufferCount = 1;

  vk::CommandBuffer commandBuffer;
  _deviceContext.device.allocateCommandBuffers(&allocInfo, &commandBuffer);

  return commandBuffer;
}

void BenchVulkan::copyBufferOnce(vk::Buffer     srcBuffer,
                                 vk::Buffer     dstBuffer,
                                 vk::DeviceSize size) {
  // vk::CommandBuffer commandBuffer = getTransferCommandBuffer();
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
