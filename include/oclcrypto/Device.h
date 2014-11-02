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
#include "oclcrypto/DataBuffer.h"

#include <CL/cl.h>
#include <string>

namespace oclcrypto
{

/**
 * @brief Represents one OpenCL device with a distinct cl_device_id
 */
class OCLCRYPTO_EXPORT Device
{
    public:
        Device(cl_platform_id platformID, cl_device_id deviceID);
        ~Device();

        std::string getName() const;

        Program& createProgram(const std::string& source);
        void destroyProgram(Program& program);

        cl_device_id getCLDeviceID() const;
        cl_context getCLContext() const;
        cl_command_queue getCLQueue() const;

        DataBuffer& allocateBufferRaw(const size_t size, const unsigned short memFlags = DataBuffer::ReadWrite);

        template<typename T>
        inline DataBuffer& allocateBuffer(const size_t count, const unsigned short memFlags = DataBuffer::ReadWrite)
        {
            return allocateBufferRaw(count * sizeof(T), memFlags);
        }

        void deallocateBuffer(DataBuffer& buffer);

        unsigned int getCapacity() const;

        // noncopyable
        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;

    private:
        cl_platform_id mCLPlatformID;
        cl_device_id mCLDeviceID;
        cl_context mCLContext;
        cl_command_queue mCLQueue;

        typedef std::vector<Program*> ProgramVector;
        ProgramVector mPrograms;

        typedef std::vector<DataBuffer*> DataBufferVector;
        DataBufferVector mDataBuffers;
};

}

#endif
