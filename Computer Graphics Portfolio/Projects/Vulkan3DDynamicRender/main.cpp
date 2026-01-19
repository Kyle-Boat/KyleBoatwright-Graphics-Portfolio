/*
	Vulkan renderer
	   References / learning sources:
    - 3D Graphics Rendering Cookbook (concepts)
    - Vulkan specification + Khronos documentation
    - Vulkan Samples (KhronosGroup)
    - Sascha Willems examples / articles (concepts + patterns)
    - Additional articles for specific topics (notes in README)
		
    * Hold mouse left button and move to rotate camera
		* Scroll to see alpha blending features
*/
#include <SFML/Graphics.hpp>
#include <vulkan/vulkan.h>
#define VOLK_IMPLEMENTATION
#include <volk/volk.h>
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <fstream>
#include <optional>
#include <cassert>
#include <cstdint>
#include <cstring>   
#include <cmath>
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "slang/slang.h"
#include "slang/slang-com-ptr.h"
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

constexpr uint32_t maxFramesInFlight{ 2 };
uint32_t imageIndex{ 0 };
uint32_t frameIndex{ 0 };
VkInstance instance{ VK_NULL_HANDLE };
VkDevice device{ VK_NULL_HANDLE };
VkQueue queue{ VK_NULL_HANDLE };
VkSurfaceKHR surface{ VK_NULL_HANDLE };

bool updateSwapchain{ false };
VkSwapchainKHR swapchain{ VK_NULL_HANDLE };

VkCommandPool commandPool{ VK_NULL_HANDLE };
VkPipeline pipeline{ VK_NULL_HANDLE };
VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };

VkImage depthImage{ VK_NULL_HANDLE };
VmaAllocator allocator{ VK_NULL_HANDLE };
VmaAllocation depthImageAllocation{ VK_NULL_HANDLE };
VkImageView depthImageView{ VK_NULL_HANDLE };

std::vector<VkImage> swapchainImages;
std::vector<VkImageView> swapchainImageViews;

std::array<VkCommandBuffer, maxFramesInFlight> commandBuffers;
std::array<VkFence, maxFramesInFlight> fences;
std::array<VkSemaphore, maxFramesInFlight> presentSemaphores;
std::vector<VkSemaphore> renderSemaphores;

VmaAllocation vBufferAllocation{ VK_NULL_HANDLE };
VkBuffer vBuffer{ VK_NULL_HANDLE };

VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
VkDescriptorPool descriptorPool{ VK_NULL_HANDLE };
std::array<VkDescriptorSet, maxFramesInFlight> descriptorSets{};

struct ShaderData {
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 model[3];
	glm::vec4 lightPos{ 0.0f, -10.0f, 10.0f, 0.0f };
	uint32_t selected{ 0 };
} shaderData{};

struct ShaderDataBuffer {
	VmaAllocation allocation{ VK_NULL_HANDLE };
	VkBuffer buffer{ VK_NULL_HANDLE };
	void* mapped{ nullptr };
};
std::array<ShaderDataBuffer, maxFramesInFlight> shaderDataBuffers;

Slang::ComPtr<slang::IGlobalSession> slangGlobalSession;

glm::vec3 camPos{ 0.0f, 0.0f, -6.0f };
glm::vec3 objectRotations[3]{};
sf::Vector2i lastMousePos{};

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
};

static inline void chk(VkResult result) {
	if (result != VK_SUCCESS) {
		std::cerr << "Vulkan Error: (" << result << ")\n";
		std::exit((int)result);
	}
}
static inline void chkSwapchain(VkResult result) {
	if (result < VK_SUCCESS) {
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			updateSwapchain = true;
			return;
		}
		std::cerr << "Vulkan Error: (" << result << ")\n";
		std::exit((int)result);
	}
}
static inline void chk(bool result) {
	if (!result) {
		std::cerr << "Call returned an error\n";
		std::exit(1);
	}
}

