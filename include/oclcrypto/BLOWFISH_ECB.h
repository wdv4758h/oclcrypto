/*
 * Copyright (C) 2015 Martin Preisler <martin@preisler.me>
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

#ifndef OCLCRYPTO_BLOWFISH_ECB_H_
#define OCLCRYPTO_BLOWFISH_ECB_H_

#include "oclcrypto/ForwardDecls.h"
#include "oclcrypto/BLOWFISH_Base.h"

namespace oclcrypto
{

/**
 * @brief Provides AES ECB encryption for 128, 192 and 256bit modes
 */
class OCLCRYPTO_EXPORT BLOWFISH_ECB_Encrypt : public BLOWFISH_Base
{
    public:
        /**
         * @brief AES_ECB_Encrypt
         *
         * @param system oclcrypto central class
         * @param device Which device will be doing the encryption
         */
        BLOWFISH_ECB_Encrypt(System& system, Device& device);
        ~BLOWFISH_ECB_Encrypt();

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
        DataBuffer* mPlainText;
        DataBuffer* mCipherText;
};

}

#endif
