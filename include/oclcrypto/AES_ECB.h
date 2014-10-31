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

#ifndef OCLCRYPTO_AES_ECB_H_
#define OCLCRYPTO_AES_ECB_H_

#include "oclcrypto/ForwardDecls.h"

namespace oclcrypto
{

/**
 * @brief
 */
class OCLCRYPTO_EXPORT AES_ECB_Encrypt
{
    public:
        static const unsigned char Sbox[256];
        static const unsigned char Rcon[256];

        /**
         * @brief Expand given AES key into key rounds
         *
         * @param key Input key
         * @param keySize Number of chars in the input key, valid values are 16, 24 and 32
         * @param rounds Will be filled with the number of rounds expanded
         *
         * @note The user of this method is reponsible for freeing the output.
         */
        static unsigned char* expandKeyRounds(const unsigned char* key, size_t keySize, size_t& rounds);

        /**
         * @brief AES_ECB_Encrypt
         *
         * @param system oclcrypto central class
         * @param device Which device will be doing the encryption
         */
        AES_ECB_Encrypt(System& system, Device& device);

        ~AES_ECB_Encrypt();

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

        template<size_t size>
        inline void setKey(const unsigned char (&key)[size])
        {
            const size_t adjustedSize = key[size - 1] == 0x00 ? size - 1 : size;
            setKey(&key[0], adjustedSize);
        }

        template<size_t size>
        inline void setKey(const char (&key)[size])
        {
            const size_t adjustedSize = key[size - 1] == 0x00 ? size - 1 : size;
            setKey(reinterpret_cast<const unsigned char*>(&key[0]), adjustedSize);
        }

        void setPlainText(const unsigned char* plaintext, size_t size);

        inline void setPlainText(const char* plaintext, size_t size)
        {
            setPlainText(reinterpret_cast<const unsigned char*>(plaintext), size);
        }

        template<size_t size>
        inline void setPlainText(const unsigned char (&plaintext)[size])
        {
            const size_t adjustedSize = plaintext[size - 1] == 0x00 ? size - 1 : size;
            setPlainText(&plaintext[0], adjustedSize);
        }

        template<size_t size>
        inline void setPlainText(const char (&plaintext)[size])
        {
            const size_t adjustedSize = plaintext[size - 1] == 0x00 ? size - 1 : size;
            setPlainText(reinterpret_cast<const unsigned char*>(&plaintext[0]), adjustedSize);
        }

        void execute(size_t localWorkSize);

    private:
        System& mSystem;
        Device& mDevice;

        DataBuffer* mExpandedKey;
        DataBuffer* mPlainText;
        DataBuffer* mCipherText;
};

}

#endif
