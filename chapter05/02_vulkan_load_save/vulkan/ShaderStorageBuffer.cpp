#include "ShaderStorageBuffer.h"

#include <VkBootstrap.h>

bool ShaderStorageBuffer::init(VkRenderData& renderData, VkShaderStorageBufferData &SSBOData, size_t bufferSize) {
  /* avoid errors with zero sized buffers */
  if (bufferSize == 0) {
    bufferSize = 1024;
  }

  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = bufferSize;
  bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

  VmaAllocationCreateInfo vmaAllocInfo{};
  vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

  VkResult result = vmaCreateBuffer(renderData.rdAllocator, &bufferInfo, &vmaAllocInfo,
    &SSBOData.buffer, &SSBOData.bufferAlloc, nullptr);
  if (result != VK_SUCCESS) {
    Logger::log(1, "%s error: could not allocate SSBO via VMA (error: %i)\n", __FUNCTION__, result);
    return false;
  }

  SSBOData.bufferSize = bufferSize;
  Logger::log(1, "%s: created SSBO of size %i\n", __FUNCTION__, bufferSize);
    return true;
}

bool ShaderStorageBuffer::checkForResize(VkRenderData& renderData, VkShaderStorageBufferData& SSBOData, size_t bufferSize) {
  if (bufferSize > SSBOData.bufferSize) {
    Logger::log(1, "%s: resize SSBO %p from %i to %i bytes\n", __FUNCTION__, SSBOData.buffer, SSBOData.bufferSize, bufferSize);
    cleanup(renderData, SSBOData);
    init(renderData, SSBOData, bufferSize);
    return true;
  }
  return false;
}

void ShaderStorageBuffer::cleanup(VkRenderData& renderData, VkShaderStorageBufferData &SSBOData) {
  VkResult result = vkQueueWaitIdle(renderData.rdGraphicsQueue);
  if (result != VK_SUCCESS) {
    Logger::log(1, "%s fatal error: could not wait for device idle (error: %i)\n", __FUNCTION__, result);
  }
  vmaDestroyBuffer(renderData.rdAllocator, SSBOData.buffer, SSBOData.bufferAlloc);
}