static bool loadGLBMesh(
	const char* path,
	std::vector<Vertex>& outVertices,
	std::vector<uint32_t>& outIndices)
{
	outVertices.clear();
	outIndices.clear();

	cgltf_options options{};
	cgltf_data* data = nullptr;

	cgltf_result res = cgltf_parse_file(&options, path, &data);
	if (res != cgltf_result_success || !data) {
		std::cerr << "cgltf_parse_file failed for: " << path << "\n";
		return false;
	}

	res = cgltf_load_buffers(&options, data, path);
	if (res != cgltf_result_success) {
		std::cerr << "cgltf_load_buffers failed for: " << path << "\n";
		cgltf_free(data);
		return false;
	}

	res = cgltf_validate(data);
	if (res != cgltf_result_success) {
		std::cerr << "cgltf_validate failed.\n";
	}

	const cgltf_mesh* mesh = nullptr;
	const cgltf_primitive* prim = nullptr;

	auto pickFromScene = [&]() -> bool {
		if (!data->scene) return false;
		for (cgltf_size ni = 0; ni < data->scene->nodes_count; ni++) {
			cgltf_node* node = data->scene->nodes[ni];
			if (node && node->mesh && node->mesh->primitives_count > 0) {
				mesh = node->mesh;
				prim = &node->mesh->primitives[0];
				return true;
			}
		}
		return false;
		};

	if (!pickFromScene()) {
		if (data->meshes_count > 0 && data->meshes[0].primitives_count > 0) {
			mesh = &data->meshes[0];
			prim = &data->meshes[0].primitives[0];
		}
	}

	if (!mesh || !prim) {
		std::cerr << "No mesh primitive found in GLB: " << path << "\n";
		cgltf_free(data);
		return false;
	}

	const cgltf_accessor* accPos = nullptr;
	const cgltf_accessor* accNrm = nullptr;
	const cgltf_accessor* accUV0 = nullptr;

	for (cgltf_size ai = 0; ai < prim->attributes_count; ai++) {
		const cgltf_attribute& attr = prim->attributes[ai];
		if (!attr.data) continue;

		if (attr.type == cgltf_attribute_type_position) accPos = attr.data;
		else if (attr.type == cgltf_attribute_type_normal) accNrm = attr.data;
		else if (attr.type == cgltf_attribute_type_texcoord && attr.index == 0) accUV0 = attr.data;
	}

	if (!accPos) {
		std::cerr << "GLB primitive has no position attribute.\n";
		cgltf_free(data);
		return false;
	}

	const cgltf_size vtxCount = accPos->count;
	outVertices.resize((size_t)vtxCount);

	for (cgltf_size i = 0; i < vtxCount; i++) {
		float p[3]{ 0,0,0 };
		float n[3]{ 0,0,1 };
		float uv[2]{ 0,0 };

		cgltf_accessor_read_float(accPos, i, p, 3);
		if (accNrm) cgltf_accessor_read_float(accNrm, i, n, 3);
		if (accUV0) cgltf_accessor_read_float(accUV0, i, uv, 2);

		Vertex v{};
		v.pos = glm::vec3(p[0], p[1], p[2]);
		v.normal = glm::normalize(glm::vec3(n[0], n[1], n[2]));
		v.uv = glm::vec2(uv[0], uv[1]);
		outVertices[(size_t)i] = v;
	}

	if (prim->indices) {
		const cgltf_accessor* accIdx = prim->indices;
		outIndices.reserve((size_t)accIdx->count);
		for (cgltf_size i = 0; i < accIdx->count; i++) {
			cgltf_size idx = cgltf_accessor_read_index(accIdx, i);
			outIndices.push_back((uint32_t)idx);
		}
	}
	else {
		outIndices.resize((size_t)vtxCount);
		for (uint32_t i = 0; i < (uint32_t)vtxCount; i++) outIndices[i] = i;
	}

	cgltf_free(data);
	return true;
}

