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

#include <oclcrypto/System.h>
#include <oclcrypto/DataBuffer.h>
#include <oclcrypto/AES_ECB.h>

#include <openssl/engine.h>
#include <openssl/aes.h>

#include <iostream>
#include <string.h>

#define DYNAMIC_ENGINE
#define OCLCRYPTO_ENGINE_ID "oclcrypto"
#define OCLCRYPTO_ENGINE_NAME	"oclrypto engine"

static int oclcrypto_cipher_nids[] = {
    NID_aes_128_ecb,
    //NID_aes_128_ctr,

    //NID_aes_192_ecb,
    //NID_aes_192_ctr,

    //NID_aes_256_ecb,
    //NID_aes_256_ctr,
};

static int oclcrypto_cipher_nids_num = (sizeof(oclcrypto_cipher_nids)/sizeof(oclcrypto_cipher_nids[0]));

static int oclcrypto_aes_ecb_init(EVP_CIPHER_CTX* ctx, const unsigned char* key, const unsigned char* /*iv*/, int /*enc*/)
{
    ENGINE* engine = ctx->engine;
    oclcrypto::System* system = reinterpret_cast<oclcrypto::System*>(ENGINE_get_ex_data(engine, 0));

    if (!ctx->cipher_data)
        ctx->cipher_data = ctx->encrypt ?
            reinterpret_cast<void*>(new oclcrypto::AES_ECB_Encrypt(*system, system->getBestDevice())) :
            reinterpret_cast<void*>(new oclcrypto::AES_ECB_Decrypt(*system, system->getBestDevice()));

    const int key_len = EVP_CIPHER_CTX_key_length(ctx); // in bytes

    // TODO: Maybe we can just use AES_Base?
    if (ctx->encrypt)
    {
        oclcrypto::AES_ECB_Encrypt* context = reinterpret_cast<oclcrypto::AES_ECB_Encrypt*>(ctx->cipher_data);
        context->setKey(key, key_len);
    }
    else // decrypt
    {
        oclcrypto::AES_ECB_Decrypt* context = reinterpret_cast<oclcrypto::AES_ECB_Decrypt*>(ctx->cipher_data);
        context->setKey(key, key_len);
    }

	return 1;
}

static int oclcrypto_aes_ecb_do_cipher(EVP_CIPHER_CTX* ctx, unsigned char* out_arg, const unsigned char* in_arg, size_t nbytes)
{
    //assert(in_arg && out_arg && ctx && nbytes);

    switch (EVP_CIPHER_CTX_mode(ctx))
    {
    case EVP_CIPH_ECB_MODE:
        if (ctx->encrypt)
        {
            oclcrypto::AES_ECB_Encrypt* context = reinterpret_cast<oclcrypto::AES_ECB_Encrypt*>(ctx->cipher_data);
            context->setPlainText(in_arg, nbytes);
            context->execute(128); // TODO: arbitrary
            {
                auto data = context->getCipherText()->lockRead<unsigned char>();
                for (size_t j = 0; j < data.size(); ++j)
                    out_arg[j] = data[j];
            }
        }
        else
        {
            oclcrypto::AES_ECB_Decrypt* context = reinterpret_cast<oclcrypto::AES_ECB_Decrypt*>(ctx->cipher_data);
            context->setCipherText(in_arg, nbytes);
            context->execute(128); // TODO: arbitrary
            {
                auto data = context->getPlainText()->lockRead<unsigned char>();
                for (size_t j = 0; j < data.size(); ++j)
                    out_arg[j] = data[j];
            }
        }
    }

    return 1;
}

static int oclcrypto_aes_ecb_cleanup(EVP_CIPHER_CTX* ctx)
{
    //ENGINE* engine = ctx->engine;
    //oclcrypto::System* system = reinterpret_cast<oclcrypto::System*>(ENGINE_get_ex_data(engine, 0));

    if (ctx->cipher_data)
    {
        if (ctx->encrypt)
        {
            oclcrypto::AES_ECB_Encrypt* context = reinterpret_cast<oclcrypto::AES_ECB_Encrypt*>(ctx->cipher_data);
            delete context;
        }
        else
        {
            oclcrypto::AES_ECB_Decrypt* context = reinterpret_cast<oclcrypto::AES_ECB_Decrypt*>(ctx->cipher_data);
            delete context;
        }
    }

    return 1;
}

