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

#ifndef OCLCRYPTO_AES_CTR_H_
#define OCLCRYPTO_AES_CTR_H_

#include "oclcrypto/ForwardDecls.h"
#include "oclcrypto/AES_Base.h"
#include <CL/cl.h>

namespace oclcrypto
{

/**
 * @brief Provides AES CTR encryption for 128, 192 and 256bit modes
 */
class OCLCRYPTO_EXPORT AES_CTR_Encrypt : public AES_Base
{
    public:
        /**
         * @brief AES_ECB_Encrypt
         *
         * @param system oclcrypto central class
         * @param device Which device will be doing the encryption
         */
        AES_CTR_Encrypt(System& system, Device& device);
        ~AES_CTR_Encrypt();

        /**
         * @note initial counter is also called 'nonce' in various materials
         */
        void setInitialCounter(const unsigned char ic[16]);

        void setPlainText(const unsigned char* plaintext, size_t size);

        inline void setPlainText(const char* plaintext, size_t size)
        {
            setPlainText(reinterpret_cast<const unsigned char*>(plaintext), size);
        }

        void execute(size_t localWorkSize);

        inline DataBuffer* getCipherText()
        {
            return mCipherText;
        }

    private:
        cl_uchar16 mIC;

        DataBuffer* mPlainText;
        DataBuffer* mCipherText;
};

}

#endif
