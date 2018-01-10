/*
 * Copyright (c) 2017, Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "runtime/memory_manager/svm_memory_manager.h"
#include "driver_diagnostics_tests.h"
#include "unit_tests/helpers/debug_manager_state_restore.h"

using namespace OCLRT;

TEST_F(PerformanceHintEnqueueBufferTest, GivenBlockingReadWhenEnqueueReadBufferIsCallingWithCPUCopyThenContextProvidesProperHint) {

    buffer->forceDisallowCPUCopy = false;
    pCmdQ->enqueueReadBuffer(
        buffer,
        CL_TRUE,
        0,
        MemoryConstants::cacheLineSize,
        address,
        0,
        nullptr,
        nullptr);
    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[CL_ENQUEUE_READ_BUFFER_REQUIRES_COPY_DATA], static_cast<cl_mem>(buffer), address);
    EXPECT_TRUE(containsHint(expectedHint, userData));
}

TEST_P(PerformanceHintEnqueueReadBufferTest, GivenHostPtrAndSizeAlignmentsWhenEnqueueReadBufferIsCallingThenContextProvidesHintsAboutAlignments) {

    uintptr_t addressForReadBuffer = (uintptr_t)address;
    size_t sizeForReadBuffer = MemoryConstants::cacheLineSize;
    if (!alignedAddress) {
        addressForReadBuffer++;
    }
    if (!alignedSize) {
        sizeForReadBuffer--;
    }
    pCmdQ->enqueueReadBuffer(buffer, CL_FALSE,
                             0,
                             sizeForReadBuffer,
                             (void *)addressForReadBuffer,
                             0,
                             nullptr,
                             nullptr);
    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[CL_ENQUEUE_READ_BUFFER_REQUIRES_COPY_DATA], static_cast<cl_mem>(buffer), addressForReadBuffer);
    EXPECT_TRUE(containsHint(expectedHint, userData));
    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[CL_ENQUEUE_READ_BUFFER_DOESNT_MEET_ALIGNMENT_RESTRICTIONS], addressForReadBuffer, sizeForReadBuffer, MemoryConstants::pageSize, MemoryConstants::pageSize);
    EXPECT_EQ(!(alignedSize && alignedAddress), containsHint(expectedHint, userData));
}

TEST_P(PerformanceHintEnqueueReadBufferTest, GivenHostPtrAndSizeAlignmentsWhenEnqueueReadBufferRectIsCallingThenContextProvidesHintsAboutAlignments) {

    uintptr_t addressForReadBufferRect = (uintptr_t)address;
    size_t sizeForReadBufferRect = MemoryConstants::cacheLineSize;
    if (!alignedAddress) {
        addressForReadBufferRect++;
    }
    if (!alignedSize) {
        sizeForReadBufferRect--;
    }
    size_t bufferOrigin[] = {0, 0, 0};
    size_t hostOrigin[] = {0, 0, 0};
    size_t region[] = {sizeForReadBufferRect, 1, 1};

    pCmdQ->enqueueReadBufferRect(
        buffer,
        CL_TRUE,
        bufferOrigin,
        hostOrigin,
        region,
        0,
        0,
        0,
        0,
        (void *)addressForReadBufferRect,
        0,
        nullptr,
        nullptr);
    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[CL_ENQUEUE_READ_BUFFER_RECT_REQUIRES_COPY_DATA], static_cast<cl_mem>(buffer), addressForReadBufferRect);
    EXPECT_TRUE(containsHint(expectedHint, userData));
    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[CL_ENQUEUE_READ_BUFFER_RECT_DOESNT_MEET_ALIGNMENT_RESTRICTIONS], addressForReadBufferRect, sizeForReadBufferRect, MemoryConstants::pageSize, MemoryConstants::pageSize);
    EXPECT_EQ(!(alignedSize && alignedAddress), containsHint(expectedHint, userData));
}

TEST_F(PerformanceHintEnqueueBufferTest, GivenNonBlockingWriteWhenEnqueueWriteBufferIsCallingWithoutCPUCopyThenContextProvidesProperHint) {

    buffer->forceDisallowCPUCopy = true;
    pCmdQ->enqueueWriteBuffer(
        buffer,
        CL_FALSE,
        0,
        MemoryConstants::cacheLineSize,
        address,
        0,
        nullptr,
        nullptr);
    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[CL_ENQUEUE_WRITE_BUFFER_REQUIRES_COPY_DATA], static_cast<cl_mem>(buffer));
    EXPECT_TRUE(containsHint(expectedHint, userData));
}

TEST_F(PerformanceHintEnqueueBufferTest, GivenBlockingWriteWhenEnqueueWriteBufferIsCallingWithCPUCopyThenContextProvidesProperHint) {

    buffer->forceDisallowCPUCopy = false;
    pCmdQ->enqueueWriteBuffer(
        buffer,
        CL_TRUE,
        0,
        MemoryConstants::cacheLineSize,
        address,
        0,
        nullptr,
        nullptr);
    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[CL_ENQUEUE_WRITE_BUFFER_REQUIRES_COPY_DATA], static_cast<cl_mem>(buffer));
    EXPECT_TRUE(containsHint(expectedHint, userData));
}

TEST_F(PerformanceHintEnqueueBufferTest, GivenNonBlockingWriteWhenEnqueueWriteBufferRectIsCallingThenContextProvidesProperHint) {

    size_t bufferOrigin[] = {0, 0, 0};
    size_t hostOrigin[] = {0, 0, 0};
    size_t region[] = {1, 2, 1};

    pCmdQ->enqueueWriteBufferRect(
        buffer,
        CL_FALSE,
        bufferOrigin,
        hostOrigin,
        region,
        MemoryConstants::cacheLineSize,
        MemoryConstants::cacheLineSize,
        MemoryConstants::cacheLineSize,
        MemoryConstants::cacheLineSize,
        address,
        0,
        nullptr,
        nullptr);

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[CL_ENQUEUE_WRITE_BUFFER_RECT_REQUIRES_COPY_DATA], static_cast<cl_mem>(buffer));
    EXPECT_TRUE(containsHint(expectedHint, userData));
}

TEST_P(PerformanceHintEnqueueReadImageTest, GivenHostPtrAndSizeAlignmentsWhenEnqueueReadImageIsCallingThenContextProvidesHintsAboutAlignments) {

    size_t hostOrigin[] = {0, 0, 0};
    uintptr_t addressForReadImage = (uintptr_t)address;
    size_t sizeForReadImageInPixels = MemoryConstants::cacheLineSize;
    bool hintWithMisalignment = !(alignedAddress && alignedSize);
    if (!alignedAddress) {
        addressForReadImage++;
    }
    if (!alignedSize) {
        sizeForReadImageInPixels--;
    }
    size_t region[] = {sizeForReadImageInPixels, 1, 1};
    pCmdQ->enqueueReadImage(image,
                            CL_FALSE,
                            hostOrigin,
                            region,
                            0,
                            0,
                            (void *)addressForReadImage,
                            0,
                            nullptr,
                            nullptr);
    size_t sizeForReadImage = sizeForReadImageInPixels * image->getSurfaceFormatInfo().ImageElementSizeInBytes;
    ASSERT_EQ(alignedSize, isAligned<MemoryConstants::cacheLineSize>(sizeForReadImage));

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[CL_ENQUEUE_READ_IMAGE_DOESNT_MEET_ALIGNMENT_RESTRICTIONS], addressForReadImage, sizeForReadImage, MemoryConstants::pageSize, MemoryConstants::pageSize);
    EXPECT_EQ(hintWithMisalignment, containsHint(expectedHint, userData));
}

TEST_F(PerformanceHintEnqueueImageTest, GivenNonBlockingWriteWhenEnqueueWriteImageIsCallingThenContextProvidesProperHint) {

    size_t hostOrigin[] = {0, 0, 0};
    size_t region[] = {1, 1, 1};

    pCmdQ->enqueueWriteImage(
        image,
        CL_FALSE,
        hostOrigin,
        region,
        MemoryConstants::cacheLineSize,
        MemoryConstants::cacheLineSize,
        address,
        0,
        nullptr,
        nullptr);

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[CL_ENQUEUE_WRITE_IMAGE_REQUIRES_COPY_DATA], static_cast<cl_mem>(image));
    EXPECT_TRUE(containsHint(expectedHint, userData));
}

TEST_P(PerformanceHintEnqueueMapTest, GivenZeroCopyFlagWhenEnqueueMapBufferIsCallingThenContextProvidesProperHint) {

    Buffer *buffer;
    void *address;
    bool zeroCopyBuffer = GetParam();
    size_t sizeForBuffer = MemoryConstants::cacheLineSize;
    if (!zeroCopyBuffer) {
        sizeForBuffer++;
    }

    address = alignedMalloc(2 * MemoryConstants::cacheLineSize, MemoryConstants::cacheLineSize);
    buffer = Buffer::create(context, CL_MEM_USE_HOST_PTR, sizeForBuffer, address, retVal);

    pCmdQ->enqueueMapBuffer(buffer, CL_FALSE, 0, 0, MemoryConstants::cacheLineSize, 0, nullptr, nullptr, retVal);

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[CL_ENQUEUE_MAP_BUFFER_DOESNT_REQUIRE_COPY_DATA], static_cast<cl_mem>(buffer));
    EXPECT_EQ(zeroCopyBuffer, containsHint(expectedHint, userData));

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[CL_ENQUEUE_MAP_BUFFER_REQUIRES_COPY_DATA], static_cast<cl_mem>(buffer));
    EXPECT_EQ(!zeroCopyBuffer, containsHint(expectedHint, userData));

    alignedFree(address);
    delete buffer;
}

TEST_P(PerformanceHintEnqueueMapTest, GivenZeroCopyFlagWhenEnqueueMapImageIsCallingThenContextProvidesProperHint) {

    Image *image;
    bool zeroCopyImage;

    zeroCopyImage = GetParam();

    size_t origin[] = {0, 0, 0};
    size_t region[] = {1, 1, 1};

    if (zeroCopyImage) {
        image = ImageHelper<ImageReadOnly<Image1dDefaults>>::create(context);
    } else {
        image = ImageHelper<ImageUseHostPtr<Image1dDefaults>>::create(context);
    }
    EXPECT_EQ(zeroCopyImage, image->isMemObjZeroCopy());
    pCmdQ->enqueueMapImage(
        image,
        CL_FALSE,
        0,
        origin,
        region,
        nullptr,
        nullptr,
        0,
        nullptr,
        nullptr,
        retVal);

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[CL_ENQUEUE_MAP_IMAGE_DOESNT_REQUIRE_COPY_DATA], static_cast<cl_mem>(image));
    EXPECT_EQ(zeroCopyImage, containsHint(expectedHint, userData));

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[CL_ENQUEUE_MAP_IMAGE_REQUIRES_COPY_DATA], static_cast<cl_mem>(image));
    EXPECT_EQ(!zeroCopyImage, containsHint(expectedHint, userData));

    delete image;
}

TEST_P(PerformanceHintEnqueueMapTest, GivenZeroCopyFlagWhenEnqueueUnmapIsCallingWithBufferThenContextProvidesProperHint) {

    Buffer *buffer;
    void *address;
    bool zeroCopyBuffer = GetParam();
    size_t sizeForBuffer = MemoryConstants::cacheLineSize;
    if (!zeroCopyBuffer) {
        sizeForBuffer++;
    }

    address = alignedMalloc(2 * MemoryConstants::cacheLineSize, MemoryConstants::cacheLineSize);
    buffer = Buffer::create(context, CL_MEM_USE_HOST_PTR, sizeForBuffer, address, retVal);

    void *mapPtr = pCmdQ->enqueueMapBuffer(buffer, CL_FALSE, 0, 0, MemoryConstants::cacheLineSize, 0, nullptr, nullptr, retVal);
    pCmdQ->enqueueUnmapMemObject(buffer, mapPtr, 0, nullptr, nullptr);

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[CL_ENQUEUE_UNMAP_MEM_OBJ_REQUIRES_COPY_DATA], mapPtr, static_cast<cl_mem>(buffer));
    EXPECT_EQ(!zeroCopyBuffer, containsHint(expectedHint, userData));

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[CL_ENQUEUE_UNMAP_MEM_OBJ_DOESNT_REQUIRE_COPY_DATA], mapPtr);
    EXPECT_EQ(zeroCopyBuffer, containsHint(expectedHint, userData));

    alignedFree(address);
    delete buffer;
}

TEST_P(PerformanceHintEnqueueMapTest, GivenZeroCopyFlagWhenEnqueueUnmapIsCallingWithImageThenContextProvidesProperHint) {

    Image *image;
    bool zeroCopyImage;

    zeroCopyImage = GetParam();

    size_t origin[] = {0, 0, 0};
    size_t region[] = {1, 1, 1};

    if (zeroCopyImage) {
        image = ImageHelper<ImageReadOnly<Image1dDefaults>>::create(context);
    } else {
        image = ImageHelper<ImageUseHostPtr<Image1dDefaults>>::create(context);
    }
    EXPECT_EQ(zeroCopyImage, image->isMemObjZeroCopy());

    void *mapPtr = pCmdQ->enqueueMapImage(image, CL_FALSE, 0, origin, region, nullptr, nullptr, 0, nullptr, nullptr, retVal);

    pCmdQ->enqueueUnmapMemObject(image, mapPtr, 0, nullptr, nullptr);

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[CL_ENQUEUE_UNMAP_MEM_OBJ_REQUIRES_COPY_DATA], mapPtr, static_cast<cl_mem>(image));
    EXPECT_EQ(!zeroCopyImage, containsHint(expectedHint, userData));

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[CL_ENQUEUE_UNMAP_MEM_OBJ_DOESNT_REQUIRE_COPY_DATA], mapPtr);
    EXPECT_EQ(zeroCopyImage, containsHint(expectedHint, userData));

    delete image;
}

TEST_F(PerformanceHintEnqueueTest, GivenSVMPointerWhenEnqueueSVMMapIsCallingThenContextProvidesProperHint) {

    void *svmPtr = context->getSVMAllocsManager()->createSVMAlloc(256);

    pCmdQ->enqueueSVMMap(CL_FALSE, 0, svmPtr, 256, 0, nullptr, nullptr);

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[CL_ENQUEUE_SVM_MAP_DOESNT_REQUIRE_COPY_DATA], svmPtr);
    EXPECT_TRUE(containsHint(expectedHint, userData));

    context->getSVMAllocsManager()->freeSVMAlloc(svmPtr);
}

TEST_F(PerformanceHintEnqueueKernelTest, GivenNullLocalSizeAndEnableComputeWorkSizeNDIsDefaultWhenEnqueueKernelIsCallingThenContextProvidesProperHint) {

    retVal = pCmdQ->enqueueKernel(kernel, 1, nullptr, globalWorkGroupSize, nullptr, 0, nullptr, nullptr);
    EXPECT_EQ(CL_SUCCESS, retVal);

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[NULL_LOCAL_WORKGROUP_SIZE], kernel->getKernelInfo().name.c_str(),
             *kernel->localWorkSizeX, *kernel->localWorkSizeY, *kernel->localWorkSizeZ);
    EXPECT_TRUE(containsHint(expectedHint, userData));
}

TEST_F(PerformanceHintEnqueueKernelTest, GivenNullLocalSizeAndEnableComputeWorkSizeNDIsTrueWhenEnqueueKernelIsCallingThenContextProvidesProperHint) {

    bool isWorkGroupSizeEnabled = DebugManager.flags.EnableComputeWorkSizeND.get();
    DebugManager.flags.EnableComputeWorkSizeND.set(true);
    retVal = pCmdQ->enqueueKernel(kernel, 1, nullptr, globalWorkGroupSize, nullptr, 0, nullptr, nullptr);
    EXPECT_EQ(CL_SUCCESS, retVal);

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[NULL_LOCAL_WORKGROUP_SIZE], kernel->getKernelInfo().name.c_str(),
             *kernel->localWorkSizeX, *kernel->localWorkSizeY, *kernel->localWorkSizeZ);
    EXPECT_TRUE(containsHint(expectedHint, userData));
    DebugManager.flags.EnableComputeWorkSizeND.set(isWorkGroupSizeEnabled);
}

TEST_F(PerformanceHintEnqueueKernelTest, GivenNullLocalSizeAndEnableComputeWorkSizeNDIsFalseWhenEnqueueKernelIsCallingThenContextProvidesProperHint) {

    bool isWorkGroupSizeEnabled = DebugManager.flags.EnableComputeWorkSizeND.get();
    DebugManager.flags.EnableComputeWorkSizeND.set(false);
    retVal = pCmdQ->enqueueKernel(kernel, 1, nullptr, globalWorkGroupSize, nullptr, 0, nullptr, nullptr);
    EXPECT_EQ(CL_SUCCESS, retVal);

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[NULL_LOCAL_WORKGROUP_SIZE], kernel->getKernelInfo().name.c_str(),
             *kernel->localWorkSizeX, *kernel->localWorkSizeY, *kernel->localWorkSizeZ);
    EXPECT_TRUE(containsHint(expectedHint, userData));
    DebugManager.flags.EnableComputeWorkSizeND.set(isWorkGroupSizeEnabled);
}

TEST_F(PerformanceHintEnqueueKernelTest, GivenNullLocalSizeAndEnableComputeWorkSizeSquaredIsDefaultWhenEnqueueKernelIsCallingThenContextProvidesProperHint) {

    retVal = pCmdQ->enqueueKernel(kernel, 1, nullptr, globalWorkGroupSize, nullptr, 0, nullptr, nullptr);
    EXPECT_EQ(CL_SUCCESS, retVal);

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[NULL_LOCAL_WORKGROUP_SIZE], kernel->getKernelInfo().name.c_str(),
             *kernel->localWorkSizeX, *kernel->localWorkSizeY, *kernel->localWorkSizeZ);
    EXPECT_TRUE(containsHint(expectedHint, userData));
}

TEST_F(PerformanceHintEnqueueKernelTest, GivenNullLocalSizeAndEnableComputeWorkSizeSquaredIsTrueWhenEnqueueKernelIsCallingThenContextProvidesProperHint) {

    DebugManagerStateRestore dbgRestore;
    DebugManager.flags.EnableComputeWorkSizeSquared.set(true);
    DebugManager.flags.EnableComputeWorkSizeND.set(false);
    retVal = pCmdQ->enqueueKernel(kernel, 1, nullptr, globalWorkGroupSize, nullptr, 0, nullptr, nullptr);
    EXPECT_EQ(CL_SUCCESS, retVal);

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[NULL_LOCAL_WORKGROUP_SIZE], kernel->getKernelInfo().name.c_str(),
             *kernel->localWorkSizeX, *kernel->localWorkSizeY, *kernel->localWorkSizeZ);
    EXPECT_TRUE(containsHint(expectedHint, userData));
}

TEST_F(PerformanceHintEnqueueKernelTest, GivenNullLocalSizeAndEnableComputeWorkSizeSquaredIsFalseWhenEnqueueKernelIsCallingThenContextProvidesProperHint) {

    DebugManagerStateRestore dbgRestore;
    DebugManager.flags.EnableComputeWorkSizeSquared.set(false);
    DebugManager.flags.EnableComputeWorkSizeND.set(false);
    retVal = pCmdQ->enqueueKernel(kernel, 1, nullptr, globalWorkGroupSize, nullptr, 0, nullptr, nullptr);
    EXPECT_EQ(CL_SUCCESS, retVal);

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[NULL_LOCAL_WORKGROUP_SIZE], kernel->getKernelInfo().name.c_str(),
             *kernel->localWorkSizeX, *kernel->localWorkSizeY, *kernel->localWorkSizeZ);
    EXPECT_TRUE(containsHint(expectedHint, userData));
}

TEST_P(PerformanceHintEnqueueKernelBadSizeTest, GivenBadLocalWorkGroupSizeWhenEnqueueKernelIsCallingThenContextProvidesProperHint) {

    size_t preferredWorkGroupSize[3];
    size_t localWorkGroupSize[3];
    int badSizeDimension;
    uint32_t workDim = globalWorkGroupSize[1] == 1 ? 1 : globalWorkGroupSize[2] == 1 ? 2 : 3;
    auto maxWorkGroupSize = static_cast<uint32_t>(pPlatform->getDevice(0)->getDeviceInfo().maxWorkGroupSize);
    uint32_t simdSize = 32;
    if (DebugManager.flags.EnableComputeWorkSizeND.get()) {
        WorkSizeInfo wsInfo(maxWorkGroupSize, 0u, simdSize, 0u, IGFX_GEN9_CORE, 32u, 0u, false, false);
        computeWorkgroupSizeND(wsInfo, preferredWorkGroupSize, globalWorkGroupSize, workDim);
    } else if (DebugManager.flags.EnableComputeWorkSizeSquared.get() && workDim == 2) {
        computeWorkgroupSizeSquared(maxWorkGroupSize, preferredWorkGroupSize, globalWorkGroupSize, simdSize, workDim);
    } else
        computeWorkgroupSize2D(maxWorkGroupSize, preferredWorkGroupSize, globalWorkGroupSize, simdSize);
    for (auto i = 0; i < 3; i++) {
        localWorkGroupSize[i] = preferredWorkGroupSize[i];
    }

    badSizeDimension = GetParam();
    if (badSizeDimension == 0) {
        localWorkGroupSize[badSizeDimension] /= 2;
    } else {
        localWorkGroupSize[badSizeDimension] *= 2;
    }

    retVal = pCmdQ->enqueueKernel(kernel, 3, nullptr, globalWorkGroupSize, localWorkGroupSize, 0, nullptr, nullptr);
    EXPECT_EQ(CL_SUCCESS, retVal);

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[BAD_LOCAL_WORKGROUP_SIZE],
             localWorkGroupSize[0], localWorkGroupSize[1], localWorkGroupSize[2], kernel->getKernelInfo().name.c_str(),
             preferredWorkGroupSize[0], preferredWorkGroupSize[1], preferredWorkGroupSize[2]);
    EXPECT_TRUE(containsHint(expectedHint, userData));
}

TEST_F(PerformanceHintEnqueueKernelPrintfTest, GivenKernelWithPrintfWhenEnqueueKernelIsCalledWithWorkDim3ThenContextProvidesProperHint) {
    size_t preferredWorkGroupSize[3];
    auto maxWorkGroupSize = static_cast<uint32_t>(pPlatform->getDevice(0)->getDeviceInfo().maxWorkGroupSize);
    if (DebugManager.flags.EnableComputeWorkSizeND.get()) {
        WorkSizeInfo wsInfo(maxWorkGroupSize, 0u, 32u, 0u, IGFX_GEN9_CORE, 32u, 0u, false, false);
        computeWorkgroupSizeND(wsInfo, preferredWorkGroupSize, globalWorkGroupSize, 2);
    } else
        computeWorkgroupSize2D(maxWorkGroupSize, preferredWorkGroupSize, globalWorkGroupSize, 32);
    retVal = pCmdQ->enqueueKernel(kernel, 3, nullptr, globalWorkGroupSize, preferredWorkGroupSize, 0, nullptr, nullptr);
    EXPECT_EQ(CL_SUCCESS, retVal);

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[PRINTF_DETECTED_IN_KERNEL], kernel->getKernelInfo().name.c_str());
    EXPECT_TRUE(containsHint(expectedHint, userData));
}

TEST_F(PerformanceHintEnqueueTest, GivenKernelWithCoherentPtrWhenEnqueueKernelIsCalledWithWorkDim2ThenContextProvidesProperHint) {
    size_t preferredWorkGroupSize[3];
    size_t globalWorkGroupSize[3] = {1, 1, 1};
    auto maxWorkGroupSize = static_cast<uint32_t>(pPlatform->getDevice(0)->getDeviceInfo().maxWorkGroupSize);
    MockKernelWithInternals mockKernel(*pPlatform->getDevice(0), context);
    Kernel::SimpleKernelArgInfo kernelArgInfo;

    if (DebugManager.flags.EnableComputeWorkSizeND.get()) {
        WorkSizeInfo wsInfo(maxWorkGroupSize, 0u, 32u, 0u, IGFX_GEN9_CORE, 32u, 0u, false, false);
        computeWorkgroupSizeND(wsInfo, preferredWorkGroupSize, globalWorkGroupSize, 2);
    } else
        computeWorkgroupSize2D(maxWorkGroupSize, preferredWorkGroupSize, globalWorkGroupSize, 32);

    auto buffer = new MockBuffer();
    buffer->getGraphicsAllocation()->setCoherent(true);
    auto clBuffer = (cl_mem)buffer;

    kernelArgInfo.object = clBuffer;
    kernelArgInfo.type = Kernel::kernelArgType::BUFFER_OBJ;

    std::vector<Kernel::SimpleKernelArgInfo> kernelArguments;
    kernelArguments.resize(1);
    kernelArguments[0] = kernelArgInfo;
    mockKernel.kernelInfo.kernelArgInfo.resize(1);
    mockKernel.mockKernel->setKernelArguments(kernelArguments);

    retVal = pCmdQ->enqueueKernel(mockKernel.mockKernel, 2, nullptr, globalWorkGroupSize, preferredWorkGroupSize, 0, nullptr, nullptr);

    EXPECT_EQ(CL_SUCCESS, retVal);

    snprintf(expectedHint, DriverDiagnostics::maxHintStringSize, DriverDiagnostics::hintFormat[KERNEL_REQUIRES_COHERENCY], mockKernel.mockKernel->getKernelInfo().name.c_str());
    EXPECT_TRUE(containsHint(expectedHint, userData));
    delete buffer;
}

const int validDimensions[] = {0, 1, 2};

INSTANTIATE_TEST_CASE_P(
    DriverDiagnosticsTests,
    PerformanceHintEnqueueReadBufferTest,
    testing::Combine(
        ::testing::Bool(),
        ::testing::Bool()));

INSTANTIATE_TEST_CASE_P(
    DriverDiagnosticsTests,
    PerformanceHintEnqueueReadImageTest,
    testing::Combine(
        ::testing::Bool(),
        ::testing::Bool()));

INSTANTIATE_TEST_CASE_P(
    DriverDiagnosticsTests,
    PerformanceHintEnqueueMapTest,
    testing::Bool());

INSTANTIATE_TEST_CASE_P(
    DriverDiagnosticsTests,
    PerformanceHintEnqueueKernelBadSizeTest,
    testing::ValuesIn(validDimensions));