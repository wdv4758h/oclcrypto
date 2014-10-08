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

#ifndef OCLCRYPTO_PROGRAM_H_
#define OCLCRYPTO_PROGRAM_H_

#include "oclcrypto/ForwardDecls.h"
#include <CL/cl.h>

namespace oclcrypto
{

/**
 * @brief Represents one OpenCL program
 *
 * Program is created on one particular OpenCL device annd can't be shared
 * between devices.
 */
class Program
{
    public:
        /**
         * @param source Source code of the program in ASCII
         */
        Program(Device& device, const std::string& source);

        ~Program();

        /**
         * @brief Retrieves parent device of this program
         *
         * @note
         * Device can't change once the class is constructed, programs can't
         * be migrated to other devices.
         */
        Device& getDevice() const;

        /**
         * @brief Retrieves source code
         *
         * @note
         * Source code can't change once the class is constructed
         */
        const std::string& getSource() const;

        // noncopyable
        Program(const Program&) = delete;
        Program& operator=(const Program&) = delete;

    private:
        Device& mDevice;
        const std::string mSource;

        cl_program mProgram;
};

}

#endif
