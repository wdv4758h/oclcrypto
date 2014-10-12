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

#include "oclcrypto/Kernel.h"
#include "oclcrypto/CLError.h"
#include "oclcrypto/Program.h"
#include "oclcrypto/DataBuffer.h"
#include "oclcrypto/Device.h"

namespace oclcrypto
{

Kernel::Kernel(Program& program, const std::string& name):
    mProgram(program),
    mName(name)
{
    cl_int err;
    mCLKernel = clCreateKernel(program.getCLProgram(), name.c_str(), &err);
    CLErrorGuard(err);
}

Kernel::~Kernel()
{
    try
    {
        CLErrorGuard(clReleaseKernel(mCLKernel));
    }
    catch (...)
    {
        // TODO: log
    }
}

Program& Kernel::getProgram() const
{
    return mProgram;
}

const std::string& Kernel::getName() const
{
    return mName;
}

void Kernel::setParameter(size_t idx, DataBuffer& buffer)
{
    CLErrorGuard(clSetKernelArg(mCLKernel, idx, sizeof(cl_mem), buffer.getCLMemPtr()));
}

void Kernel::setParameterPOD(size_t idx, size_t podSize, const void* pod)
{
    CLErrorGuard(clSetKernelArg(mCLKernel, idx, podSize, pod));
}

void Kernel::execute(size_t globalWorkSize, size_t localWorkSize, bool blockUntilComplete)
{
    const cl_command_queue queue = mProgram.getDevice().getCLQueue();

    CLErrorGuard(
        clEnqueueNDRangeKernel(
            queue, mCLKernel, 1, nullptr,
            &globalWorkSize, &localWorkSize, 0, nullptr, nullptr
        )
    );

    if (blockUntilComplete)
        CLErrorGuard(clFinish(queue));
}

}