int main(int argc, char* argv[])
{
	volkInitialize();

	VkApplicationInfo appInfo{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Kyle's Vulkan Project",
		.apiVersion = VK_API_VERSION_1_3
	};

	const std::vector<const char*> instanceExtensions{ sf::Vulkan::getGraphicsRequiredInstanceExtensions() };
	VkInstanceCreateInfo instanceCI{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo,
		.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size()),
		.ppEnabledExtensionNames = instanceExtensions.data(),
	};
	chk(vkCreateInstance(&instanceCI, nullptr, &instance));
	volkLoadInstance(instance);

	uint32_t deviceCount{ 0 };
	chk(vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr));
	std::vector<VkPhysicalDevice> devices(deviceCount);
	chk(vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()));

	uint32_t deviceIndex{ 0 };
	if (argc > 1) {
		deviceIndex = (uint32_t)std::stoi(argv[1]);
		assert(deviceIndex < deviceCount);
	}

	VkPhysicalDeviceProperties2 deviceProperties{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
	vkGetPhysicalDeviceProperties2(devices[deviceIndex], &deviceProperties);
	std::cout << "Device: " << deviceProperties.properties.deviceName << "\n";

	uint32_t queueFamilyCount{ 0 };
	vkGetPhysicalDeviceQueueFamilyProperties(devices[deviceIndex], &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(devices[deviceIndex], &queueFamilyCount, queueFamilies.data());

	uint32_t queueFamily{ 0 };
	for (size_t i = 0; i < queueFamilies.size(); i++) {
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			queueFamily = (uint32_t)i;
			break;
		}
	}

	const float qfpriorities{ 1.0f };
	VkDeviceQueueCreateInfo queueCI{
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = queueFamily,
		.queueCount = 1,
		.pQueuePriorities = &qfpriorities
	};

	VkPhysicalDeviceVulkan13Features enabledVk13Features{
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
		.synchronization2 = true,
		.dynamicRendering = true
	};

	const std::vector<const char*> deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	const VkPhysicalDeviceFeatures enabledVk10Features{ .samplerAnisotropy = VK_TRUE };

	VkDeviceCreateInfo deviceCI{
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = &enabledVk13Features,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &queueCI,
		.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
		.ppEnabledExtensionNames = deviceExtensions.data(),
		.pEnabledFeatures = &enabledVk10Features
	};
	chk(vkCreateDevice(devices[deviceIndex], &deviceCI, nullptr, &device));
	volkLoadDevice(device);
	vkGetDeviceQueue(device, queueFamily, 0, &queue);

	VmaVulkanFunctions vkFunctions{};
	vkFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
	vkFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

	VmaAllocatorCreateInfo allocatorCI{
		.physicalDevice = devices[deviceIndex],
		.device = device,
		.pVulkanFunctions = &vkFunctions,
		.instance = instance
	};
	chk(vmaCreateAllocator(&allocatorCI, &allocator));

	auto window = sf::RenderWindow(sf::VideoMode({ 1280, 720u }), "Kyle's Vulkan Project");
	chk(window.createVulkanSurface(instance, surface));

	VkSurfaceCapabilitiesKHR surfaceCaps{};
	chk(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(devices[deviceIndex], surface, &surfaceCaps));

	const VkFormat imageFormat{ VK_FORMAT_B8G8R8A8_SRGB };

	VkExtent2D extent = surfaceCaps.currentExtent;
	if (extent.width == 0xFFFFFFFFu) {
		extent.width = window.getSize().x;
		extent.height = window.getSize().y;
	}

	VkSwapchainCreateInfoKHR swapchainCI{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = surface,
		.minImageCount = surfaceCaps.minImageCount,
		.imageFormat = imageFormat,
		.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR,
		.imageExtent = extent,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = VK_PRESENT_MODE_FIFO_KHR
	};
	chk(vkCreateSwapchainKHR(device, &swapchainCI, nullptr, &swapchain));

	uint32_t imageCount{ 0 };
	chk(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr));
	swapchainImages.resize(imageCount);
	chk(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data()));

	swapchainImageViews.resize(imageCount);
	for (uint32_t i = 0; i < imageCount; i++) {
		VkImageViewCreateInfo viewCI{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = swapchainImages[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = imageFormat,
			.subresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1 }
		};
		chk(vkCreateImageView(device, &viewCI, nullptr, &swapchainImageViews[i]));
	}

	std::vector<VkFormat> depthFormatList{
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D32_SFLOAT_S8_UINT
	};

	VkFormat depthFormat{ VK_FORMAT_UNDEFINED };
	for (VkFormat& format : depthFormatList) {
		VkFormatProperties2 formatProperties{ .sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2 };
		vkGetPhysicalDeviceFormatProperties2(devices[deviceIndex], format, &formatProperties);
		if (formatProperties.formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
			depthFormat = format;
			break;
		}
	}
	assert(depthFormat != VK_FORMAT_UNDEFINED);

	VkImageAspectFlags depthAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	if (depthFormat == VK_FORMAT_D24_UNORM_S8_UINT || depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT) {
		depthAspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	VkImageCreateInfo depthImageCI{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = depthFormat,
		.extent{.width = window.getSize().x, .height = window.getSize().y, .depth = 1 },
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};

	VmaAllocationCreateInfo depthAllocCI{ .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT, .usage = VMA_MEMORY_USAGE_AUTO };
	chk(vmaCreateImage(allocator, &depthImageCI, &depthAllocCI, &depthImage, &depthImageAllocation, nullptr));

	VkImageViewCreateInfo depthViewCI{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = depthImage,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = depthFormat,
		.subresourceRange{.aspectMask = depthAspect, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1 }
	};
	chk(vkCreateImageView(device, &depthViewCI, nullptr, &depthImageView));

	VkSemaphoreCreateInfo semaphoreCI{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	VkFenceCreateInfo fenceCI{ .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .flags = VK_FENCE_CREATE_SIGNALED_BIT };
	for (uint32_t i = 0; i < maxFramesInFlight; i++) {
		chk(vkCreateFence(device, &fenceCI, nullptr, &fences[i]));
		chk(vkCreateSemaphore(device, &semaphoreCI, nullptr, &presentSemaphores[i]));
	}
	renderSemaphores.resize(swapchainImages.size());
	for (auto& semaphore : renderSemaphores) {
		chk(vkCreateSemaphore(device, &semaphoreCI, nullptr, &semaphore));
	}

	VkCommandPoolCreateInfo commandPoolCI{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = queueFamily
	};
	chk(vkCreateCommandPool(device, &commandPoolCI, nullptr, &commandPool));

	VkCommandBufferAllocateInfo cbAllocCI{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = commandPool,
		.commandBufferCount = maxFramesInFlight
	};
	chk(vkAllocateCommandBuffers(device, &cbAllocCI, commandBuffers.data()));

	const char* glbPath = "assets/ms_317.glb";
	if (argc > 2) glbPath = argv[2];

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	chk(loadGLBMesh(glbPath, vertices, indices));

	const VkDeviceSize indexCount = (VkDeviceSize)indices.size();
	const VkDeviceSize vBufSize = (VkDeviceSize)(sizeof(Vertex) * vertices.size());
	const VkDeviceSize iBufSize = (VkDeviceSize)(sizeof(uint32_t) * indices.size());

	VkBufferCreateInfo bufferCI{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = vBufSize + iBufSize,
		.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
	};
	VmaAllocationCreateInfo bufferAllocCI{
		.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
				 VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
				 VMA_ALLOCATION_CREATE_MAPPED_BIT,
		.usage = VMA_MEMORY_USAGE_AUTO
	};
	chk(vmaCreateBuffer(allocator, &bufferCI, &bufferAllocCI, &vBuffer, &vBufferAllocation, nullptr));

	void* bufferPtr{ nullptr };
	chk(vmaMapMemory(allocator, vBufferAllocation, &bufferPtr));
	std::memcpy(bufferPtr, vertices.data(), (size_t)vBufSize);
	std::memcpy(((char*)bufferPtr) + vBufSize, indices.data(), (size_t)iBufSize);
	vmaUnmapMemory(allocator, vBufferAllocation);

	for (uint32_t i = 0; i < maxFramesInFlight; i++) {
		VkBufferCreateInfo uBufferCI{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = sizeof(ShaderData),
			.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
		};
		VmaAllocationCreateInfo uBufferAllocCI{
			.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
					 VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
					 VMA_ALLOCATION_CREATE_MAPPED_BIT,
			.usage = VMA_MEMORY_USAGE_AUTO
		};
		chk(vmaCreateBuffer(allocator, &uBufferCI, &uBufferAllocCI,
			&shaderDataBuffers[i].buffer, &shaderDataBuffers[i].allocation, nullptr));
		chk(vmaMapMemory(allocator, shaderDataBuffers[i].allocation, &shaderDataBuffers[i].mapped));
	}

	{
		VkDescriptorSetLayoutBinding uboBinding{
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
		};

		VkDescriptorSetLayoutCreateInfo dslCI{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.bindingCount = 1,
			.pBindings = &uboBinding
		};
		chk(vkCreateDescriptorSetLayout(device, &dslCI, nullptr, &descriptorSetLayout));

		VkDescriptorPoolSize poolSize{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = maxFramesInFlight
		};

		VkDescriptorPoolCreateInfo dpCI{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = maxFramesInFlight,
			.poolSizeCount = 1,
			.pPoolSizes = &poolSize
		};
		chk(vkCreateDescriptorPool(device, &dpCI, nullptr, &descriptorPool));

		std::array<VkDescriptorSetLayout, maxFramesInFlight> layouts{};
		layouts.fill(descriptorSetLayout);

		VkDescriptorSetAllocateInfo allocInfo{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = descriptorPool,
			.descriptorSetCount = maxFramesInFlight,
			.pSetLayouts = layouts.data()
		};
		chk(vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()));

		for (uint32_t i = 0; i < maxFramesInFlight; i++) {
			VkDescriptorBufferInfo bufInfo{
				.buffer = shaderDataBuffers[i].buffer,
				.offset = 0,
				.range = sizeof(ShaderData)
			};

			VkWriteDescriptorSet write{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = descriptorSets[i],
				.dstBinding = 0,
				.descriptorCount = 1,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.pBufferInfo = &bufInfo
			};
			vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
		}
	}

	slang::createGlobalSession(slangGlobalSession.writeRef());
	auto slangTargets{ std::to_array<slang::TargetDesc>({ {.format{SLANG_SPIRV}, .profile{slangGlobalSession->findProfile("spirv_1_4")} } }) };
	auto slangOptions{ std::to_array<slang::CompilerOptionEntry>({ { slang::CompilerOptionName::EmitSpirvDirectly, {slang::CompilerOptionValueKind::Int, 1} } }) };
	slang::SessionDesc slangSessionDesc{
		.targets{slangTargets.data()},
		.targetCount{SlangInt(slangTargets.size())},
		.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR,
		.compilerOptionEntries{slangOptions.data()},
		.compilerOptionEntryCount{uint32_t(slangOptions.size())}
	};

	Slang::ComPtr<slang::ISession> slangSession;
	{
		SlangResult r = slangGlobalSession->createSession(slangSessionDesc, slangSession.writeRef());
		if (SLANG_FAILED(r) || !slangSession) {
			std::cerr << "Slang: createSession failed.\n";
			return EXIT_FAILURE;
		}
	}

	Slang::ComPtr<ISlangBlob> diagnostics;
	Slang::ComPtr<slang::IModule> slangModule;
	slangModule.attach(slangSession->loadModuleFromSource(
		"shaderModule",
		"shader.slang",
		nullptr,
		diagnostics.writeRef()));

	if (diagnostics && diagnostics->getBufferSize() > 0) {
		std::cerr << "Slang diagnostics:\n"
			<< (const char*)diagnostics->getBufferPointer() << "\n";
	}
	if (!slangModule) {
		std::cerr << "Slang failed to load/compile shader.slang\n";
		return EXIT_FAILURE;
	}

	Slang::ComPtr<ISlangBlob> spirv;
	{
		SlangResult r = slangModule->getTargetCode(0, spirv.writeRef());
		if (SLANG_FAILED(r) || !spirv) {
			std::cerr << "Failed to get SPIR-V from Slang module.\n";
			return EXIT_FAILURE;
		}
	}

	VkShaderModuleCreateInfo shaderModuleCI{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = spirv->getBufferSize(),
		.pCode = (uint32_t*)spirv->getBufferPointer()
	};
	VkShaderModule shaderModule{};
	chk(vkCreateShaderModule(device, &shaderModuleCI, nullptr, &shaderModule));

	VkPipelineLayoutCreateInfo pipelineLayoutCI{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 1,
		.pSetLayouts = &descriptorSetLayout,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = nullptr
	};
	chk(vkCreatePipelineLayout(device, &pipelineLayoutCI, nullptr, &pipelineLayout));

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages{
		{.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .stage = VK_SHADER_STAGE_VERTEX_BIT,   .module = shaderModule, .pName = "mainVS" },
		{.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, .stage = VK_SHADER_STAGE_FRAGMENT_BIT, .module = shaderModule, .pName = "mainFS" }
	};

	VkVertexInputBindingDescription vertexBinding{ .binding = 0, .stride = sizeof(Vertex), .inputRate = VK_VERTEX_INPUT_RATE_VERTEX };
	std::vector<VkVertexInputAttributeDescription> vertexAttributes{
		{.location = 0, .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT, .offset = offsetof(Vertex, pos) },
		{.location = 1, .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT, .offset = offsetof(Vertex, normal) },
		{.location = 2, .binding = 0, .format = VK_FORMAT_R32G32_SFLOAT,    .offset = offsetof(Vertex, uv) },
	};

	VkPipelineVertexInputStateCreateInfo vertexInputState{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &vertexBinding,
		.vertexAttributeDescriptionCount = (uint32_t)vertexAttributes.size(),
		.pVertexAttributeDescriptions = vertexAttributes.data(),
	};

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
	};

	std::vector<VkDynamicState> dynamicStates{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo dynamicState{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = (uint32_t)dynamicStates.size(),
		.pDynamicStates = dynamicStates.data()
	};

	VkPipelineViewportStateCreateInfo viewportState{ .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, .viewportCount = 1, .scissorCount = 1 };

	VkPipelineRasterizationStateCreateInfo rasterizationState{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.lineWidth = 1.0f
	};

	VkPipelineMultisampleStateCreateInfo multisampleState{ .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT };

	VkPipelineDepthStencilStateCreateInfo depthStencilState{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = VK_TRUE,
		.depthWriteEnable = VK_FALSE,
		.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL
	};

	VkPipelineColorBlendAttachmentState blendAttachment{
		.blendEnable = VK_TRUE,

		.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		.colorBlendOp = VK_BLEND_OP_ADD,

		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		.alphaBlendOp = VK_BLEND_OP_ADD,

		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
											VK_COLOR_COMPONENT_G_BIT |
											VK_COLOR_COMPONENT_B_BIT |
											VK_COLOR_COMPONENT_A_BIT
	};

	VkPipelineColorBlendStateCreateInfo colorBlendState{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &blendAttachment
	};

	VkPipelineRenderingCreateInfo renderingCI{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &imageFormat,
		.depthAttachmentFormat = depthFormat
	};

	VkGraphicsPipelineCreateInfo pipelineCI{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = &renderingCI,
		.stageCount = (uint32_t)shaderStages.size(),
		.pStages = shaderStages.data(),
		.pVertexInputState = &vertexInputState,
		.pInputAssemblyState = &inputAssemblyState,
		.pViewportState = &viewportState,
		.pRasterizationState = &rasterizationState,
		.pMultisampleState = &multisampleState,
		.pDepthStencilState = &depthStencilState,
		.pColorBlendState = &colorBlendState,
		.pDynamicState = &dynamicState,
		.layout = pipelineLayout
	};
	chk(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCI, nullptr, &pipeline));

	sf::Clock clock;
	while (window.isOpen()) {
		chk(vkWaitForFences(device, 1, &fences[frameIndex], true, UINT64_MAX));
		chk(vkResetFences(device, 1, &fences[frameIndex]));
		chkSwapchain(vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, presentSemaphores[frameIndex], VK_NULL_HANDLE, &imageIndex));

		shaderData.projection = glm::perspective(glm::radians(45.0f),
			(float)window.getSize().x / (float)window.getSize().y, 0.1f, 128.0f);
		shaderData.view = glm::translate(glm::mat4(1.0f), camPos);

		for (int i = 0; i < 3; i++) {
			glm::vec3 instancePos((float)(i) * 3.0f, 0.0f, 0.0f);
			shaderData.model[i] = glm::translate(glm::mat4(1.0f), instancePos) * glm::mat4_cast(glm::quat(objectRotations[i]));
		}
		std::memcpy(shaderDataBuffers[frameIndex].mapped, &shaderData, sizeof(ShaderData));

		VkCommandBuffer cb = commandBuffers[frameIndex];
		chk(vkResetCommandBuffer(cb, 0));

		VkCommandBufferBeginInfo cbBI{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT };
		chk(vkBeginCommandBuffer(cb, &cbBI));

		std::array<VkImageMemoryBarrier2, 2> outputBarriers{
			VkImageMemoryBarrier2{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
		.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = VK_ACCESS_2_NONE,
		.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,                 
		.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.image = swapchainImages[imageIndex],
		.subresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1 }
			},

			VkImageMemoryBarrier2{
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
				.srcStageMask = VK_PIPELINE_STAGE_2_NONE,
				.srcAccessMask = VK_ACCESS_2_NONE,
				.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
				.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				.image = depthImage,
				.subresourceRange{.aspectMask = depthAspect, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1 }
			}
		};

		VkDependencyInfo barrierDependencyInfo{
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.imageMemoryBarrierCount = (uint32_t)outputBarriers.size(),
			.pImageMemoryBarriers = outputBarriers.data()
		};
		vkCmdPipelineBarrier2(cb, &barrierDependencyInfo);

		VkRenderingAttachmentInfo colorAttachmentInfo{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = swapchainImageViews[imageIndex],
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue{.color{ 0.05f, 0.05f, 0.07f, 1.0f } }
		};

		VkRenderingAttachmentInfo depthAttachmentInfo{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = depthImageView,
			.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.clearValue{.depthStencil{ 1.0f, 0 } }
		};

		VkRenderingInfo renderingInfo{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.renderArea{.offset{0,0}, .extent{ window.getSize().x, window.getSize().y } },
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colorAttachmentInfo,
			.pDepthAttachment = &depthAttachmentInfo
		};

		vkCmdBeginRendering(cb, &renderingInfo);

		VkViewport vp{
			.x = 0.0f,
			.y = 0.0f,
			.width = (float)window.getSize().x,
			.height = (float)window.getSize().y,
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};
		vkCmdSetViewport(cb, 0, 1, &vp);

		VkRect2D scissor{ .offset{0,0}, .extent{ window.getSize().x, window.getSize().y } };
		vkCmdSetScissor(cb, 0, 1, &scissor);

		vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

		vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
			0, 1, &descriptorSets[frameIndex], 0, nullptr);

		VkDeviceSize vOffset{ 0 };
		vkCmdBindVertexBuffers(cb, 0, 1, &vBuffer, &vOffset);
		vkCmdBindIndexBuffer(cb, vBuffer, vBufSize, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(cb, (uint32_t)indexCount, 1, 0, 0, 0);

		vkCmdEndRendering(cb);

		VkImageMemoryBarrier2 barrierPresent{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_2_NONE,
			.dstAccessMask = VK_ACCESS_2_NONE,
			.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			.image = swapchainImages[imageIndex],
			.subresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1 }
		};
		VkDependencyInfo barrierPresentDependencyInfo{
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &barrierPresent
		};
		vkCmdPipelineBarrier2(cb, &barrierPresentDependencyInfo);

		chk(vkEndCommandBuffer(cb));

		VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo submitInfo{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &presentSemaphores[frameIndex],
			.pWaitDstStageMask = &waitStages,
			.commandBufferCount = 1,
			.pCommandBuffers = &cb,
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &renderSemaphores[imageIndex],
		};
		chk(vkQueueSubmit(queue, 1, &submitInfo, fences[frameIndex]));

		frameIndex = (frameIndex + 1) % maxFramesInFlight;

		VkPresentInfoKHR presentInfo{
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &renderSemaphores[imageIndex],
			.swapchainCount = 1,
			.pSwapchains = &swapchain,
			.pImageIndices = &imageIndex
		};
		chkSwapchain(vkQueuePresentKHR(queue, &presentInfo));

		sf::Time elapsed = clock.restart();
		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) window.close();

			if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
					auto delta = lastMousePos - mouseMoved->position;
					objectRotations[shaderData.selected].x += (float)delta.y * 0.0005f * (float)elapsed.asMilliseconds();
					objectRotations[shaderData.selected].y -= (float)delta.x * 0.0005f * (float)elapsed.asMilliseconds();
				}
				lastMousePos = mouseMoved->position;
			}
			if (const auto* mouseWheelScrolled = event->getIf<sf::Event::MouseWheelScrolled>()) {
				camPos.z += (float)mouseWheelScrolled->delta * 0.025f * (float)elapsed.asMilliseconds();
			}
			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
				if (keyPressed->code == sf::Keyboard::Key::Add) {
					shaderData.selected = (shaderData.selected < 2) ? shaderData.selected + 1 : 0;
				}
				if (keyPressed->code == sf::Keyboard::Key::Subtract) {
					shaderData.selected = (shaderData.selected > 0) ? shaderData.selected - 1 : 2;
				}
			}
			if (event->getIf<sf::Event::Resized>()) {
				updateSwapchain = true;
			}
		}

		if (updateSwapchain) {
			updateSwapchain = false;
			chk(vkDeviceWaitIdle(device));
			chk(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(devices[deviceIndex], surface, &surfaceCaps));

			VkExtent2D newExtent = surfaceCaps.currentExtent;
			if (newExtent.width == 0xFFFFFFFFu) {
				newExtent.width = window.getSize().x;
				newExtent.height = window.getSize().y;
			}

			VkSwapchainKHR oldSwapchain = swapchain;
			swapchainCI.oldSwapchain = oldSwapchain;
			swapchainCI.imageExtent = newExtent;

			chk(vkCreateSwapchainKHR(device, &swapchainCI, nullptr, &swapchain));

			for (auto& iv : swapchainImageViews) vkDestroyImageView(device, iv, nullptr);

			chk(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr));
			swapchainImages.resize(imageCount);
			chk(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data()));

			swapchainImageViews.resize(imageCount);
			for (uint32_t i = 0; i < imageCount; i++) {
				VkImageViewCreateInfo viewCI{
					.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
					.image = swapchainImages[i],
					.viewType = VK_IMAGE_VIEW_TYPE_2D,
					.format = imageFormat,
					.subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1 }
				};
				chk(vkCreateImageView(device, &viewCI, nullptr, &swapchainImageViews[i]));
			}

			vkDestroySwapchainKHR(device, oldSwapchain, nullptr);

			vmaDestroyImage(allocator, depthImage, depthImageAllocation);
			vkDestroyImageView(device, depthImageView, nullptr);

			depthImageCI.extent = { window.getSize().x, window.getSize().y, 1 };
			VmaAllocationCreateInfo allocCI2{ .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT, .usage = VMA_MEMORY_USAGE_AUTO };
			chk(vmaCreateImage(allocator, &depthImageCI, &allocCI2, &depthImage, &depthImageAllocation, nullptr));

			VkImageViewCreateInfo viewCI2{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = depthImage,
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = depthFormat,
				.subresourceRange{.aspectMask = depthAspect, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1 }
			};
			chk(vkCreateImageView(device, &viewCI2, nullptr, &depthImageView));
		}
	}

	chk(vkDeviceWaitIdle(device));

	for (uint32_t i = 0; i < maxFramesInFlight; i++) {
		vkDestroyFence(device, fences[i], nullptr);
		vkDestroySemaphore(device, presentSemaphores[i], nullptr);
		vmaUnmapMemory(allocator, shaderDataBuffers[i].allocation);
		vmaDestroyBuffer(allocator, shaderDataBuffers[i].buffer, shaderDataBuffers[i].allocation);
	}

	for (auto s : renderSemaphores) vkDestroySemaphore(device, s, nullptr);

	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

	vmaDestroyImage(allocator, depthImage, depthImageAllocation);
	vkDestroyImageView(device, depthImageView, nullptr);

	for (auto iv : swapchainImageViews) vkDestroyImageView(device, iv, nullptr);

	vmaDestroyBuffer(allocator, vBuffer, vBufferAllocation);

	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyPipeline(device, pipeline, nullptr);
	vkDestroySwapchainKHR(device, swapchain, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyCommandPool(device, commandPool, nullptr);
	vkDestroyShaderModule(device, shaderModule, nullptr);

	vmaDestroyAllocator(allocator);
	vkDestroyDevice(device, nullptr);
	vkDestroyInstance(instance, nullptr);

	return 0;
}
