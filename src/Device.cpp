/*
 * Copyright (C) 2014 Martin Preisler <martin@preisler.me>
 *
 * This file is part of oclcrypto.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "oclcrypto/Device.h"
#include "oclcrypto/CLError.h"
#include "oclcrypto/Program.h"

#include <algorithm>

namespace oclcrypto
{

Device::Device(cl_platform_id platformID, cl_device_id deviceID):
    mCLPlatformID(platformID),
    mCLDeviceID(deviceID)
{
    cl_context_properties contextProperties[] = {
        CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(mCLPlatformID),
        0
    };

    cl_int err;

    mCLContext = clCreateContext(contextProperties, 1, &mCLDeviceID, nullptr, nullptr, &err);
    CLErrorGuard(err);
    // TODO: command_queue_properties might be useful for profiling
    mCLQueue = clCreateCommandQueue(mCLContext, mCLDeviceID, 0, &err);
    CLErrorGuard(err);
}

Device::~Device()
{
    for (ProgramVector::reverse_iterator it = mPrograms.rbegin();
         it != mPrograms.rend(); ++it)
    {
        // don't use destroyProgram here because that would change the vector needlessly
        delete *it;
    }
    mPrograms.clear();

    for (DataBufferVector::reverse_iterator it = mDataBuffers.rbegin();
         it != mDataBuffers.rend(); ++it)
    {
        // don't use deallocateBuffer here because that would change the vector needlessly
        delete *it;
    }
    mDataBuffers.clear();

    try
    {
        CLErrorGuard(clReleaseCommandQueue(mCLQueue));
        CLErrorGuard(clReleaseContext(mCLContext));

        // We shall not call clReleaseDevice here, clReleaseDevice releases subdevices,
        // not the main devices. Calling it here has strange effects on various OpenCL
        // implementations
    }
    catch (...) // can't throw in dtor
    {
        // TODO: log the exceptions
    }
}

Program& Device::createProgram(const std::string& source)
{
    Program* ret = new Program(*this, source);
    mPrograms.push_back(ret);
    return *ret;
}

void Device::destroyProgram(Program& program)
{
    ProgramVector::iterator it = std::find(mPrograms.begin(), mPrograms.end(), &program);

    if (it == mPrograms.end())
        throw std::invalid_argument(
            "Given Program has already been destroyed by this Device or "
            "it belongs to another Device."
        );

    mPrograms.erase(it);
    delete &program;
}

cl_device_id Device::getCLDeviceID() const
{
    return mCLDeviceID;
}

cl_context Device::getCLContext() const
{
    return mCLContext;
}

cl_command_queue Device::getCLQueue() const
{
    return mCLQueue;
}

DataBuffer& Device::allocateBufferRaw(const size_t size, const unsigned short memFlags)
{
    DataBuffer* ret = new DataBuffer(*this, size, memFlags);
    mDataBuffers.push_back(ret);
    return *ret;
}

void Device::deallocateBuffer(DataBuffer& buffer)
{
    DataBufferVector::iterator it = std::find(mDataBuffers.begin(), mDataBuffers.end(), &buffer);

    if (it == mDataBuffers.end())
        throw std::invalid_argument(
            "Given DataBuffer has already been destroyed by this Device or "
            "it belongs to another Device."
        );

    mDataBuffers.erase(it);
    delete &buffer;
}

unsigned int Device::getCapacity() const
{
    // TODO: This is completely arbitrary for now
    return 3;
}

}
