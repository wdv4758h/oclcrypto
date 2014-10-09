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
#include <oclcrypto/Program.h>
#include <oclcrypto/Kernel.h>
#include <oclcrypto/CLError.h>

#include <boost/test/unit_test.hpp>

#include "TestSuiteFixture.h"

struct RawOpenCLFixture
{
    oclcrypto::System system;
};

const char* translate_opencl =
    "__kernel void translate(__global int* values, __global int* deltas, int pod)\n"
    "{\n"
    "    int gid = get_global_id(0);\n"
    "    values[gid] += deltas[gid] * pod;\n"
    "}\n";

BOOST_FIXTURE_TEST_SUITE(RawOpenCL, RawOpenCLFixture)

BOOST_AUTO_TEST_CASE(SimpleProgramCompilation)
{
    BOOST_REQUIRE_GT(system.getDeviceCount(), 0);

    for (size_t i = 0; i < system.getDeviceCount(); ++i)
    {
        oclcrypto::Device& device = system.getDevice(i);

        {
            // invalid syntax has to throw exception
            BOOST_CHECK_THROW(device.createProgram("invalid $ syntax"), oclcrypto::CLProgramCompilationError);
        }

        {
            // empty program should compile without exception
            oclcrypto::Program& empty = device.createProgram("");

            BOOST_CHECK_EQUAL(empty.getKernelCount(), 0);
            BOOST_CHECK_THROW(empty.createKernel("nonexistant"), oclcrypto::CLError);
            BOOST_CHECK_EQUAL(empty.getKernelCount(), 0);

            device.destroyProgram(empty);

            // destroying it twice should cause exception
            BOOST_CHECK_THROW(device.destroyProgram(empty), std::invalid_argument);
        }

        {
            // minimum one kernel program
            oclcrypto::Program& simple = device.createProgram(translate_opencl);

            // nothing requested yet, so the expected kernel count is 0
            BOOST_CHECK_EQUAL(simple.getKernelCount(), 0);
            BOOST_CHECK_THROW(simple.createKernel("nonexistant"), oclcrypto::CLError);
            // kernel count still zero after error
            BOOST_CHECK_EQUAL(simple.getKernelCount(), 0);

            oclcrypto::Kernel& kernel = simple.createKernel("translate");
            // translate was created, the kernel count should be 1 now
            BOOST_CHECK_EQUAL(simple.getKernelCount(), 1);

            {
                // another kernel of the same function
                oclcrypto::Kernel& kernel2 = simple.createKernel("translate");
                BOOST_CHECK_EQUAL(simple.getKernelCount(), 2);
                simple.destroyKernel(kernel2);
                BOOST_CHECK_EQUAL(simple.getKernelCount(), 1);
            }

            simple.destroyKernel(kernel);
            // destroying twice should throw
            BOOST_CHECK_THROW(simple.destroyKernel(kernel), std::invalid_argument);
        }
    }
}

BOOST_AUTO_TEST_CASE(KernelArguments)
{
    BOOST_REQUIRE_GT(system.getDeviceCount(), 0);

    for (size_t i = 0; i < system.getDeviceCount(); ++i)
    {
        oclcrypto::Device& device = system.getDevice(0);
        oclcrypto::Program& program = device.createProgram(translate_opencl);

        oclcrypto::DataBuffer& values = device.allocateBuffer<int>(16, oclcrypto::DataBuffer::ReadWrite);
        BOOST_CHECK_EQUAL(values.getSize(), 16 * sizeof(int));
        BOOST_CHECK_EQUAL(values.getArraySize<int>(), 16);
        BOOST_CHECK_EQUAL(values.getLockState(), oclcrypto::DataBuffer::Unlocked);

        {
            auto data = values.lockWrite<int>();
            for (int j = 0; j < 16; ++j)
                data[j] = j;

            // out of bounds should throw
            BOOST_CHECK_THROW(data[16] = 123, std::out_of_range);

            BOOST_CHECK_EQUAL(values.getLockState(), oclcrypto::DataBuffer::WriteLocked);
            data.flush();
            BOOST_CHECK_EQUAL(values.getLockState(), oclcrypto::DataBuffer::Unlocked);
            // second flush should throw
            BOOST_CHECK_THROW(data.flush(), std::runtime_error);
        }

        oclcrypto::DataBuffer& deltas = device.allocateBuffer<int>(16, oclcrypto::DataBuffer::Read);
        {
            auto data = deltas.lockWrite<int>();
            for (int j = 0; j < 16; ++j)
                data[j] = j;

            // it will get flushed automatically when going of of scope
            BOOST_CHECK_EQUAL(deltas.getLockState(), oclcrypto::DataBuffer::WriteLocked);
        }
        BOOST_CHECK_EQUAL(deltas.getLockState(), oclcrypto::DataBuffer::Unlocked);

        // the following write gets discarded
        {
            auto data = deltas.lockWrite<int>();
            for (int j = 0; j < 16; ++j)
                data[j] = j * 10;

            BOOST_CHECK_EQUAL(deltas.getLockState(), oclcrypto::DataBuffer::WriteLocked);
            data.discard();
            // second discard should throw
            BOOST_CHECK_THROW(data.discard(), std::runtime_error);
            BOOST_CHECK_EQUAL(deltas.getLockState(), oclcrypto::DataBuffer::Unlocked);
            // flushing after discard should throw
            BOOST_CHECK_THROW(data.flush(), std::runtime_error);
        }


        oclcrypto::Kernel& kernel = program.createKernel("translate");
        kernel.setParameter(0, values);
        kernel.setParameter(1, deltas);
        kernel.setParameter(2, 1);
        /*
        kernel.execute();

        {
            auto data = values.lockRead<int>();
            for (size_t j = 0; j < 16; ++j)
                BOOST_CHECK_EQUALS(data[j], 2.0f * j);
        }

        // multiple executions of the same kernel have to work
        kernel.execute();

        {
            auto data = values.lockRead<int>();
            for (size_t j = 0; j < 16; ++j)
                BOOST_CHECK_EQUALS(data[j], 3.0f * j);
        }*/
    }
}


BOOST_AUTO_TEST_SUITE_END()
