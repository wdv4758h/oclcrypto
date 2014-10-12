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
    RawOpenCLFixture():
        system(true)
    {}

    oclcrypto::System system;
};

const char* opencl_sample_code =
    "__kernel void square(__global int* input, __global int* output)\n"
    "{\n"
    "    int gid = get_global_id(0);\n"
    "    output[gid] = input[gid] * input[gid];\n"
    //"    printf(\"input[%i] is: %i\\n\", gid, input[gid]);\n"
    "}\n"
    "\n"
    "__kernel void set_to_constant(__global int* output, int c)\n"
    "{\n"
    "    int gid = get_global_id(0);\n"
    //"    printf(\"constant is: %i\\n\", c);\n"
    "    output[gid] = c;\n"
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
            // this crashes pocl
            //BOOST_CHECK_THROW(empty.createKernel("nonexistant"), oclcrypto::CLError);
            BOOST_CHECK_EQUAL(empty.getKernelCount(), 0);

            device.destroyProgram(empty);

            // destroying it twice should cause exception
            BOOST_CHECK_THROW(device.destroyProgram(empty), std::invalid_argument);
        }

        {
            // minimum one kernel program
            oclcrypto::Program& simple = device.createProgram(opencl_sample_code);

            // nothing requested yet, so the expected kernel count is 0
            BOOST_CHECK_EQUAL(simple.getKernelCount(), 0);
            // this crashes pocl
            //BOOST_CHECK_THROW(simple.createKernel("nonexistant"), oclcrypto::CLError);
            // kernel count still zero after error
            BOOST_CHECK_EQUAL(simple.getKernelCount(), 0);

            oclcrypto::Kernel& kernel = simple.createKernel("square");
            // translate was created, the kernel count should be 1 now
            BOOST_CHECK_EQUAL(simple.getKernelCount(), 1);

            {
                // another kernel of the same function
                oclcrypto::Kernel& kernel2 = simple.createKernel("square");
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

BOOST_AUTO_TEST_CASE(DataBuffers)
{
    BOOST_REQUIRE_GT(system.getDeviceCount(), 0);

    for (size_t i = 0; i < system.getDeviceCount(); ++i)
    {
        oclcrypto::Device& device = system.getDevice(i);
        oclcrypto::Program& program = device.createProgram(opencl_sample_code);

        oclcrypto::DataBuffer& output = device.allocateBuffer<int>(16, oclcrypto::DataBuffer::Write);
        BOOST_CHECK_EQUAL(output.getSize(), 16 * sizeof(int));
        BOOST_CHECK_EQUAL(output.getArraySize<int>(), 16);
        BOOST_CHECK_EQUAL(output.getLockState(), oclcrypto::DataBuffer::Unlocked);

        {
            auto data = output.lockWrite<int>();
            for (int j = 0; j < 16; ++j)
                data[j] = j;

            // out of bounds should throw
            BOOST_CHECK_THROW(data[16] = 123, std::out_of_range);

            BOOST_CHECK_EQUAL(output.getLockState(), oclcrypto::DataBuffer::WriteLocked);
            data.flush();
            BOOST_CHECK_EQUAL(output.getLockState(), oclcrypto::DataBuffer::Unlocked);
            // second flush should throw
            BOOST_CHECK_THROW(data.flush(), std::runtime_error);
        }
        {
            auto data = output.lockRead<int>();
            for (int j = 0; j < 16; ++j)
                BOOST_CHECK_EQUAL(data[j], j);

            BOOST_CHECK_THROW(data[17] == 17, std::out_of_range);

            BOOST_CHECK_EQUAL(output.getLockState(), oclcrypto::DataBuffer::ReadLocked);
            data.unlock();
            BOOST_CHECK_EQUAL(output.getLockState(), oclcrypto::DataBuffer::Unlocked);
            // second unlock should throw
            BOOST_CHECK_THROW(data.unlock(), std::runtime_error);
        }

        oclcrypto::DataBuffer& input = device.allocateBuffer<int>(16, oclcrypto::DataBuffer::Read);
        {
            auto data = input.lockWrite<int>();
            for (int j = 0; j < 16; ++j)
                data[j] = j;

            // it will get flushed automatically when going of of scope
            BOOST_CHECK_EQUAL(input.getLockState(), oclcrypto::DataBuffer::WriteLocked);
        }
        BOOST_CHECK_EQUAL(input.getLockState(), oclcrypto::DataBuffer::Unlocked);

        // the following write gets discarded
        {
            auto data = input.lockWrite<int>();
            for (int j = 0; j < 16; ++j)
                data[j] = j * 10;

            BOOST_CHECK_EQUAL(input.getLockState(), oclcrypto::DataBuffer::WriteLocked);
            data.discard();
            // second discard should throw
            BOOST_CHECK_THROW(data.discard(), std::runtime_error);
            BOOST_CHECK_EQUAL(input.getLockState(), oclcrypto::DataBuffer::Unlocked);
            // flushing after discard should throw
            BOOST_CHECK_THROW(data.flush(), std::runtime_error);
        }
    }
}

BOOST_AUTO_TEST_CASE(KernelExecution)
{
    BOOST_REQUIRE_GT(system.getDeviceCount(), 0);

    for (size_t i = 0; i < system.getDeviceCount(); ++i)
    {
        oclcrypto::Device& device = system.getDevice(i);
        oclcrypto::Program& program = device.createProgram(opencl_sample_code);

        {
            oclcrypto::DataBuffer& output = device.allocateBuffer<int>(16, oclcrypto::DataBuffer::Write);
            oclcrypto::Kernel& kernel = program.createKernel("set_to_constant");
            //kernel.setParameter(0, input);
            kernel.setParameter(0, output);
            int param = 1;
            kernel.setParameter(1, &param);

            kernel.execute(16, 8);

            {
                auto data = output.lockRead<int>();
                for (size_t j = 0; j < 16; ++j)
                    BOOST_CHECK_EQUAL(data[j], param);
            }

            // multiple executions of the same kernel have to work
            kernel.execute(16, 1);
            {
                auto data = output.lockRead<int>();
                for (size_t j = 0; j < 16; ++j)
                    BOOST_CHECK_EQUAL(data[j], param);
            }
        }
        {
            oclcrypto::DataBuffer& input = device.allocateBuffer<int>(16, oclcrypto::DataBuffer::Read);
            {
                auto data = input.lockWrite<int>();
                for (int j = 0; j < 16; ++j)
                    data[j] = j;
            }
            oclcrypto::DataBuffer& output = device.allocateBuffer<int>(16, oclcrypto::DataBuffer::Write);
            oclcrypto::Kernel& kernel = program.createKernel("square");
            kernel.setParameter(0, input);
            kernel.setParameter(1, output);

            kernel.execute(16, 8);

            {
                auto data = output.lockRead<int>();
                for (size_t j = 0; j < 16; ++j)
                    BOOST_CHECK_EQUAL(data[j], j * j);
            }

            // multiple executions of the same kernel have to work
            kernel.execute(16, 1);
            {
                auto data = output.lockRead<int>();
                for (size_t j = 0; j < 16; ++j)
                    BOOST_CHECK_EQUAL(data[j], j * j);
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
