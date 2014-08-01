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
 *
 */

#ifndef OCLCRYPTO_CL_ERROR_H_
#define OCLCRYPTO_CL_ERROR_H_

#include "oclcrypto/ForwardDecls.h"

#include <CL/cl.h>
#include <exception>

namespace oclcrypto
{
    class CLError : public std::runtime_error
    {
        public:
            CLError(cl_int returnCode, const char* file, const unsigned int line, const char* func);

            const cl_int mReturnCode;
            const char* mFile;
            const unsigned int mLine;
            const char* mFunc;

            static const char* clErrorToString(cl_int error);
    };

#ifdef NDEBUG
#    define CLErrorGuard(call) _CLErrorGuard((call), nullptr, 0, nullptr)
#else
#    define CLErrorGuard(call) _CLErrorGuard((call), __FILE__, __LINE__, __func__)
#endif

inline void _CLErrorGuard(cl_int returnCode, const char* file, const unsigned int line, const char* func)
{
    if (returnCode == CL_SUCCESS)
        return;

    throw CLError(returnCode, file, line, func);
}

}

#endif
