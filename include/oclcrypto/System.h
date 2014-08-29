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
 *
 * This class is more of a namespace for functions than a real class.
 * It is a singleton in that sense.
 */
class System
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

        static DeviceAllocationPtr allocateDevice(unsigned int workload = 1);

    protected:
        friend class DeviceAllocation;

        void _notifyAllocation(Device* device, unsigned int workload);
        void _notifyDeallocation(Device* device, unsigned int workload);

    private:
        void initializePlatform(cl_platform_id platform, bool useCPUs);

        std::multimap<int, Device*> mDevices;
};

}

#endif
