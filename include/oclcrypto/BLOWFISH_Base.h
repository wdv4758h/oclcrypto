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

#ifndef OCLCRYPTO_BLOWFISH_BASE_H_
#define OCLCRYPTO_BLOWFISH_BASE_H_

#include "oclcrypto/ForwardDecls.h"
#include <cstdint>

namespace oclcrypto
{

class OCLCRYPTO_EXPORT BLOWFISH_Base
{
    public:
        /**
         * Initial values of the P array, derived from PI
         */
        static const uint32_t Init_P[18];

        /**
         * Initial values of the sboxes, derived from PI
         *
         * Reference implementation has 4 separate sboxes, we keep them in a single array
         * for easier handling.
         * sbox1 starts at index 0, sbox2 starts at index 256, ...
         */
        static const uint32_t Init_SBoxes[4 * 256];

        /**
         * @brief Generate the P and SBox[4] arrays from given BLOWFISH key
         *
         * You do not need to use this method directly unless you are testing the
         * implementation.
         *
         * @param key Input key
         * @param keySize Number of chars in the input key, valid values are 16, 24 and 32
         * @param p preallocated unsinged int[18] where the P will be stored
         * @param sboxes preallocated unsigned int[4*256] where the 4 SBoxes will be stored
         */
        static void generatePAndSBoxes(const unsigned char* key, size_t keySize, uint32_t* p, uint32_t* sboxes);

    protected:
        BLOWFISH_Base(System& system, Device& device);
        ~BLOWFISH_Base();

    public:
        /**
         * @brief setKey
         *
         * @param key buffer containing chars representing the key
         * @param size number of chars in the key, valid values are 16, 24 and 32
         */
        void setKey(const unsigned char* key, size_t size);

        inline void setKey(const char* key, size_t size)
        {
            setKey(reinterpret_cast<const unsigned char*>(key), size);
        }

    protected:
        System& mSystem;
        Device& mDevice;

        DataBuffer* mP;
        DataBuffer* mSBoxes;
};

}

#endif
