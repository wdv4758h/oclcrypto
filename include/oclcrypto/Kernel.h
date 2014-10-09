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

#ifndef OCLCRYPTO_KERNEL_H_
#define OCLCRYPTO_KERNEL_H_

#include "oclcrypto/ForwardDecls.h"
#include <CL/cl.h>

namespace oclcrypto
{

/**
 * @brief Represents a kernel from one OpenCL program
 *
 * A kernel is a function inside program source code and a set of parameters
 * used to run it. Since parameters are also paired with the kernel it does
 * make sense to create multiple kernels of the same function.
 */
class Kernel
{
    public:
        Kernel(Program& program, const std::string& name);

        ~Kernel();

        Program& getProgram() const;

        const std::string& getName() const;

        void setParameter(size_t idx, DataBuffer& buffer);
        template<typename T>
        void setParameter(size_t idx, const T& value)
        {
            static_assert(
                std::is_pod<T>::value,
                "Only plain old data can be passed as the argument"
            );

            setParameterPOD(idx, sizeof(T), reinterpret_cast<const void*>(&value));
        }

        // noncopyable
        Kernel(const Kernel&) = delete;
        Kernel& operator=(const Kernel&) = delete;

    private:
        void setParameterPOD(size_t idx, size_t podSize, const void* pod);

        Program& mProgram;
        const std::string mName;

        cl_kernel mCLKernel;
};

}

#endif
