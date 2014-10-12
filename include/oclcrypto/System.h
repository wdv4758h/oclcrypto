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

#ifndef OCLCRYPTO_SYSTEM_H_
#define OCLCRYPTO_SYSTEM_H_

#include "oclcrypto/ForwardDecls.h"

// TODO: Hide CL dependency
#include <CL/cl.h>
#include <map>

namespace oclcrypto
{

/**
 * @brief Manages OpenCL devices found on the machine and their load
 *
 * Since it's very tricky to figure out the exact load of an OpenCL device,
 * we make these bold assumptions:
 *  - The devices are only used by oclcrypto
 *  - Each device has a certain "capacity" (we use lookup tables)
 *
 * OpenCL itself will queue the jobs and split them up if the driver deems
 * it reasonable. If he have multiple GPUs or other OpenCL devices we still
 * want to queue the jobs in the best possible queue.
 */
class OCLCRYPTO_EXPORT System
{
    public:
        /**
         * @brief Queries available OpenCL devices and initializes accordingly
         *
         * @param useCPUs Should CPU OpenCL devices also be used by oclcrypto
         */
        System(bool useCPUs = false);

        /**
         * @brief Frees resources allocated as part of initialization
         */
        ~System();

        /**
         * @brief Returns the amount of OpenCL devices available, including CPUs
         */
        size_t getDeviceCount() const;

        /**
         * @brief Allows you to iterate over available devices
         *
         * @param idx Index, at least 0 but lower than what getDeviceCount() returns
         * @note Consider using the higher level API instead of this function.
         * @note The order of devices is undefined and may change!
         */
        Device& getDevice(size_t idx);

        //DeviceAllocationPtr allocateDevice(unsigned int workload = 1);

        // noncopyable
        System(const System&) = delete;
        System& operator=(const System&) = delete;

    protected:
        //friend class DeviceAllocation;

        //void _notifyAllocation(Device* device, unsigned int workload);
        //void _notifyDeallocation(Device* device, unsigned int workload);

    private:
        void initializePlatform(cl_platform_id platform, bool useCPUs);

        typedef std::multimap<int, Device*> DeviceMap;
        DeviceMap mDevices;
};

}

#endif
