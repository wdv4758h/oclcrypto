/*
 * Copyright (C) 2014 Martin Preisler <martin@preisler.me>
 *
 * This file is part of oclcrypto.
 *
 * oclcrypto is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 */

#include "oclcrypto/DeviceManager.h"
#include "oclcrypto/CLError.h"
#include "oclcrypto/Device.h"

#include <vector>

namespace oclcrypto
{

void DeviceManager::initialize(bool useCPUs)
{
    if (msDevices.size() > 0)
    {
        // Already initialized, TODO: Warning?
        return;
    }

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

void DeviceManager::initializePlatform(cl_platform_id platform, bool useCPUs)
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
        msDevices.insert(std::make_pair(device->getCapacity(), device));
    }
}

}
