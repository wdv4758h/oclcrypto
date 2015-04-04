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

#include <oclcrypto/BLOWFISH_ECB.h>
#include <oclcrypto/System.h>
#include <oclcrypto/DataBuffer.h>

#include <boost/test/unit_test.hpp>

struct BLOWFISH_ECB_Fixture
{
    BLOWFISH_ECB_Fixture():
        system(true)
    {}

    oclcrypto::System system;
};

BOOST_FIXTURE_TEST_SUITE(BLOWFISH_ECB, BLOWFISH_ECB_Fixture)

/*
BOOST_AUTO_TEST_CASE(EncryptInvalid)
{
    BOOST_REQUIRE_GT(system.getDeviceCount(), 0);

    for (size_t i = 0; i < system.getDeviceCount(); ++i)
    {
        oclcrypto::Device& device = system.getDevice(i);

        oclcrypto::AES_ECB_Encrypt encrypt(system, device);

        BOOST_CHECK_THROW(encrypt.setKey("abcd", 4), std::invalid_argument);
        BOOST_CHECK_THROW(encrypt.setKey(static_cast<const unsigned char*>(nullptr), 16), std::invalid_argument);
    }
}*/

BOOST_AUTO_TEST_CASE(Encrypt)
{
    BOOST_REQUIRE_GT(system.getDeviceCount(), 0);

    {
        const unsigned char plaintext[] =
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        const unsigned char key[] =
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        const unsigned char expected_ciphertext[] =
        {
            0x4e, 0xf9, 0x97, 0x45, 0x61, 0x98, 0xdd, 0x78
        };

        for (size_t i = 0; i < system.getDeviceCount(); ++i)
        {
            oclcrypto::Device& device = system.getDevice(i);

            oclcrypto::BLOWFISH_ECB_Encrypt encrypt(system, device);
            encrypt.setKey(key, 8);
            encrypt.setPlainText(plaintext, 8);

            encrypt.execute(1);

            {
                auto data = encrypt.getCipherText()->lockRead<unsigned char>();
                for (size_t j = 0; j < data.size(); ++j)
                    BOOST_CHECK_EQUAL(data[j], expected_ciphertext[j]);
            }
        }
    }

    {
        const unsigned char plaintext[] =
        {
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
        };

        const unsigned char key[] =
        {
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
        };

        const unsigned char expected_ciphertext[] =
        {
            0x51, 0x86, 0x6f, 0xd5, 0xb8, 0x5e, 0xcb, 0x8a
        };

        for (size_t i = 0; i < system.getDeviceCount(); ++i)
        {
            oclcrypto::Device& device = system.getDevice(i);

            oclcrypto::BLOWFISH_ECB_Encrypt encrypt(system, device);
            encrypt.setKey(key, 8);
            encrypt.setPlainText(plaintext, 8);

            encrypt.execute(1);

            {
                auto data = encrypt.getCipherText()->lockRead<unsigned char>();
                for (size_t j = 0; j < data.size(); ++j)
                    BOOST_CHECK_EQUAL(data[j], expected_ciphertext[j]);
            }
        }
    }

    {
        const unsigned char plaintext[] =
        {
            0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
        };

        const unsigned char key[] =
        {
            0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11
        };

        const unsigned char expected_ciphertext[] =
        {
            0x7d, 0x0c, 0xc6, 0x30, 0xaf, 0xda, 0x1e, 0xc7 // WRONG
        };

        for (size_t i = 0; i < system.getDeviceCount(); ++i)
        {
            oclcrypto::Device& device = system.getDevice(i);

            oclcrypto::BLOWFISH_ECB_Encrypt encrypt(system, device);
            encrypt.setKey(key, 8);
            encrypt.setPlainText(plaintext, 8);

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
