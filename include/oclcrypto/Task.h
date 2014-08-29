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

#ifndef OCLCRYPTO_TASK_H_
#define OCLCRYPTO_TASK_H_

#include "oclcrypto/ForwardDecls.h"

#include <memory>

namespace oclcrypto
{

enum TaskAlgorithm
{
    TA_PASSTHROUGH = 1
}

enum TaskState
{
    TS_SPECIFIED = 1,
    TS_RUNNING = 2,
    TS_ERROR = 3,
    TS_FINISHED = 4
}

/**
 * @brief
 */
class Task
{
    public:
        inline Task(Algorithm algorithm, ConstBufferPtr input, ConstBufferPtr secret, BufferPtr output):
            algorithm(algorithm),
            state(TS_SPECIFIED),

            input(input),
            secret(secret),
            output(output)
        {}

        const TaskAlgorithm algorithm;
        std::atomic<TaskState> state;

        const ConstBufferPtr input;
        const ConstBufferPtr secret;
        /// @note This can only be valid if task.state equals TS_FINISHED
        const BufferPtr output;
};

}

#endif
