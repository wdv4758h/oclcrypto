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

#include "oclcrypto/Program.h"
#include "oclcrypto/CLError.h"
#include "oclcrypto/Device.h"
#include "oclcrypto/Kernel.h"

#include <algorithm>

namespace oclcrypto
{

Program::Program(Device& device, const std::string& source):
    mDevice(device),
    mSource(source)
{
    {
        const char* src = source.c_str();
        const size_t length = source.length();

        cl_int err;
        mCLProgram = clCreateProgramWithSource(
            device.getCLContext(), 1,
            (const char** const)&src, &length,
            &err
        );

        CLErrorGuard(err);
    }

    const cl_device_id deviceId = device.getCLDeviceID();
    std::string options = std::string("-cl-strict-aliasing ") +
        (mDevice.getEndianess() == E_LITTLE_ENDIAN ? "-D LITTLE_ENDIAN" : "-D BIG_ENDIAN");
    const cl_int err = clBuildProgram(mCLProgram, 1, &deviceId, options.c_str(), nullptr, nullptr);

    if (err != CL_SUCCESS)
    {
        size_t size;
        clGetProgramBuildInfo(mCLProgram, deviceId, CL_PROGRAM_BUILD_LOG, 0, nullptr, &size);
        std::unique_ptr<char[]> buf(new char[size + 1]);
        clGetProgramBuildInfo(mCLProgram, deviceId, CL_PROGRAM_BUILD_LOG, size, buf.get(), nullptr);

        // OpenCL should append \0 but it never hurts to safe-guard ourselves
        buf[size] = '\0';

        CLProgramCompilationErrorThrow(err, std::string(buf.get()));
    }
}

Program::~Program()
{
    for (KernelVector::const_iterator it = mKernels.begin();
         it != mKernels.end(); ++it)
    {
        delete *it;
    }

    mKernels.clear();

    clReleaseProgram(mCLProgram);
}

Device& Program::getDevice() const
{
    return mDevice;
}

const std::string& Program::getSource() const
{
    return mSource;
}

Kernel& Program::createKernel(const std::string& name)
{
    Kernel* ret = new Kernel(*this, name);
    mKernels.push_back(ret);
    return *ret;
}

void Program::destroyKernel(Kernel& kernel)
{
    KernelVector::iterator it = std::find(mKernels.begin(), mKernels.end(), &kernel);

    if (it == mKernels.end())
        throw std::invalid_argument("Given kernel has already been destroyed or doesn't belong in this Program.");

    mKernels.erase(it);
    delete &kernel;
}

size_t Program::getKernelCount() const
{
    return mKernels.size();
}

cl_program Program::getCLProgram() const
{
    return mCLProgram;
}

ScopedProgram::ScopedProgram(Program& program):
    mProgram(program)
{}

ScopedProgram::~ScopedProgram()
{
    mProgram.getDevice().destroyProgram(mProgram);
}

}
