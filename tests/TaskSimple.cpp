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
