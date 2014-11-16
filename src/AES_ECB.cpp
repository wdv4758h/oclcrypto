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
#include <string>

namespace oclcrypto
{

AES_ECB_Encrypt::AES_ECB_Encrypt(System& system, Device& device):
    AES_Base(system, device),

    mPlainText(nullptr),
    mCipherText(nullptr)
{}

AES_ECB_Encrypt::~AES_ECB_Encrypt()
{
    try
    {
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

void AES_ECB_Encrypt::setPlainText(const unsigned char* plaintext, size_t size)
{
    if (plaintext == nullptr)
        throw std::invalid_argument("Non-null plaintext is required");

    if (size == 0)
        throw std::invalid_argument("Make sure plaintext size greater than 0");

    if (size % 16 != 0)
        throw std::invalid_argument("Plaintext has to be padded to make full AES blocks. "
                                    "Its size has to be a multiple of 16.");

    if (!mPlainText || mPlainText->getArraySize<unsigned char>() != size)
    {
        if (mPlainText)
            mDevice.deallocateBuffer(*mPlainText);

        mPlainText = &mDevice.allocateBuffer<unsigned char>(size, DataBuffer::Read);
    }

    {
        auto data = mPlainText->lockWrite<unsigned char>();
        for (size_t i = 0; i < size; ++i)
            data[i] = plaintext[i];
    }

    if (!mCipherText || mCipherText->getArraySize<unsigned char>() != size)
    {
        if (mCipherText)
            mDevice.deallocateBuffer(*mCipherText);

        mCipherText = &mDevice.allocateBuffer<unsigned char>(size, DataBuffer::Write);
    }
}

void AES_ECB_Encrypt::execute(size_t localWorkSize)
{
    if (!mExpandedKey)
        throw std::runtime_error("Key has not been set.");

    if (!mPlainText)
        throw std::runtime_error("Plaintext has not been set.");

    if (!mCipherText)
        throw std::runtime_error("CipherText buffer has not been allocated! This is most likely a bug.");

    Program& program = mSystem.getProgramFromCache(mDevice, ProgramSources::AES);
    cl_uint rounds = 0;

    switch (mExpandedKey->getSize())
    {
        case 11 * 16: // 128bit mode
            rounds = 11;
            break;

        case 13 * 16: // 192bit mode
            rounds = 13;
            break;

        case 15 * 16: // 256bit mode
            rounds = 15;
            break;

        default:
            throw std::runtime_error("Unexpected expanded key size.");
    }

    const cl_uint plainTextSize = mPlainText->getArraySize<unsigned char>();
    assert(plainTextSize % 16 == 0);
    const cl_uint blockCount = plainTextSize / 16;

    ScopedKernel kernel(program.createKernel("AES_ECB_Encrypt"));

    kernel->setParameter(0, *mPlainText);
    kernel->setParameter(1, *mExpandedKey);
    kernel->setParameter(2, *mCipherText);
    kernel->setParameter(3, &rounds);
    kernel->allocateLocalParameter<cl_uchar16>(4, localWorkSize);

    kernel->execute(blockCount, localWorkSize, false);
}

AES_ECB_Decrypt::AES_ECB_Decrypt(System& system, Device& device):
    AES_Base(system, device),

    mCipherText(nullptr),
    mPlainText(nullptr)
{}

AES_ECB_Decrypt::~AES_ECB_Decrypt()
{
    try
    {
        if (mCipherText)
            mDevice.deallocateBuffer(*mCipherText);

        if (mPlainText)
            mDevice.deallocateBuffer(*mPlainText);
    }
    catch (...)
    {
        // TODO: log?
    }
}

void AES_ECB_Decrypt::setCipherText(const unsigned char* ciphertext, size_t size)
{
    if (ciphertext == nullptr)
        throw std::invalid_argument("Non-null ciphertext is required");

    if (size == 0)
        throw std::invalid_argument("Make sure ciphertext size greater than 0");

    if (size % 16 != 0)
        throw std::invalid_argument("Ciphertext has to be padded to make full AES blocks. "
                                    "Its size has to be a multiple of 16.");

    if (!mCipherText || mCipherText->getArraySize<unsigned char>() != size)
    {
        if (mCipherText)
            mDevice.deallocateBuffer(*mCipherText);

        mCipherText = &mDevice.allocateBuffer<unsigned char>(size, DataBuffer::Read);
    }

    {
        auto data = mCipherText->lockWrite<unsigned char>();
        for (size_t i = 0; i < size; ++i)
            data[i] = ciphertext[i];
    }

    if (!mPlainText || mPlainText->getArraySize<unsigned char>() != size)
    {
        if (mPlainText)
            mDevice.deallocateBuffer(*mPlainText);

        mPlainText = &mDevice.allocateBuffer<unsigned char>(size, DataBuffer::Write);
    }
}

void AES_ECB_Decrypt::execute(size_t localWorkSize)
{
    if (!mExpandedKey)
        throw std::runtime_error("Key has not been set.");

    if (!mCipherText)
        throw std::runtime_error("CipherText has not been set.");

    if (!mPlainText)
        throw std::runtime_error("PlainText buffer has not been allocated! This is most likely a bug.");

    Program& program = mSystem.getProgramFromCache(mDevice, ProgramSources::AES);
    cl_uint rounds = 0;

    switch (mExpandedKey->getSize())
    {
        case 11 * 16: // 128bit mode
            rounds = 11;
            break;

        case 13 * 16: // 192bit mode
            rounds = 13;
            break;

        case 15 * 16: // 256bit mode
            rounds = 15;
            break;

        default:
            throw std::runtime_error("Unexpected expanded key size.");
    }

    const cl_uint cipherTextSize = mCipherText->getArraySize<unsigned char>();
    assert(cipherTextSize % 16 == 0);
    const cl_uint blockCount = cipherTextSize / 16;

    ScopedKernel kernel(program.createKernel("AES_ECB_Decrypt"));

    kernel->setParameter(0, *mCipherText);
    kernel->setParameter(1, *mExpandedKey);
    kernel->setParameter(2, *mPlainText);
    kernel->setParameter(3, &rounds);
    kernel->allocateLocalParameter<cl_uchar16>(4, localWorkSize);

    kernel->execute(blockCount, localWorkSize, false);
}

}
