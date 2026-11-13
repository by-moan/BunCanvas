#include "src/gpu/GpuTypesPriv.h"
#include "src/gpu/vk/vulkanmemoryallocator/VulkanMemoryAllocatorPriv.h"

namespace VK {
    namespace {
        // Core Vulkan device & instance handles
        VkInstance instance = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        uint32_t negotiatedApiVersion = VK_API_VERSION_1_0;

        // Queues for JS (main) thread and Render thread
        uint32_t graphicsQueueFamily = 0;
        VkQueue mainQueue = VK_NULL_HANDLE;
        VkQueue renderQueue = VK_NULL_HANDLE;

        // Enabled extensions kept alive for Skia
        std::vector<const char*> instanceExtensions;
        std::vector<const char*> deviceExtensions;
        skgpu::VulkanExtensions skiaExtensions;

        // Window presentation & swapchain state
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        VkFormat swapchainFormat = VK_FORMAT_B8G8R8A8_UNORM;
        VkColorSpaceKHR swapchainColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        VkExtent2D swapchainExtent{0, 0};
        std::vector<VkImage> swapchainImages;

        // Frame synchronization primitives
        VkSemaphore imageAvailableSem = VK_NULL_HANDLE;
        VkSemaphore renderFinishedSem = VK_NULL_HANDLE;
        VkFence inFlightFence = VK_NULL_HANDLE;

        size_t getProcCallCount = 0;

        PFN_vkVoidFunction getProc(const char* name, VkInstance inst, VkDevice dev) {
            ++getProcCallCount;
            PFN_vkVoidFunction proc = nullptr;
            if (dev != VK_NULL_HANDLE) {
                proc = vkGetDeviceProcAddr(dev, name);
            }
            if (!proc) {
                proc = vkGetInstanceProcAddr(inst, name);
            }
            if (!proc) {
                std::cerr << "[VK] Missing Vulkan proc: " << name << "\n";
            }
            return proc;
        }

        skgpu::VulkanBackendContext buildBackendContext(VkQueue targetQueue) {
            skgpu::VulkanBackendContext ctx{};
            ctx.fInstance = instance;
            ctx.fPhysicalDevice = physicalDevice;
            ctx.fDevice = device;
            ctx.fQueue = targetQueue;
            ctx.fGraphicsQueueIndex = graphicsQueueFamily;
            ctx.fMaxAPIVersion = negotiatedApiVersion;
            ctx.fVkExtensions = &skiaExtensions;
            ctx.fGetProc = getProc;
            ctx.fMemoryAllocator = skgpu::VulkanMemoryAllocators::Make(ctx, skgpu::ThreadSafe::kNo);
            return ctx;
        }

        void destroySwapchainOnly() {
            if (device == VK_NULL_HANDLE) return;
            swapchainImages.clear();
            if (swapchain != VK_NULL_HANDLE) {
                vkDestroySwapchainKHR(device, swapchain, nullptr);
                swapchain = VK_NULL_HANDLE;
            }
        }

        std::mutex sharedQueueMtx;
    } // anonymous namespace

    // 1. Initialize VkInstance, VkPhysicalDevice, VkDevice, Queues, and Skia VulkanExtensions
    bool initCore(bool enableValidation = false) {
        if (device != VK_NULL_HANDLE) return true;

        uint32_t glfwExtCount = 0;
        const char** glfwExts = glfwGetRequiredInstanceExtensions(&glfwExtCount);
        if (!glfwExts) {
            std::cerr << "GLFW failed to find Vulkan instance extensions\n";
            return false;
        }

        instanceExtensions.assign(glfwExts, glfwExts + glfwExtCount);
        std::vector<const char*> layers;
        if (enableValidation) {
            instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            layers.push_back("VK_LAYER_KHRONOS_validation");
        }
#ifdef __APPLE__
        instanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

        uint32_t queriedVersion = VK_API_VERSION_1_0;
        auto enumInstVer = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(
            vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion")
        );
        if (enumInstVer) {
            enumInstVer(&queriedVersion);
        }
        negotiatedApiVersion = queriedVersion;

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "BunCanvas";
        appInfo.apiVersion = negotiatedApiVersion;

        VkInstanceCreateInfo instInfo{};
        instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instInfo.pApplicationInfo = &appInfo;
#ifdef __APPLE__
        uint32_t instExtCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &instExtCount, nullptr);
        std::vector<VkExtensionProperties> availInstExts(instExtCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &instExtCount, availInstExts.data());

