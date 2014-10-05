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

#include "oclcrypto/System.h"
#include "oclcrypto/CLError.h"
#include "oclcrypto/Device.h"

#include <vector>

namespace oclcrypto
{

System::System(bool useCPUs)
{
    cl_uint platformCount = 0;
    CLErrorGuard(clGetPlatformIDs(0, nullptr, &platformCount));

    typedef std::vector<cl_platform_id> CLPlatformVector;
    CLPlatformVector platforms(platformCount);
    CLErrorGuard(clGetPlatformIDs(platformCount, platforms.data(), NULL));

    for (CLPlatformVector::const_iterator it = platforms.begin();
         it != platforms.end(); ++it)
    {
        initializePlatform(*it, useCPUs);
    }
}

System::~System()
{}

void System::initializePlatform(cl_platform_id platform, bool useCPUs)
{
    cl_device_type deviceType = useCPUs ?
        CL_DEVICE_TYPE_ALL : CL_DEVICE_TYPE_ALL & ~CL_DEVICE_TYPE_CPU;

    cl_uint deviceCount = 0;
    CLErrorGuard(clGetDeviceIDs(platform, deviceType, 0, nullptr, &deviceCount));

    typedef std::vector<cl_device_id> CLDeviceVector;
    CLDeviceVector devices(deviceCount);

    CLErrorGuard(clGetDeviceIDs(platform, deviceType, deviceCount, devices.data(), &deviceCount));

    for (CLDeviceVector::const_iterator it = devices.begin();
         it != devices.end(); ++it)
    {
        Device* device = new Device(platform, *it);
        mDevices.insert(std::make_pair(device->getCapacity(), device));
    }
}

}
