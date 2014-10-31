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

#include "oclcrypto/AES_ECB.h"
#include "oclcrypto/Device.h"
#include "oclcrypto/DataBuffer.h"
#include "oclcrypto/Program.h"
#include "oclcrypto/Kernel.h"
#include "oclcrypto/System.h"

#include <cassert>

namespace oclcrypto
{

const unsigned char AES_ECB_Encrypt::Sbox[256] =
{
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

const unsigned char AES_ECB_Encrypt::Rcon[256] =
{
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
    0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39,
    0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
    0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
    0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
    0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
    0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b,
    0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
    0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
    0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
    0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35,
    0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
    0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
    0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63,
    0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
    0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d
};

// key schedule code adapted from http://www.samiam.org/key-schedule.html

/* This is the core key expansion, which, given a 4-byte value,
 * does some scrambling */
static inline void expandKeyRounds_scheduleCore(unsigned char* in, unsigned char i)
{
    /* Rotate the input 8 bits to the left */
    const unsigned char first = in[0];
    for (size_t c = 0; c < 3; ++c)
        in[c] = in[c + 1];
    in[3] = first;

    /* Apply Rijndael's s-box on all 4 bytes */
    for (size_t c = 0; c < 4; ++c)
        in[c] = AES_ECB_Encrypt::Sbox[in[c]];

    /* On just the first byte, add 2^i to the byte */
    in[0] ^= AES_ECB_Encrypt::Rcon[i];
}

static inline void expandKeyRounds128(const unsigned char* key, unsigned char* output)
{
    for (size_t c = 0; c < 16; ++c)
        output[c] = key[c];

    unsigned char t[4];
    unsigned char i = 1;

    /* We need 11 sets of sixteen bytes each for 128-bit mode */
    /* c starts at 16 because the first sub-key is the user-supplied key */
    for (size_t c = 16; c < 11 * 16;)
    {
        /* Copy the temporary variable over */
        for (size_t a = 0; a < 4; ++a)
            t[a] = output[a + c - 4];

        /* Every four blocks (of four bytes), do a complex calculation */
        if (c % 16 == 0)
            expandKeyRounds_scheduleCore(t, i++);

        for (size_t a = 0; a < 4; ++a)
        {
            output[c] = output[c - 16] ^ t[a];
            c++;
        }
    }
}

static inline void expandKeyRounds192(const unsigned char* key, unsigned char* output)
{
    for (size_t c = 0; c < 24; ++c)
        output[c] = key[c];

    unsigned char t[4];
    unsigned char i = 1;

    /* We need 12 sets of sixteen bytes each for 192-bit mode */
    /* c starts at 24 because the first sub-key is the user-supplied key */
    for (size_t c = 24; c < 12 * 16;)
    {
        /* Copy the temporary variable over */
        for (size_t a = 0; a < 4; ++a)
            t[a] = output[a + c - 4];

        /* Every six blocks (of four bytes), do a complex calculation */
        if (c % 24 == 0)
            expandKeyRounds_scheduleCore(t, i++);

        for (size_t a = 0; a < 4; ++a)
        {
            output[c] = output[c - 24] ^ t[a];
            c++;
        }
    }
}

static inline void expandKeyRounds256(const unsigned char* key, unsigned char* output)
{
    for (size_t c = 0; c < 32; ++c)
        output[c] = key[c];

    unsigned char t[4];
    unsigned char i = 1;

    /* We need 15 sets of sixteen bytes each for 256-bit mode */
    /* c starts at 32 because the first sub-key is the user-supplied key */
    for (size_t c = 32; c < 15 * 16;)
    {
        /* Copy the temporary variable over */
        for (size_t a = 0; a < 4; ++a)
            t[a] = output[a + c - 4];

        /* Every eight blocks (of four bytes), do a complex calculation */
        if (c % 32 == 0)
            expandKeyRounds_scheduleCore(t, i++);

        /* For 256-bit keys, we add an extra sbox to the calculation */
        if (c % 32 == 16)
        {
            for (size_t a = 0; a < 4; ++a)
                t[a] = AES_ECB_Encrypt::Sbox[t[a]];
        }

        for (size_t a = 0; a < 4; ++a)
        {
            output[c] = output[c - 32] ^ t[a];
            c++;
        }
    }
}

unsigned char* AES_ECB_Encrypt::expandKeyRounds(const unsigned char* key, size_t keySize, size_t& rounds)
{
    // Size in bytes
    switch (keySize)
    {
        case 16:
            rounds = 11;
            break;
        case 24:
            rounds = 12;
            break;
        case 32:
            rounds = 15;
            break;

        default:
            throw std::invalid_argument("Invalid key size, has to be 16, 24 or 32.");
            break;
    }

    std::unique_ptr<unsigned char[]> ret(new unsigned char[rounds * 16]);

    switch (keySize)
    {
        case 16:
            expandKeyRounds128(key, ret.get());
            break;
        case 24:
            expandKeyRounds192(key, ret.get());
            break;
        case 32:
            expandKeyRounds256(key, ret.get());
            break;

        default:
            // we should never ever get here
            break;
    }

    return ret.release();
}

AES_ECB_Encrypt::AES_ECB_Encrypt(System& system, Device& device):
    mSystem(system),
    mDevice(device),

    mExpandedKey(nullptr),
    mPlainText(nullptr),
    mCipherText(nullptr)
{}

AES_ECB_Encrypt::~AES_ECB_Encrypt()
{
    try
    {
        if (mExpandedKey)
            mDevice.deallocateBuffer(*mExpandedKey);

        if (mPlainText)
            mDevice.deallocateBuffer(*mPlainText);

        if (mCipherText)
            mDevice.deallocateBuffer(*mCipherText);
    }
    catch (...)
    {
        // TODO: log?
    }
}

void AES_ECB_Encrypt::setKey(const unsigned char* key, size_t size)
{
    if (key == nullptr)
        throw std::invalid_argument("non-null key is required");

    if (!(size == 16 || size == 24 || size == 32))
        throw std::invalid_argument("Can't use given key of size " + std::to_string(size) + ". Make sure key size is 16, 24 or 32 (in bytes).");

    size_t rounds = 0;
    std::unique_ptr<unsigned char[]> expandedKey(expandKeyRounds(key, size, rounds));

    if (mExpandedKey)
    {
        mDevice.deallocateBuffer(*mExpandedKey);
        mExpandedKey = nullptr;
    }

    mExpandedKey = &mDevice.allocateBuffer<unsigned char>(rounds * 16, DataBuffer::Read);
    {
        auto data = mExpandedKey->lockWrite<unsigned char>();
        for (size_t i = 0; i < rounds * 16; ++i)
            data[i] = expandedKey[i];
    }
}

void AES_ECB_Encrypt::setPlainText(const unsigned char* plaintext, size_t size)
{
    if (plaintext == nullptr)
        throw std::invalid_argument("non-null plaintext is required");

    if (size == 0)
        throw std::invalid_argument("Make sure plain text size greater than 0");

    if (mPlainText)
    {
        mDevice.deallocateBuffer(*mPlainText);
        mPlainText = nullptr;
    }

    mPlainText = &mDevice.allocateBuffer<unsigned char>(size, DataBuffer::Read);
    {
        auto data = mExpandedKey->lockWrite<unsigned char>();
        for (size_t i = 0; i < size; ++i)
            data[i] = plaintext[i];
    }

    // we changed plain text, so let us invalidate ciphertext if any
    if (mCipherText)
    {
        mDevice.deallocateBuffer(*mCipherText);
        mCipherText = nullptr;
    }
}

void AES_ECB_Encrypt::execute(size_t localWorkSize)
{
    if (!mExpandedKey)
        throw std::runtime_error("Key has not been set.");

    if (!mPlainText)
        throw std::runtime_error("Plaintext has not been set.");

    if (mCipherText)
        throw std::runtime_error("Ciphertext buffer is not NULL, perhaps"
                                 "encryption has already been executed.");

    Program& program = mSystem.getProgramFromCache(mDevice, ProgramSources::AES);
    cl_uint rounds = 0;

    switch (mExpandedKey->getSize())
    {
        case 11 * 16: // 128bit mode
            rounds = 11;
            break;

        case 12 * 16: // 192bit mode
            rounds = 12;
            break;

        case 15 * 16: // 256bit mode
            rounds = 15;
            break;

        default:
            throw std::runtime_error("Unexpected expanded key size.");
    }

    const cl_uint plainTextSize = mPlainText->getArraySize<unsigned char>();
    assert(plainTextSize % 16 == 0); // temporary limitation
    const cl_uint cipherTextSize = plainTextSize;
    const cl_uint blockCount = plainTextSize / 16;

    mCipherText = &mDevice.allocateBuffer<unsigned char>(cipherTextSize, DataBuffer::Write);

    try
    {
        ScopedKernel kernel(program.createKernel("AES_ECB_Encrypt"));

        kernel->setParameter(0, *mPlainText);
        kernel->setParameter(1, *mExpandedKey);
        kernel->setParameter(2, *mCipherText);
        kernel->setParameter(3, &rounds);
        kernel->setParameter(4, &blockCount);

        kernel->execute(blockCount, localWorkSize);
    }
    catch (...)
    {
        mDevice.deallocateBuffer(*mCipherText);
        mCipherText = nullptr;
        throw;
    }
}

}