static const EVP_CIPHER oclcrypto_aes_128_ecb =
{
    NID_aes_128_ecb, // nid
    16, // block_size
    16, // key_len
    0, // iv_len
    EVP_CIPH_ECB_MODE, // flags
    oclcrypto_aes_ecb_init, // init
    oclcrypto_aes_ecb_do_cipher, // do_cipher
    oclcrypto_aes_ecb_cleanup, // cleanup
    sizeof(void*) + 16, // ctx_size
    EVP_CIPHER_set_asn1_iv, // set_asn1_parameters - unused?
    EVP_CIPHER_get_asn1_iv, // get_asn1_parameters - unused?
    nullptr, // control
    nullptr // app_data
};


static int oclcrypto_ciphers(ENGINE* e, const EVP_CIPHER** cipher, const int** nids, int nid)
{
    if (!cipher)
    {
        *nids = oclcrypto_cipher_nids;
        return oclcrypto_cipher_nids_num;
    }

    switch (nid)
    {
        case NID_aes_128_ecb:
            *cipher = &oclcrypto_aes_128_ecb;
            break;
        default:
            *cipher = nullptr;
            return 0;
    }

	return 1;
}

extern "C" int oclcrypto_init(ENGINE* engine)
{
    oclcrypto::System* system = new oclcrypto::System();
    ENGINE_set_ex_data(engine, 0, system);
    return 1;
}

extern "C" int oclcrypto_finish(ENGINE* engine)
{
    oclcrypto::System* system = reinterpret_cast<oclcrypto::System*>(ENGINE_get_ex_data(engine, 0));
    delete system;
    ENGINE_set_ex_data(engine, 0, nullptr);
    return 1;
}

static const ENGINE_CMD_DEFN oclcrypto_cmd_defns[] = {
    {0, NULL, NULL, 0}
};

static int oclcrypto_engine_ctrl(ENGINE* e, int cmd, long i, void* p, void (*f) ())
{
    switch (cmd)
    {
        default:
            break;
    }
    std::cerr << "Unknown command" << std::endl;
    return 0;
}

static int oclcrypto_bind_helper(ENGINE * e)
{
    if (!ENGINE_set_id(e, OCLCRYPTO_ENGINE_ID) ||
        !ENGINE_set_init_function(e, oclcrypto_init) ||
        !ENGINE_set_finish_function(e, oclcrypto_finish) ||
        !ENGINE_set_ctrl_function(e, oclcrypto_engine_ctrl) ||
        !ENGINE_set_cmd_defns(e, oclcrypto_cmd_defns) ||
        !ENGINE_set_name(e, OCLCRYPTO_ENGINE_NAME) ||
        !ENGINE_set_ciphers(e, oclcrypto_ciphers))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

static ENGINE* ENGINE_oclcrypto()
{
    ENGINE* ret = ENGINE_new();
    if (!ret)
        return nullptr;

    if (!oclcrypto_bind_helper(ret))
    {
        ENGINE_free(ret);
        return nullptr;
    }
    return ret;
}

static int oclcrypto_bind_fn(ENGINE *e, const char *id)
{
    if (id && (strcmp(id, OCLCRYPTO_ENGINE_ID) != 0))
    {
        std::cerr << "bad engine id" << std::endl;
        return 0;
    }
    if (!oclcrypto_bind_helper(e))
    {
        std::cerr << "bind failed" << std::endl;
        return 0;
    }

    return 1;
}

IMPLEMENT_DYNAMIC_CHECK_FN()
IMPLEMENT_DYNAMIC_BIND_FN(oclcrypto_bind_fn)
