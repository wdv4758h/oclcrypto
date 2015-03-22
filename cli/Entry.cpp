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

#include <string>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <iomanip>

#include <oclcrypto/System.h>
#include <oclcrypto/Device.h>
#include <oclcrypto/AES_ECB.h>
#include <oclcrypto/AES_CTR.h>
#include <oclcrypto/AES_GCM.h>

// TODO: Would be great to use boost::program_options or something similar
//       I don't to avoid dependencies.

int main(int argc, char** argv)
{
    const std::string help =
        "oclcrypto-cli ALGORITHM KEY INPUT_FILE OUTPUT_FILE\n"
        "\n"
        "ALGORITHM can be one of aes-ecb-enc, aes-ecb-dec."//, aes-ctr-enc, aes-gcm-enc."
        "If OUTPUT_FILE is missing the result is printed to stdout."
        "\n"
        "Example: oclcrypto-cli aes-ecb-enc HELOHELOHELOHELO inputfile.txt outputfile.txt\n"
        ;

    if (argc < 1 + 4)
    {
        std::cout << "At least 4 arguments required." << std::endl;
        std::cout << std::endl;
        std::cout << help << std::endl;

        return 1;
    }

    const std::string algorithm = argv[1];
    const std::string key = argv[2];
    std::ifstream input_file(argv[3]);
    std::vector<char> input_text(
        (std::istreambuf_iterator<char>(input_file)),
        std::istreambuf_iterator<char>()
    );
    size_t padSize = 16 - input_text.size() % 16;
    if (padSize < 16)
    {
        for (size_t i = 0; i < padSize; ++i)
            input_text.push_back('\0');
    }
    std::vector<char> output_text;
    std::ofstream output_file(argv[4]);

    oclcrypto::System system;
    oclcrypto::Device& device = system.getBestDevice();

    if (algorithm.compare(0, 3, "aes") == 0)
    {
        oclcrypto::AES_Base* alg = nullptr;

        if (algorithm == "aes-ecb-enc")
        {
            alg = new oclcrypto::AES_ECB_Encrypt(system, device);
        }
        else if (algorithm == "aes-ecb-dec")
        {
            alg = new oclcrypto::AES_ECB_Decrypt(system, device);
        }
        /*else if (algorithm == "aes-ctr-enc")
        {
            alg = new oclcrypto::AES_CTR_Encrypt(system, device);
        }
        else if (algorithm == "aes-gcm-enc")
        {
            alg = new oclcrypto::AES_GCM_Encrypt(system, device);
        }*/
        else
        {
            std::cout << "Invalid algorithm." << std::endl;
            std::cout << std::endl;
            std::cout << help << std::endl;

            return 1;
        }

        alg->setKey(key.c_str(), key.size());

        if (algorithm == "aes-ecb-enc")
        {
            oclcrypto::AES_ECB_Encrypt* algo = static_cast<oclcrypto::AES_ECB_Encrypt*>(alg);
            algo->setPlainText(&input_text[0], input_text.size());
            algo->execute(device.suggestLocalWorkSize());

            const size_t size = algo->getCipherText()->getArraySize<char>();
            {
                auto data = algo->getCipherText()->lockRead<char>();
                for (size_t i = 0; i < size; ++i)
                    output_text.push_back(data[i]);
            }
        }
        else if (algorithm == "aes-ecb-dec")
        {
            oclcrypto::AES_ECB_Decrypt* algo = static_cast<oclcrypto::AES_ECB_Decrypt*>(alg);
            algo->setCipherText(&input_text[0], input_text.size());
            algo->execute(device.suggestLocalWorkSize());

            const size_t size = algo->getPlainText()->getArraySize<char>();
            {
                auto data = algo->getPlainText()->lockRead<char>();
                for (size_t i = 0; i < size; ++i)
                    output_text.push_back(data[i]);
            }
        }
        /*
        else if (algorithm == "aes-ctr-enc")
        {
            alg = new oclcrypto::AES_CTR_Encrypt(system, device);
        }
        else if (algorithm == "aes-gcm-enc")
        {
            alg = new oclcrypto::AES_GCM_Encrypt(system, device);
        }*/

        for (size_t i = 0; i < output_text.size(); ++i)
            output_file << output_text[i];
    }

    return 0;
}
