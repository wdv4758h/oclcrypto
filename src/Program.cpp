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
        mProgram = clCreateProgramWithSource(
            device.getContext(), 1,
            (const char** const)&src, &length,
            &err
        );

        CLErrorGuard(err);
    }

    const cl_device_id deviceId = device.getDeviceID();
    const cl_int err = clBuildProgram(mProgram, 1, &deviceId, nullptr, nullptr, nullptr);

    if (err != CL_SUCCESS)
    {
        size_t size;
        clGetProgramBuildInfo(mProgram, device.getDeviceID(), CL_PROGRAM_BUILD_LOG, 0, nullptr, &size);
        std::unique_ptr<char[]> buf(new char[size]);
        clGetProgramBuildInfo(mProgram, device.getDeviceID(), CL_PROGRAM_BUILD_LOG, size, buf.get(), nullptr);

        // OpenCL should append \0 but it never hurts to safe-guard ourselves
        buf[size - 1] = '\0';

        CLProgramCompilationErrorThrow(err, std::string(buf.get()));
    }
}

Program::~Program()
{}

Device& Program::getDevice() const
{
    return mDevice;
}

const std::string& Program::getSource() const
{
    return mSource;
}

}