        bool hasPortabilityEnum = std::any_of(
            availInstExts.begin(), availInstExts.end(),
            [](const VkExtensionProperties& ext) {
                return std::strcmp(ext.extensionName, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0;
            }
        );

        if (hasPortabilityEnum) {
            if (std::none_of(instanceExtensions.begin(), instanceExtensions.end(), [](const char* e) {
                return std::strcmp(e, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0;
            })) {
                instanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            }
            instInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        } else {
            instanceExtensions.erase(
                std::remove_if(instanceExtensions.begin(), instanceExtensions.end(), [](const char* e) {
                    return std::strcmp(e, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0;
                }),
                instanceExtensions.end()
            );
        }
#endif
        instInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
        instInfo.ppEnabledExtensionNames = instanceExtensions.data();
        instInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
        instInfo.ppEnabledLayerNames = layers.empty() ? nullptr : layers.data();

        VkResult instRes = vkCreateInstance(&instInfo, nullptr, &instance);
        if (instRes != VK_SUCCESS) {
            std::cerr << "vkCreateInstance failed: " << instRes << "\n";
            return false;
        }

        // Pick a GPU (prefer discrete/integrated over CPU llvmpipe)
        uint32_t gpuCount = 0;
        vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
        if (gpuCount == 0) {
            std::cerr << "No Vulkan physical devices found\n";
            return false;
        }
        std::vector<VkPhysicalDevice> gpus(gpuCount);
        vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.data());

        physicalDevice = gpus[0];
        for (VkPhysicalDevice gpu : gpus) {
            VkPhysicalDeviceProperties props{};
            vkGetPhysicalDeviceProperties(gpu, &props);
            if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
                props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
                physicalDevice = gpu;
                break;
            }
        }

        VkPhysicalDeviceProperties chosenProps{};
        vkGetPhysicalDeviceProperties(physicalDevice, &chosenProps);
        std::cout << "Vulkan GPU: " << chosenProps.deviceName << "\n";

        // Find graphics + present queue family
        uint32_t qfCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &qfCount, nullptr);
        std::vector<VkQueueFamilyProperties> qfProps(qfCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &qfCount, qfProps.data());

        bool foundQueue = false;
        uint32_t maxQueues = 1;
        for (uint32_t i = 0; i < qfCount; ++i) {
            if ((qfProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
                glfwGetPhysicalDevicePresentationSupport(instance, physicalDevice, i)) {
                graphicsQueueFamily = i;
                maxQueues = qfProps[i].queueCount;
                foundQueue = true;
                break;
            }
        }

        if (!foundQueue) {
            std::cerr << "No Vulkan queue family supports both graphics and presentation\n";
            return false;
        }

        uint32_t queueCount = std::min(maxQueues, 2u);
        float priorities[2] = {1.0f, 1.0f};

        VkDeviceQueueCreateInfo qInfo{};
        qInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qInfo.queueFamilyIndex = graphicsQueueFamily;
        qInfo.queueCount = queueCount;
        qInfo.pQueuePriorities = priorities;

        deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
#ifdef __APPLE__
        uint32_t devExtCount = 0;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &devExtCount, nullptr);
        std::vector<VkExtensionProperties> availDevExts(devExtCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &devExtCount, availDevExts.data());
        for (const auto& ext : availDevExts) {
            if (std::strcmp(ext.extensionName, "VK_KHR_portability_subset") == 0) {
                deviceExtensions.push_back("VK_KHR_portability_subset");
                break;
            }
        }
#endif

        VkDeviceCreateInfo devInfo{};
        devInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        devInfo.queueCreateInfoCount = 1;
        devInfo.pQueueCreateInfos = &qInfo;
        devInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        devInfo.ppEnabledExtensionNames = deviceExtensions.data();

        VkResult devRes = vkCreateDevice(physicalDevice, &devInfo, nullptr, &device);
        if (devRes != VK_SUCCESS) {
            std::cerr << "vkCreateDevice failed: " << devRes << "\n";
            return false;
        }

