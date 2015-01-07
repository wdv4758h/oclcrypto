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

#ifndef OCLCRYPTO_CL_ERROR_H_
#define OCLCRYPTO_CL_ERROR_H_

#include "oclcrypto/ForwardDecls.h"

#include <CL/cl.h>
#include <exception>

namespace oclcrypto
{

class OCLCRYPTO_EXPORT CLError : public std::runtime_error
{
    public:
        CLError(cl_int returnCode,
                const char* file, const unsigned int line, const char* func);

        const cl_int mReturnCode;

        const char* mFile;
        const unsigned int mLine;
        const char* mFunc;

        static const char* clErrorToString(cl_int error);
};

#ifdef NDEBUG
#    define CLErrorGuard(call) ::oclcrypto::_CLErrorGuard((call), nullptr, 0, nullptr)
#else
#    define CLErrorGuard(call) ::oclcrypto::_CLErrorGuard((call), __FILE__, __LINE__, "")
#endif

inline void _CLErrorGuard(cl_int returnCode, const char* file, const unsigned int line, const char* func)
{
    if (returnCode == CL_SUCCESS)
        return;

    throw CLError(returnCode, file, line, func);
}

class OCLCRYPTO_EXPORT CLProgramCompilationError : public std::runtime_error
{
    public:
        CLProgramCompilationError(cl_int returnCode, const std::string& log,
                                  const char* file, const unsigned int line, const char* func);

        const cl_int mReturnCode;

        const std::string mLog;
        const char* mFile;
        const unsigned int mLine;
        const char* mFunc;
};

#ifdef NDEBUG
#    define CLProgramCompilationErrorThrow(err, log) do { throw CLProgramCompilationError(err, log, __FILE__, __LINE__, ""); } while (false)
#else
#    define CLProgramCompilationErrorThrow(err, log) do { throw CLProgramCompilationError(err, log, __FILE__, __LINE__, ""); } while (false)
#endif

}

#endif
