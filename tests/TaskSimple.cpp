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

#include <boost/test/unit_test.hpp>
#include "TestSuiteFixture.h"

/*
void test()
{
    const oclcrypto::Buffer input("plainplainplain");
    const oclcrypto::Buffer secret("secret");

    oclcrypto::Buffer output;
    BOOST_REQUIRE_EQUAL(output.size(), 0);

    oclcrypto::Task task(oclcrypto::PASSTHROUGH, input, secret, output);
    BOOST_REQUIRE_EQUAL(task.status, TS_SPECIFIED);

    // useCPUs = true
    oclcrypto::System system(true);
    std::future<bool> scheduledTask = system.schedule(spec);

    // task status is std::atomic
    while (task.status == TS_SPECIFIED)
    {}

    BOOST_REQUIRE_EQUAL(task.status, TS_RUNNING);

    scheduleTask.wait();
    BOOST_REQUIRE_EQUAL(task.status, TS_FINISHED);
}
*/
