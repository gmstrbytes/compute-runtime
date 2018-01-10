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

#pragma once
#include "runtime/memory_manager/os_agnostic_memory_manager.h"
#include "runtime/command_stream/command_stream_receiver_hw.h"
#include "runtime/memory_manager/os_agnostic_memory_manager.h"
#include <map>

namespace OCLRT {

template <typename GfxFamily>
class UltCommandStreamReceiver : public CommandStreamReceiverHw<GfxFamily> {
    using BaseClass = CommandStreamReceiverHw<GfxFamily>;

  public:
    using BaseClass::dshState;
    using BaseClass::hwInfo;
    using BaseClass::ihState;
    using BaseClass::iohState;
    using BaseClass::sshState;
    using BaseClass::CommandStreamReceiver::commandStream;
    using BaseClass::CommandStreamReceiver::disableL3Cache;
    using BaseClass::CommandStreamReceiver::dispatchMode;
    using BaseClass::CommandStreamReceiver::flushStamp;
    using BaseClass::CommandStreamReceiver::isPreambleSent;
    using BaseClass::CommandStreamReceiver::lastSentCoherencyRequest;
    using BaseClass::CommandStreamReceiver::lastSentL3Config;
    using BaseClass::CommandStreamReceiver::lastSentThreadAribtrationPolicy;
    using BaseClass::CommandStreamReceiver::latestSentStatelessMocsConfig;
    using BaseClass::CommandStreamReceiver::taskCount;
    using BaseClass::CommandStreamReceiver::taskLevel;

    UltCommandStreamReceiver(const UltCommandStreamReceiver &) = delete;
    UltCommandStreamReceiver &operator=(const UltCommandStreamReceiver &) = delete;

    static CommandStreamReceiver *create(const HardwareInfo &hwInfoIn) {
        return new UltCommandStreamReceiver<GfxFamily>(hwInfoIn);
    }

    UltCommandStreamReceiver(const HardwareInfo &hwInfoIn) : BaseClass(hwInfoIn) {
        tempTagLocation = new GraphicsAllocation(nullptr, 0);
        this->tagAllocation = tempTagLocation;
        this->tagAddress = reinterpret_cast<uint32_t *>(tempTagLocation->getUnderlyingBuffer());
    }

    virtual MemoryManager *createMemoryManager(bool enable64kbPages) override {
        memoryManager = new OsAgnosticMemoryManager(enable64kbPages);
        return memoryManager;
    }

    void overrideCsrSizeReqFlags(CsrSizeRequestFlags &flags) { this->csrSizeRequestFlags = flags; }

    virtual ~UltCommandStreamReceiver() override;
    GraphicsAllocation *getTagAllocation() { return tagAllocation; }
    GraphicsAllocation *getPreemptionCsrAllocation() {
        return this->preemptionCsrAllocation;
    }

  protected:
    using BaseClass::CommandStreamReceiver::cleanAllocationList;
    using BaseClass::CommandStreamReceiver::memoryManager;
    using BaseClass::CommandStreamReceiver::tagAddress;
    using BaseClass::CommandStreamReceiver::tagAllocation;

    GraphicsAllocation *tempTagLocation;
};

template <typename GfxFamily>
UltCommandStreamReceiver<GfxFamily>::~UltCommandStreamReceiver() {
    delete tempTagLocation;
}

} // namespace OCLRT