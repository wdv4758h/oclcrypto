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

#ifndef OCLCRYPTO_TASK_H_
#define OCLCRYPTO_TASK_H_

#include "oclcrypto/ForwardDecls.h"

#include <memory>

namespace oclcrypto
{

enum TaskAlgorithm
{
    TA_PASSTHROUGH = 1
};

enum TaskState
{
    TS_SPECIFIED = 1,
    TS_RUNNING = 2,
    TS_ERROR = 3,
    TS_FINISHED = 4
};

/**
 * @brief
 */
class OCLCRYPTO_EXPORT Task
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
