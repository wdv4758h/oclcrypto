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

#include <oclcrypto/AES_GCM.h>
#include <oclcrypto/System.h>
#include <oclcrypto/DataBuffer.h>

#include <boost/test/unit_test.hpp>

struct AES_GCM_Fixture
{
    AES_GCM_Fixture():
        system(true)
    {}

    oclcrypto::System system;
};

BOOST_FIXTURE_TEST_SUITE(AES_GCM, AES_GCM_Fixture)

BOOST_AUTO_TEST_CASE(Encrypt128)
{
    BOOST_REQUIRE_GT(system.getDeviceCount(), 0);

    {
        // test vector taken from proposed GCM spec, Appendix B test vectors

        const unsigned char plaintext[] =
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        const unsigned char key[] =
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        const unsigned char initial_vector[] =
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        const unsigned char expected_ciphertext[] =
        {
            0x03, 0x88, 0xda, 0xce, 0x60, 0xb6, 0xa3, 0x92,
            0xf3, 0x28, 0xc2, 0xb9, 0x71, 0xb2, 0xfe, 0x78
        };

        for (size_t i = 0; i < system.getDeviceCount(); ++i)
        {
            oclcrypto::Device& device = system.getDevice(i);

            oclcrypto::AES_GCM_Encrypt encrypt(system, device);
            encrypt.setKey(key, 16);
            encrypt.setInitialVector(initial_vector);
            encrypt.setPlainText(plaintext, 16);

            encrypt.execute(1);

            {
                auto data = encrypt.getCipherText()->lockRead<unsigned char>();
                for (size_t j = 0; j < data.size(); ++j)
                    BOOST_CHECK_EQUAL(data[j], expected_ciphertext[j]);
            }
        }
    }

    {
        // test vector taken from proposed GCM spec, Appendix B test vectors

        const unsigned char plaintext[] =
        {
            0xd9, 0x31, 0x32, 0x25, 0xf8, 0x84, 0x06, 0xe5, 0xa5, 0x59, 0x09, 0xc5, 0xaf, 0xf5, 0x26, 0x9a,
            0x86, 0xa7, 0xa9, 0x53, 0x15, 0x34, 0xf7, 0xda, 0x2e, 0x4c, 0x30, 0x3d, 0x8a, 0x31, 0x8a, 0x72,
            0x1c, 0x3c, 0x0c, 0x95, 0x95, 0x68, 0x09, 0x53, 0x2f, 0xcf, 0x0e, 0x24, 0x49, 0xa6, 0xb5, 0x25,
            0xb1, 0x6a, 0xed, 0xf5, 0xaa, 0x0d, 0xe6, 0x57, 0xba, 0x63, 0x7b, 0x39, 0x1a, 0xaf, 0xd2, 0x55
        };

        const unsigned char key[] =
        {
            0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
            0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08,
        };

        const unsigned char initial_vector[] =
        {
            0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xdb, 0xad, 0xde, 0xca, 0xf8, 0x88, /* added padding */ 0x00, 0x00, 0x00, 0x00
        };

        const unsigned char expected_ciphertext[] =
        {
            0x42, 0x83, 0x1e, 0xc2, 0x21, 0x77, 0x74, 0x24, 0x4b, 0x72, 0x21, 0xb7, 0x84, 0xd0, 0xd4, 0x9c,
            0xe3, 0xaa, 0x21, 0x2f, 0x2c, 0x02, 0xa4, 0xe0, 0x35, 0xc1, 0x7e, 0x23, 0x29, 0xac, 0xa1, 0x2e,
            0x21, 0xd5, 0x14, 0xb2, 0x54, 0x66, 0x93, 0x1c, 0x7d, 0x8f, 0x6a, 0x5a, 0xac, 0x84, 0xaa, 0x05,
            0x1b, 0xa3, 0x0b, 0x39, 0x6a, 0x0a, 0xac, 0x97, 0x3d, 0x58, 0xe0, 0x91, 0x47, 0x3f, 0x59, 0x85
        };

        for (size_t i = 0; i < system.getDeviceCount(); ++i)
        {
            oclcrypto::Device& device = system.getDevice(i);

            oclcrypto::AES_GCM_Encrypt encrypt(system, device);
            encrypt.setKey(key, 16);
            encrypt.setInitialVector(initial_vector);
            encrypt.setPlainText(plaintext, 4 * 16);

            encrypt.execute(1);

            {
                auto data = encrypt.getCipherText()->lockRead<unsigned char>();
                for (size_t j = 0; j < data.size(); ++j)
                    BOOST_CHECK_EQUAL(data[j], expected_ciphertext[j]);
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