        vkGetDeviceQueue(device, graphicsQueueFamily, 0, &mainQueue);
        vkGetDeviceQueue(device, graphicsQueueFamily, queueCount > 1 ? 1 : 0, &renderQueue);

        skiaExtensions.init(
            getProc,
            instance,
            physicalDevice,
            static_cast<uint32_t>(instanceExtensions.size()),
            instanceExtensions.data(),
            static_cast<uint32_t>(deviceExtensions.size()),
            deviceExtensions.data()
        );

        return true;
    }

    // Add a helper in namespace VK:
    //Forward declaration. Implementation is in Canvas.cpp
    void flushAndSyncMainContext(GrDirectContext* mainCtx);

    // 2. Factories for your global GrDirectContext pointers
    sk_sp<GrDirectContext> createMainThreadContext() {
        getProcCallCount = 0;
        auto ctx = GrDirectContexts::MakeVulkan(buildBackendContext(mainQueue));
        std::cerr << "[VK] MakeVulkan (main): ctx=" << ctx.get()
                << " getProcCalls=" << getProcCallCount
                << " apiVersion=0x" << std::hex << negotiatedApiVersion << std::dec << "\n";
        if (ctx == nullptr) std::cout << "[CRIT_ERROR] createMainThreadContext() returned nullptr! \n";
        return ctx;
    }

    sk_sp<GrDirectContext> createRenderThreadContext() {
        return GrDirectContexts::MakeVulkan(buildBackendContext(renderQueue));
    }

    // 3. Create or recreate the swapchain (used on startup, resize, and vsync toggle)
    bool recreateSwapchain(int width, int height, bool vsync) {
        if (device == VK_NULL_HANDLE || surface == VK_NULL_HANDLE) return false;
        vkDeviceWaitIdle(device);
        destroySwapchainOnly();

        VkSurfaceCapabilitiesKHR caps{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps);

        uint32_t fmtCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &fmtCount, nullptr);
        std::vector<VkSurfaceFormatKHR> formats(fmtCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &fmtCount, formats.data());

        swapchainFormat = formats[0].format;
        swapchainColorSpace = formats[0].colorSpace;
        for (const auto& f : formats) {
            if (f.format == VK_FORMAT_B8G8R8A8_UNORM &&
                f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                swapchainFormat = f.format;
                swapchainColorSpace = f.colorSpace;
                break;
            }
        }

        swapchainExtent = {
            std::clamp(static_cast<uint32_t>(std::max(width, 1)), caps.minImageExtent.width, caps.maxImageExtent.width),
            std::clamp(static_cast<uint32_t>(std::max(height, 1)), caps.minImageExtent.height, caps.maxImageExtent.height)
        };

        uint32_t imageCount = caps.minImageCount + 1;
        if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount) {
            imageCount = caps.maxImageCount;
        }

        VkSwapchainCreateInfoKHR scInfo{};
        scInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        scInfo.surface = surface;
        scInfo.minImageCount = imageCount;
        scInfo.imageFormat = swapchainFormat;
        scInfo.imageColorSpace = swapchainColorSpace;
        scInfo.imageExtent = swapchainExtent;
        scInfo.imageArrayLayers = 1;
        scInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                          | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                          | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        scInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        scInfo.preTransform = caps.currentTransform;
        scInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        scInfo.presentMode = vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;
        scInfo.clipped = VK_TRUE;

        if (vkCreateSwapchainKHR(device, &scInfo, nullptr, &swapchain) != VK_SUCCESS) return false;

        uint32_t actualCount = 0;
        vkGetSwapchainImagesKHR(device, swapchain, &actualCount, nullptr);
        swapchainImages.resize(actualCount);
        vkGetSwapchainImagesKHR(device, swapchain, &actualCount, swapchainImages.data());

        return true;
    }

    // 4. Attach GLFW window & create synchronization primitives
    bool initWindowSurface(GLFWwindow* win, int width, int height, bool vsync) {
        if (glfwCreateWindowSurface(instance, win, nullptr, &surface) != VK_SUCCESS) return false;

        VkSemaphoreCreateInfo semInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        VkFenceCreateInfo fenceInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateSemaphore(device, &semInfo, nullptr, &imageAvailableSem) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semInfo, nullptr, &renderFinishedSem) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
            return false;
        }

        return recreateSwapchain(width, height, vsync);
    }

    // 5. Swapchain accessors for wrapping global SkSurface objects
    uint32_t getSwapchainImageCount() {
        return static_cast<uint32_t>(swapchainImages.size());
    }

    SkColorType getSwapchainColorType() {
        return (swapchainFormat == VK_FORMAT_R8G8B8A8_UNORM)
            ? kRGBA_8888_SkColorType
            : kBGRA_8888_SkColorType;
    }

    GrBackendRenderTarget getSwapchainRenderTarget(uint32_t index) {
        GrVkImageInfo info{};
        info.fImage = swapchainImages[index];
        info.fImageTiling = VK_IMAGE_TILING_OPTIMAL;
        info.fImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        info.fFormat = swapchainFormat;
        info.fImageUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                              | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                              | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        info.fSampleCount = 1;
        info.fLevelCount = 1;

        return GrBackendRenderTargets::MakeVk(
            static_cast<int>(swapchainExtent.width),
            static_cast<int>(swapchainExtent.height),
            info
        );
    }

    // 6. Frame acquisition & presentation
    bool acquireNextImage(uint32_t* outIndex) {
        vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);

        VkResult res = vkAcquireNextImageKHR(
            device, swapchain, UINT64_MAX, imageAvailableSem, VK_NULL_HANDLE, outIndex
        );
        if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
            return false;
        }

        // Only reset after a successful acquire so a failed acquire doesn't leave the fence unsignaled
        vkResetFences(device, 1, &inFlightFence);
        return true;
    }

    void presentImage(GrDirectContext* renderCtx, SkSurface* targetSurface, uint32_t imageIndex) {
        GrBackendSemaphore waitSem = GrBackendSemaphores::MakeVk(imageAvailableSem);
        GrBackendSemaphore signalSem = GrBackendSemaphores::MakeVk(renderFinishedSem);

        // 1. Wait for the acquired swapchain image before Skia executes draw commands
        renderCtx->wait(1, &waitSem, false);

        // 2. Transition swapchain image layout to PRESENT_SRC_KHR and attach the signal semaphore
        skgpu::MutableTextureState presentState =
            skgpu::MutableTextureStates::MakeVulkan(
                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                VK_QUEUE_FAMILY_IGNORED
            );

        GrFlushInfo flushInfo{};
        flushInfo.fNumSemaphores = 1;
        flushInfo.fSignalSemaphores = &signalSem;

        renderCtx->flush(
            targetSurface,
            flushInfo,
            &presentState
        );

        // Synchronize queue access when mainQueue and renderQueue share the same VkQueue
        std::unique_lock<std::mutex> lock(sharedQueueMtx, std::defer_lock);
        if (mainQueue == renderQueue) {
            lock.lock();
        }

        // 3. Submit recorded Vulkan command buffers to the render queue
        GrSubmitInfo submitInfo{};
        submitInfo.fSync = GrSyncCpu::kNo;
        renderCtx->submit(submitInfo);

        // 4. Signal inFlightFence once all prior submissions on renderQueue complete
        vkQueueSubmit(renderQueue, 0, nullptr, inFlightFence);

        // 5. Present the swapchain image once renderFinishedSem is signaled
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinishedSem;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapchain;
        presentInfo.pImageIndices = &imageIndex;

        vkQueuePresentKHR(renderQueue, &presentInfo);
    }
    void waitIdle() {
        if (device != VK_NULL_HANDLE) vkDeviceWaitIdle(device);
    }

    void shutdown() {
        if (device != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(device);
            destroySwapchainOnly();
            if (inFlightFence != VK_NULL_HANDLE) vkDestroyFence(device, inFlightFence, nullptr);
            if (renderFinishedSem != VK_NULL_HANDLE) vkDestroySemaphore(device, renderFinishedSem, nullptr);
            if (imageAvailableSem != VK_NULL_HANDLE) vkDestroySemaphore(device, imageAvailableSem, nullptr);
            if (surface != VK_NULL_HANDLE) vkDestroySurfaceKHR(instance, surface, nullptr);
            vkDestroyDevice(device, nullptr);
            device = VK_NULL_HANDLE;
        }
        if (instance != VK_NULL_HANDLE) {
            vkDestroyInstance(instance, nullptr);
            instance = VK_NULL_HANDLE;
        }
    }
} // namespace VK