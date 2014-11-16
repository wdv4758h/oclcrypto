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

#include <oclcrypto/AES_ECB.h>
#include <oclcrypto/System.h>
#include <oclcrypto/DataBuffer.h>

#include <boost/test/unit_test.hpp>

struct AES_ECB_Fixture
{
    AES_ECB_Fixture():
        system(true)
    {}

    oclcrypto::System system;
};

BOOST_FIXTURE_TEST_SUITE(AES_ECB, AES_ECB_Fixture)

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
}

BOOST_AUTO_TEST_CASE(Encrypt128)
{
    BOOST_REQUIRE_GT(system.getDeviceCount(), 0);

    {
        // test vector generated using openssl:
        // put 16x 0x00 into plain, then run:
        // $ openssl aes-128-ecb -nosalt -K 00000000000000000000000000000000 -p -nopad -in plain -out cipher

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

        const unsigned char expected_ciphertext[] =
        {
            0x66, 0xe9, 0x4b, 0xd4, 0xef, 0x8a, 0x2c, 0x3b,
            0x88, 0x4c, 0xfa, 0x59, 0xca, 0x34, 0x2b, 0x2e
        };

        for (size_t i = 0; i < system.getDeviceCount(); ++i)
        {
            oclcrypto::Device& device = system.getDevice(i);

            oclcrypto::AES_ECB_Encrypt encrypt(system, device);
            encrypt.setKey(key, 16);
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
        // test vector taken from FIPS 197, example C.1

        const unsigned char plaintext[] =
        {
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
        };

        const unsigned char key[] =
        {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        };

        const unsigned char expected_ciphertext[] =
        {
            0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
            0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a
        };

        for (size_t i = 0; i < system.getDeviceCount(); ++i)
        {
            oclcrypto::Device& device = system.getDevice(i);

            oclcrypto::AES_ECB_Encrypt encrypt(system, device);
            encrypt.setPlainText(plaintext, 16);
            encrypt.setKey(key, 16);

            encrypt.execute(1);

            {
                auto data = encrypt.getCipherText()->lockRead<unsigned char>();
                for (size_t j = 0; j < data.size(); ++j)
                    BOOST_CHECK_EQUAL(data[j], expected_ciphertext[j]);
            }
        }
    }

    {
        // test vector taken from FIPS 197, example C.1, stretched to 8x the size

        const unsigned char plaintext[] =
        {
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
        };

        const unsigned char key[] =
        {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        };

        const unsigned char expected_ciphertext[] =
        {
            0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
            0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a,
            0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
            0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a,
            0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
            0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a,
            0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
            0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a,
            0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
            0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a,
            0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
            0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a,
            0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
            0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a,
            0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
            0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a
        };

        for (size_t i = 0; i < system.getDeviceCount(); ++i)
        {
            oclcrypto::Device& device = system.getDevice(i);

            oclcrypto::AES_ECB_Encrypt encrypt(system, device);
            encrypt.setPlainText(plaintext, 16 * 8);
            encrypt.setKey(key, 16);

            encrypt.execute(1);

            {
                auto data = encrypt.getCipherText()->lockRead<unsigned char>();
                for (size_t j = 0; j < data.size(); ++j)
                    BOOST_CHECK_EQUAL(data[j], expected_ciphertext[j]);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(Encrypt192)
{
    BOOST_REQUIRE_GT(system.getDeviceCount(), 0);

    {
        // test vector taken from FIPS 197, example C.2
        const unsigned char plaintext[] =
        {
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
        };

        const unsigned char key[] =
        {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17
        };

        const unsigned char expected_ciphertext[] =
        {
            0xdd, 0xa9, 0x7c, 0xa4, 0x86, 0x4c, 0xdf, 0xe0,
            0x6e, 0xaf, 0x70, 0xa0, 0xec, 0x0d, 0x71, 0x91
        };

        for (size_t i = 0; i < system.getDeviceCount(); ++i)
        {
            oclcrypto::Device& device = system.getDevice(i);

            oclcrypto::AES_ECB_Encrypt encrypt(system, device);
            encrypt.setKey(key, 24);
            encrypt.setPlainText(plaintext, 16);

            encrypt.execute(1);

            {
                auto data = encrypt.getCipherText()->lockRead<unsigned char>();
                for (size_t j = 0; j < data.size(); ++j)
                    BOOST_CHECK_EQUAL(data[j], expected_ciphertext[j]);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(Encrypt256)
{
    BOOST_REQUIRE_GT(system.getDeviceCount(), 0);

    {
        // test vector taken from FIPS 197, example C.3
        const unsigned char plaintext[] =
        {
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
        };

        const unsigned char key[] =
        {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
            0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
        };

        const unsigned char expected_ciphertext[] =
        {
            0x8e, 0xa2, 0xb7, 0xca, 0x51, 0x67, 0x45, 0xbf,
            0xea, 0xfc, 0x49, 0x90, 0x4b, 0x49, 0x60, 0x89,
        };

        for (size_t i = 0; i < system.getDeviceCount(); ++i)
        {
            oclcrypto::Device& device = system.getDevice(i);

            oclcrypto::AES_ECB_Encrypt encrypt(system, device);
            encrypt.setKey(key, 32);
            encrypt.setPlainText(plaintext, 16);

            encrypt.execute(1);

            {
                auto data = encrypt.getCipherText()->lockRead<unsigned char>();
                for (size_t j = 0; j < data.size(); ++j)
                    BOOST_CHECK_EQUAL(data[j], expected_ciphertext[j]);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(DecryptInvalid)
{
    BOOST_REQUIRE_GT(system.getDeviceCount(), 0);

    for (size_t i = 0; i < system.getDeviceCount(); ++i)
    {
        oclcrypto::Device& device = system.getDevice(i);

        oclcrypto::AES_ECB_Decrypt decrypt(system, device);

        BOOST_CHECK_THROW(decrypt.setKey("abcd", 4), std::invalid_argument);
        BOOST_CHECK_THROW(decrypt.setKey(static_cast<const unsigned char*>(nullptr), 16), std::invalid_argument);
    }
}

BOOST_AUTO_TEST_CASE(Decrypt128)
{
    BOOST_REQUIRE_GT(system.getDeviceCount(), 0);

    {
        // just inverted the test vector for Encrypt128, generated with openssl

        const unsigned char ciphertext[] =
        {
            0x66, 0xe9, 0x4b, 0xd4, 0xef, 0x8a, 0x2c, 0x3b,
            0x88, 0x4c, 0xfa, 0x59, 0xca, 0x34, 0x2b, 0x2e
        };

        const unsigned char key[] =
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        const unsigned char expected_plaintext[] =
        {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        for (size_t i = 0; i < system.getDeviceCount(); ++i)
        {
            oclcrypto::Device& device = system.getDevice(i);

            oclcrypto::AES_ECB_Decrypt decrypt(system, device);
            decrypt.setKey(key, 16);
            decrypt.setCipherText(ciphertext, 16);

            decrypt.execute(1);

            {
                auto data = decrypt.getPlainText()->lockRead<unsigned char>();
                for (size_t j = 0; j < data.size(); ++j)
                    BOOST_CHECK_EQUAL(data[j], expected_plaintext[j]);
            }
        }
    }

    {
        // inverted test vector taken from FIPS 197, example C.1

        const unsigned char ciphertext[] =
        {
            0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
            0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a
        };

        const unsigned char key[] =
        {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        };

        const unsigned char expected_plaintext[] =
        {
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
        };

        for (size_t i = 0; i < system.getDeviceCount(); ++i)
        {
            oclcrypto::Device& device = system.getDevice(i);

            oclcrypto::AES_ECB_Decrypt decrypt(system, device);
            decrypt.setCipherText(ciphertext, 16);
            decrypt.setKey(key, 16);

            decrypt.execute(1);

            {
                auto data = decrypt.getPlainText()->lockRead<unsigned char>();
                for (size_t j = 0; j < data.size(); ++j)
                    BOOST_CHECK_EQUAL(data[j], expected_plaintext[j]);
            }
        }
    }

    {
        // inverted test vector taken from FIPS 197, example C.1, stretched to 8x the size

        const unsigned char ciphertext[] =
        {
            0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
            0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a,
            0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
            0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a,
            0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
            0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a,
            0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
            0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a,
            0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
            0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a,
            0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
            0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a,
            0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
            0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a,
            0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
            0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a,
        };

        const unsigned char key[] =
        {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
        };

        const unsigned char expected_plaintext[] =
        {
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
        };

        for (size_t i = 0; i < system.getDeviceCount(); ++i)
        {
            oclcrypto::Device& device = system.getDevice(i);

            oclcrypto::AES_ECB_Decrypt decrypt(system, device);
            decrypt.setCipherText(ciphertext, 16 * 8);
            decrypt.setKey(key, 16);

            decrypt.execute(1);

            {
                auto data = decrypt.getPlainText()->lockRead<unsigned char>();
                for (size_t j = 0; j < data.size(); ++j)
                    BOOST_CHECK_EQUAL(data[j], expected_plaintext[j]);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(Decrypt192)
{
    BOOST_REQUIRE_GT(system.getDeviceCount(), 0);

    {
        // inverted test vector taken from FIPS 197, example C.2
        const unsigned char ciphertext[] =
        {
            0xdd, 0xa9, 0x7c, 0xa4, 0x86, 0x4c, 0xdf, 0xe0,
            0x6e, 0xaf, 0x70, 0xa0, 0xec, 0x0d, 0x71, 0x91
        };

        const unsigned char key[] =
        {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17
        };

        const unsigned char expected_plaintext[] =
        {
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
        };

        for (size_t i = 0; i < system.getDeviceCount(); ++i)
        {
            oclcrypto::Device& device = system.getDevice(i);

            oclcrypto::AES_ECB_Decrypt decrypt(system, device);
            decrypt.setKey(key, 24);
            decrypt.setCipherText(ciphertext, 16);

            decrypt.execute(1);

            {
                auto data = decrypt.getPlainText()->lockRead<unsigned char>();
                for (size_t j = 0; j < data.size(); ++j)
                    BOOST_CHECK_EQUAL(data[j], expected_plaintext[j]);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(Decrypt256)
{
    BOOST_REQUIRE_GT(system.getDeviceCount(), 0);

    {
        // inverted test vector taken from FIPS 197, example C.3
        const unsigned char ciphertext[] =
        {
            0x8e, 0xa2, 0xb7, 0xca, 0x51, 0x67, 0x45, 0xbf,
            0xea, 0xfc, 0x49, 0x90, 0x4b, 0x49, 0x60, 0x89,
        };

        const unsigned char key[] =
        {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
            0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
        };

        const unsigned char expected_plaintext[] =
        {
            0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
        };

        for (size_t i = 0; i < system.getDeviceCount(); ++i)
        {
            oclcrypto::Device& device = system.getDevice(i);

            oclcrypto::AES_ECB_Decrypt decrypt(system, device);
            decrypt.setKey(key, 32);
            decrypt.setCipherText(ciphertext, 16);

            decrypt.execute(1);

            {
                auto data = decrypt.getPlainText()->lockRead<unsigned char>();
                for (size_t j = 0; j < data.size(); ++j)
                    BOOST_CHECK_EQUAL(data[j], expected_plaintext[j]);
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(EncryptDecryptRoundTrip128)
{
    BOOST_REQUIRE_GT(system.getDeviceCount(), 0);

    const size_t BUFFER_SIZE = 16 * 100;

    unsigned char plaintext[BUFFER_SIZE];
    for (size_t i = 0; i < BUFFER_SIZE; ++i)
        plaintext[i] = 0x00;


    const unsigned char key[] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };

    for (size_t i = 0; i < system.getDeviceCount(); ++i)
    {
        oclcrypto::Device& device = system.getDevice(i);

        oclcrypto::AES_ECB_Encrypt encrypt(system, device);
        encrypt.setKey(key, 16);
        encrypt.setPlainText(plaintext, BUFFER_SIZE);

        encrypt.execute(1);

        unsigned char ciphertext[BUFFER_SIZE];

        {
            auto data = encrypt.getCipherText()->lockRead<unsigned char>();
            for (size_t j = 0; j < data.size(); ++j)
                ciphertext[j] = data[j];
        }

        oclcrypto::AES_ECB_Decrypt decrypt(system, device);
        decrypt.setKey(key, 16);
        decrypt.setCipherText(ciphertext, BUFFER_SIZE);

        decrypt.execute(1);

        {
            auto data = decrypt.getPlainText()->lockRead<unsigned char>();
            for (size_t j = 0; j < data.size(); ++j)
                BOOST_CHECK_EQUAL(data[j], plaintext[j]);
        }
    }
}

BOOST_AUTO_TEST_CASE(EncryptDecryptRoundTrip192)
{
    BOOST_REQUIRE_GT(system.getDeviceCount(), 0);

    const size_t BUFFER_SIZE = 16 * 100;

    unsigned char plaintext[BUFFER_SIZE];
    for (size_t i = 0; i < BUFFER_SIZE; ++i)
        plaintext[i] = 0x00;


    const unsigned char key[] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17
    };

    for (size_t i = 0; i < system.getDeviceCount(); ++i)
    {
        oclcrypto::Device& device = system.getDevice(i);

        oclcrypto::AES_ECB_Encrypt encrypt(system, device);
        encrypt.setKey(key, 24);
        encrypt.setPlainText(plaintext, BUFFER_SIZE);

        encrypt.execute(1);

        unsigned char ciphertext[BUFFER_SIZE];

        {
            auto data = encrypt.getCipherText()->lockRead<unsigned char>();
            for (size_t j = 0; j < data.size(); ++j)
                ciphertext[j] = data[j];
        }

        oclcrypto::AES_ECB_Decrypt decrypt(system, device);
        decrypt.setKey(key, 24);
        decrypt.setCipherText(ciphertext, BUFFER_SIZE);

        decrypt.execute(1);

        {
            auto data = decrypt.getPlainText()->lockRead<unsigned char>();
            for (size_t j = 0; j < data.size(); ++j)
                BOOST_CHECK_EQUAL(data[j], plaintext[j]);
        }
    }
}

BOOST_AUTO_TEST_CASE(EncryptDecryptRoundTrip256)
{
    BOOST_REQUIRE_GT(system.getDeviceCount(), 0);

    const size_t BUFFER_SIZE = 16 * 100;

    unsigned char plaintext[BUFFER_SIZE];
    for (size_t i = 0; i < BUFFER_SIZE; ++i)
        plaintext[i] = 0x00;


    const unsigned char key[] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
    };

    for (size_t i = 0; i < system.getDeviceCount(); ++i)
    {
        oclcrypto::Device& device = system.getDevice(i);

        oclcrypto::AES_ECB_Encrypt encrypt(system, device);
        encrypt.setKey(key, 32);
        encrypt.setPlainText(plaintext, BUFFER_SIZE);

        encrypt.execute(1);

        unsigned char ciphertext[BUFFER_SIZE];

        {
            auto data = encrypt.getCipherText()->lockRead<unsigned char>();
            for (size_t j = 0; j < data.size(); ++j)
                ciphertext[j] = data[j];
        }

        oclcrypto::AES_ECB_Decrypt decrypt(system, device);
        decrypt.setKey(key, 32);
        decrypt.setCipherText(ciphertext, BUFFER_SIZE);

        decrypt.execute(1);

        {
            auto data = decrypt.getPlainText()->lockRead<unsigned char>();
            for (size_t j = 0; j < data.size(); ++j)
                BOOST_CHECK_EQUAL(data[j], plaintext[j]);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
