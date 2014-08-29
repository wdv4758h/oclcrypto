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

#ifndef OCLCRYPTO_DEVICE_H_
#define OCLCRYPTO_DEVICE_H_

#include "oclcrypto/ForwardDecls.h"
#include <CL/cl.h>

namespace oclcrypto
{

/**
 * @brief Represents one OpenCL device with a distinct cl_device_id
 */
class Device
{
    public:
        Device(cl_platform_id platformID, cl_device_id deviceID);
        ~Device();

        unsigned int getCapacity() const;

    private:
        cl_platform_id mPlatformID;
        cl_device_id mDeviceID;
        cl_context mContext;
        cl_command_queue mQueue;
};

}

#endif
