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

#include "oclcrypto/BLOWFISH_ECB.h"
#include "oclcrypto/Device.h"
#include "oclcrypto/DataBuffer.h"
#include "oclcrypto/Program.h"
#include "oclcrypto/Kernel.h"
#include "oclcrypto/System.h"

#include <cassert>
#include <string>

namespace oclcrypto
{

BLOWFISH_ECB_Encrypt::BLOWFISH_ECB_Encrypt(System& system, Device& device):
    BLOWFISH_Base(system, device),

    mPlainText(nullptr),
    mCipherText(nullptr)
{}

BLOWFISH_ECB_Encrypt::~BLOWFISH_ECB_Encrypt()
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

void BLOWFISH_ECB_Encrypt::setPlainText(const unsigned char* plaintext, size_t size)
{
    if (plaintext == nullptr)
        throw std::invalid_argument("Non-null plaintext is required");

    if (size == 0)
        throw std::invalid_argument("Make sure plaintext size greater than 0");

    if (size % 8 != 0)
        throw std::invalid_argument("Plaintext has to be padded to make full BLOWFISH blocks. "
                                    "Its size has to be a multiple of 8.");

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

void BLOWFISH_ECB_Encrypt::execute(size_t localWorkSize)
{
    if (!mP || !mSBoxes)
        throw std::runtime_error("Key has not been set.");

    if (!mPlainText)
        throw std::runtime_error("Plaintext has not been set.");

    if (!mCipherText)
        throw std::runtime_error("CipherText buffer has not been allocated! This is most likely a bug.");

    Program& program = mSystem.getProgramFromCache(mDevice, ProgramSources::BLOWFISH);

    const cl_uint plainTextSize = mPlainText->getArraySize<unsigned char>();
    assert(plainTextSize % 8 == 0);
    const cl_uint blockCount = plainTextSize / 8;

    ScopedKernel kernel(program.createKernel("BLOWFISH_ECB_Encrypt"));

    kernel->setParameter(0, *mPlainText);
    kernel->setParameter(1, *mP);
    kernel->setParameter(2, *mSBoxes);
    kernel->setParameter(3, *mCipherText);
    //kernel->allocateLocalParameter<cl_uchar16>(4, localWorkSize);

    kernel->execute(blockCount, localWorkSize, false);
}

}
