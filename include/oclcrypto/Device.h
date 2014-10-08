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

        Program& createProgram(const std::string& source);
        void destroyProgram(Program& program);

        cl_device_id getDeviceID() const;
        cl_context getContext() const;

        unsigned int getCapacity() const;

        // noncopyable
        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;

    private:
        cl_platform_id mPlatformID;
        cl_device_id mDeviceID;
        cl_context mContext;
        cl_command_queue mQueue;

        typedef std::vector<Program*> ProgramVector;
        ProgramVector mPrograms;
};

}

#endif
