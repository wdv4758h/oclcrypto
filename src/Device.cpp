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
 */

#include "oclcrypto/Device.h"
#include "oclcrypto/CLError.h"

namespace oclcrypto
{

Device::Device(cl_platform_id platformID, cl_device_id deviceID):
    mPlatformID(platformID),
    mDeviceID(deviceID)
{
    cl_context_properties contextProperties[] = {
        CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(mPlatformID),
        0
    };

    cl_int err;

    mContext = clCreateContext(contextProperties, 1, &mDeviceID, nullptr, nullptr, &err); CLErrorGuard(err);
    // TODO: command_queue_properties might be useful for profiling
    mQueue = clCreateCommandQueue(mContext, mDeviceID, 0, &err); CLErrorGuard(err);
}

Device::~Device()
{
    try
    {
        CLErrorGuard(clReleaseCommandQueue(mQueue));
        CLErrorGuard(clReleaseContext(mContext));
    }
    catch (...) // can't throw in dtor
    {
        // TODO: log the exceptions
    }
}

unsigned int Device::getCapacity() const
{
    // TODO: This is completely arbitrary for now
    return 3;
}

}
