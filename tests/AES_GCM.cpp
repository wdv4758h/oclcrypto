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
}

BOOST_AUTO_TEST_SUITE_END()
