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

#include "oclcrypto/DataBuffer.h"
#include "oclcrypto/Device.h"
#include "oclcrypto/CLError.h"

namespace oclcrypto
{

DataBuffer::DataBuffer(Device& device, const size_t size, const unsigned short memFlags):
    mDevice(device),
    mSize(size),
    mMemFlags(memFlags),

    mLockState(Unlocked)
{
    cl_mem_flags clMemFlags = 0;
    switch (memFlags)
    {
        case ReadWrite:
            clMemFlags = CL_MEM_READ_WRITE;
            break;
        case Read:
            clMemFlags = CL_MEM_READ_ONLY;
            break;
        case Write:
            clMemFlags = CL_MEM_WRITE_ONLY;
            break;

        default:
            throw std::invalid_argument("Invalid memory flags passed.");
            break;
    }

    cl_int err;
    mCLMem = clCreateBuffer(device.getCLContext(), clMemFlags, size, nullptr, &err);
    CLErrorGuard(err);
}

DataBuffer::~DataBuffer()
{
    try
    {
        CLErrorGuard(clReleaseMemObject(mCLMem));
    }
    catch (...)
    {
        // TODO: log
    }
}

Device& DataBuffer::getDevice() const
{
    return mDevice;
}

cl_mem DataBuffer::getCLMem() const
{
    return mCLMem;
}

const cl_mem* DataBuffer::getCLMemPtr() const
{
    return &mCLMem;
}

DataBuffer::LockState DataBuffer::getLockState() const
{
    return mLockState;
}

void DataBuffer::lockAndReadRawData(void* data)
{
    if (mLockState != Unlocked)
        throw std::runtime_error("This DataBuffer is already locked!");

    mLockState = ReadLocked;
    CLErrorGuard(clEnqueueReadBuffer(mDevice.getCLQueue(), mCLMem, CL_TRUE, 0, mSize, data, 0, nullptr, nullptr));
}

void DataBuffer::unlockAndWriteRawData(void* data, LockState expectedLockState)
{
    if (mLockState != expectedLockState)
        throw std::runtime_error("Lock states don't match!");

    // OpenCL API is responsible for freeing the data buffer
    CLErrorGuard(clEnqueueWriteBuffer(mDevice.getCLQueue(), mCLMem, CL_FALSE, 0, mSize, data, 0, nullptr, nullptr));
    // TODO: workaround for pocl, the following clFinish should not be required!
    CLErrorGuard(clFinish(mDevice.getCLQueue()));

    mLockState = Unlocked;
}

void DataBuffer::discardLock(LockState expectedLockState)
{
    if (mLockState != expectedLockState)
        throw std::runtime_error("Lock states don't match!");

    mLockState = Unlocked;
}

}
