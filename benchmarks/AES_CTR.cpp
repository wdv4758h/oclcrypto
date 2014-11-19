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

#include <oclcrypto/AES_CTR.h>
#include <oclcrypto/System.h>
#include <oclcrypto/Device.h>
#include <oclcrypto/DataBuffer.h>

#include <boost/timer/timer.hpp>

#include "DataGenerator.h"

#include <iostream>

boost::timer::cpu_times time_AES_CTR(
    oclcrypto::System& system, oclcrypto::Device& device,
    size_t keySize, size_t plaintextSize, unsigned int iterations)
{
    const std::vector<unsigned char> plaintext = generateRandomVector(plaintextSize);
    const std::vector<unsigned char> key = generateRandomVector(keySize);
    const std::vector<unsigned char> ic = generateRandomVector(16);

    boost::timer::cpu_timer timer;
    oclcrypto::AES_CTR_Encrypt encrypt(system, device);
    encrypt.setKey(key.data(), key.size());

    for (size_t j = 0; j < iterations; ++j)
    {
        encrypt.setInitialCounter(ic.data());
        encrypt.setPlainText(plaintext.data(), plaintext.size());
        encrypt.execute(256);
        auto lock = encrypt.getCipherText()->lockRead<unsigned char>();
    }

    return timer.elapsed();
}

void benchmark_AES_CTR(oclcrypto::System& system, size_t keySize, size_t plaintextSize)
{
    const unsigned int iterations = 100;

    std::cout << "AES CTR " + std::to_string(keySize * 8) + "bit with " + std::to_string(plaintextSize) + "-byte random plaintexts" << std::endl;

    for (size_t i = 0; i < system.getDeviceCount(); ++i)
    {
        oclcrypto::Device& device = system.getDevice(i);
        const boost::timer::cpu_times times = time_AES_CTR(system, device, keySize, plaintextSize, iterations);
        const size_t totalBytes = iterations * plaintextSize;
        const double bytesPerSecond = (double)totalBytes / times.wall * 1000 * 1000 * 1000;

        std::cout << " - " << (bytesPerSecond * 0.001) << " KB/sec on " << device.getName() << std::endl;
    }
}

void AES_CTR_Benchmarks()
{
    oclcrypto::System system(true);

    for (unsigned short keyMul = 0; keyMul <= 2; ++keyMul)
    {
        const size_t keySize = 16 + keyMul * 8;

        for (unsigned short plaintextMul = 1; plaintextMul <= 128; plaintextMul *= 4)
        {
            const size_t plaintextSize = 4096 * plaintextMul;

            benchmark_AES_CTR(system, keySize, plaintextSize);
            std::cout << std::endl;
        }
    }
}
