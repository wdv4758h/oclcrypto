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

#include <oclcrypto/System.h>
#include <oclcrypto/Device.h>
#include <oclcrypto/DataBuffer.h>
#include <oclcrypto/CLError.h>

#include <boost/timer/timer.hpp>

#include "DataGenerator.h"

#include <iostream>

boost::timer::cpu_times time_OpenCL_DataBuffer_ToDevice(
    oclcrypto::System& system, oclcrypto::Device& device,
    size_t bufferSize, unsigned int iterations)
{
    const std::vector<unsigned char> buffer = generateRandomVector(bufferSize);
    oclcrypto::DataBuffer& dataBuffer = device.allocateBuffer<unsigned char>(bufferSize, oclcrypto::DataBuffer::Write);

    boost::timer::cpu_timer timer;

    for (size_t j = 0; j < iterations; ++j)
    {
        {
            auto lock = dataBuffer.lockWrite<unsigned char>();
            for (size_t i = 0; i < bufferSize; ++i)
                lock[i] = buffer[i];
        }

        // Just to make sure OpenCL isn't postponing the transfers
        CLErrorGuard(clFinish(device.getCLQueue()));
    }

    return timer.elapsed();
}

boost::timer::cpu_times time_OpenCL_DataBuffer_FromDevice(
    oclcrypto::System& system, oclcrypto::Device& device,
    size_t bufferSize, unsigned int iterations)
{
    const std::vector<unsigned char> buffer = generateRandomVector(bufferSize);

    oclcrypto::DataBuffer& dataBuffer = device.allocateBuffer<unsigned char>(bufferSize, oclcrypto::DataBuffer::ReadWrite);
    {
        auto lock = dataBuffer.lockWrite<unsigned char>();
        for (size_t i = 0; i < bufferSize; ++i)
            lock[i] = buffer[i];
    }

    unsigned char readBuffer[bufferSize];

    boost::timer::cpu_timer timer;

    for (size_t j = 0; j < iterations; ++j)
    {
        {
            auto lock = dataBuffer.lockRead<unsigned char>();
            for (size_t i = 0; i < bufferSize; ++i)
                readBuffer[i] = lock[i];
        }

        // Just to make sure OpenCL isn't postponing the transfers
        CLErrorGuard(clFinish(device.getCLQueue()));
    }

    return timer.elapsed();
}

void benchmark_OpenCL_DataBuffer(oclcrypto::System& system, size_t bufferSize)
{
    const unsigned int iterations = 20;

    std::cout << "OpenCL DataBuffer "  + std::to_string(bufferSize) + "-byte transfer to device" << std::endl;

    for (size_t i = 0; i < system.getDeviceCount(); ++i)
    {
        oclcrypto::Device& device = system.getDevice(i);
        const boost::timer::cpu_times times = time_OpenCL_DataBuffer_ToDevice(system, device, bufferSize, iterations);
        const size_t totalBytes = iterations * bufferSize;
        const double bytesPerSecond = (double)totalBytes / times.wall * 1000 * 1000 * 1000;

        std::cout << " - " << (bytesPerSecond * 0.001) << " KB/sec on " << device.getName() << std::endl;
    }

    std::cout << "OpenCL DataBuffer "  + std::to_string(bufferSize) + "-byte transfer from device" << std::endl;

    for (size_t i = 0; i < system.getDeviceCount(); ++i)
    {
        oclcrypto::Device& device = system.getDevice(i);
        const boost::timer::cpu_times times = time_OpenCL_DataBuffer_FromDevice(system, device, bufferSize, iterations);
        const size_t totalBytes = iterations * bufferSize;
        const double bytesPerSecond = (double)totalBytes / times.wall * 1000 * 1000 * 1000;

        std::cout << " - " << (bytesPerSecond * 0.001) << " KB/sec on " << device.getName() << std::endl;
    }
}

void OpenCL_DataBuffer_Benchmarks()
{
    oclcrypto::System system(true);

    for (unsigned short bufferSizeMul = 1; bufferSizeMul <= 128; bufferSizeMul *= 4)
    {
        const size_t bufferSize = 16 * 4096 * bufferSizeMul;

        benchmark_OpenCL_DataBuffer(system, bufferSize);
        std::cout << std::endl;
    }
}
