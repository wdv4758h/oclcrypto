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

#include <oclcrypto/System.h>

#include <boost/test/unit_test.hpp>

struct AES_ECB_Fixture
{
    AES_ECB_Fixture():
        system(true)
    {}

    oclcrypto::System system;
};

BOOST_FIXTURE_TEST_SUITE(AES_ECB, AES_ECB_Fixture)

// arbitrary text that's easy to see in debugger
constexpr char plaintext[] = "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~~";

BOOST_AUTO_TEST_CASE(Encrypt128)
{
    BOOST_REQUIRE_GT(system.getDeviceCount(), 0);

    constexpr char key[] = "0123456789abcdef";
    // openssl aes-128-ecb -nosalt -in plaintext.txt -out ciphertext.txt
    // then convert each byte to hex
    constexpr char expected_ciphertext[] =
    {
        '\x14', '\x19', '\x1d', '\x69', '\x37', '\x0f', '\x8c', '\x5d',
        '\xb5', '\xc3', '\xd8', '\xb7', '\x6d', '\x53', '\xbb', '\xa5',
        '\x00', '\x92', '\xfb', '\x21', '\x85', '\x4a', '\xe1', '\x12',
        '\x91', '\x9c', '\xd0', '\xff', '\x5a', '\x95', '\x2a', '\xd7',
        '\x3a', '\xd0', '\xcd', '\x3d', '\xaa', '\x87', '\x13', '\xbb',
        '\x93', '\xbb', '\x60', '\x70', '\xe2', '\x48', '\x51', '\xdb',
        '\xad', '\x9a', '\x25', '\xb3', '\x58', '\xfb', '\x31', '\x0b',
        '\x33', '\x8b', '\xcd', '\x09', '\xeb', '\x68', '\x67', '\xf9',
        '\x07', '\x51', '\xa8', '\x2e', '\x87', '\xe4', '\x01', '\x4e',
        '\xfc', '\x6b', '\x87', '\x21', '\x60', '\x0d', '\xe1', '\x2a',
    };

    for (size_t i = 0; i < system.getDeviceCount(); ++i)
    {
        /*
        oclcrypto::AES_ECB_Encrypt encrypt(device, 64);
        encrypt.setKey(key);
        encrypt.setPlainText(plaintext);

        encrypt.execute();

        {
            auto data = encrypt.ciphertext.lockRead<char>();
            for (size_t j = 0; j < 64; ++j)
                BOOST_CHECK_EQUAL(data[j], expected_ciphertext[j]);
        }
        */
    }
}

BOOST_AUTO_TEST_SUITE_END()
