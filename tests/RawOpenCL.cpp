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
#include <oclcrypto/CLError.h>

#include <boost/test/unit_test.hpp>

#include "TestSuiteFixture.h"

struct RawOpenCLFixture
{
    oclcrypto::System system;
};

BOOST_FIXTURE_TEST_SUITE(RawOpenCL, RawOpenCLFixture)

BOOST_AUTO_TEST_CASE(SimpleProgramCompilation)
{
    BOOST_REQUIRE_GT(system.getDeviceCount(), 0);

    for (size_t i = 0; i < system.getDeviceCount(); ++i)
    {
        oclcrypto::Device& device = system.getDevice(0);

        {
            // invalid syntax has to throw exception
            BOOST_CHECK_THROW(device.createProgram("invalid $ syntax"), oclcrypto::CLProgramCompilationError);
        }

        {
            // empty program should compile without exception
            oclcrypto::Program& empty = device.createProgram("");

            BOOST_CHECK_EQUAL(empty.getKernelCount(), 0);
            BOOST_CHECK_THROW(empty.getKernel("nonexistant"), oclcrypto::CLError);
            BOOST_CHECK_EQUAL(empty.getKernelCount(), 0);

            device.destroyProgram(empty);

            // destroying it twice should cause exception
            BOOST_CHECK_THROW(device.destroyProgram(empty), std::invalid_argument);
        }

        {
            // minimum one kernel program
            oclcrypto::Program& simple = device.createProgram(
                "__kernel void translate(__global float2* positions, __global float2* deltas)\n"
                "{\n"
                "    int gid = get_global_id(0);\n"
                "    positions[gid] += deltas[gid];\n"
                "}\n"
            );

            // nothing requested yet, so the expected kernel count is 0
            BOOST_CHECK_EQUAL(simple.getKernelCount(), 0);
            BOOST_CHECK_THROW(simple.getKernel("nonexistant"), oclcrypto::CLError);

            simple.getKernel("translate");
            // translate was requested, the kernel count should be 1 now
            BOOST_CHECK_EQUAL(simple.getKernelCount(), 1);

            // this should not create a new kernel, just fetch the existing one
            // so kernel count should stay at 1
            oclcrypto::Kernel& kernel = simple.getKernel("translate");
            BOOST_CHECK_EQUAL(simple.getKernelCount(), 1);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
