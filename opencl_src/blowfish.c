/*
 * Copyright (C) 2015 Martin Preisler <martin@preisler.me>
 *
 * This file is part of oclcrypto.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * ''Software''), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED ''AS IS'', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

inline void BLOWFISH_SplitBlock(const unsigned long* block, unsigned int* left, unsigned int* right)
{
    const uchar8* block_uc = (const uchar8*)block;
    uchar4* left_uc = (uchar4*)left;
    uchar4* right_uc = (uchar4*)right;

#ifdef LITTLE_ENDIAN
    left_uc->s3210 = block_uc->s0123;
    right_uc->s3210 = block_uc->s4567;
#else
    left_uc->s0123 = block_uc->s0123;
    right_uc->s0123 = block_uc->s4567;
#endif
}

inline void BLOWFISH_WriteResultBlock(const unsigned long* block, __global __write_only unsigned long* restrict ret)
{
#ifdef LITTLE_ENDIAN
    const uchar8* block_uc = (const uchar8*)block;
    __global uchar8* restrict ret_uc = (__global uchar8* restrict)ret;

    ret_uc->s76543210 = block_uc->s01234567;
#else
    *ret = *block;
#endif
}

inline unsigned long BLOWFISH_f(unsigned int x, __global __read_only unsigned int* restrict sboxes)
{
    const unsigned int d = (unsigned int)(x & 0x00ff);
    x >>= 8;

    const unsigned int c = (unsigned int)(x & 0x00ff);
    x >>= 8;

    const unsigned int b = (unsigned int)(x & 0x00ff);
    x >>= 8;

    const unsigned int a = (unsigned int)(x & 0x00ff);

    unsigned long ret  = sboxes[0 * 256 + a] + sboxes[1 * 256 + b];
    ret ^= sboxes[2 * 256 + c];
    ret += sboxes[3 * 256 + d];

    return ret;
}

__kernel void BLOWFISH_ECB_Encrypt(
    __global __read_only unsigned long* restrict plainText,
    __global __read_only unsigned int* restrict p,
    __global __read_only unsigned int* restrict sboxes,
    __global __write_only unsigned long* restrict cipherText)
{
    const int global_id = get_global_id(0);
    unsigned long block = plainText[global_id];

    unsigned int left;
    unsigned int right;
    BLOWFISH_SplitBlock(&block, &left, &right);

    left ^= p[0];
    // I had compiler error problems with pocl just porting the key schedule
    // code with swaps. That's why the i += 2 variant is used here, without
    // swaps. It's a little bit less readable but works on all the platforms.
    for (int i = 1; i < 16; i += 2)
    {
        right ^= p[i];
        right ^= BLOWFISH_f(left, sboxes);
        left ^= p[i + 1];
        left ^= BLOWFISH_f(right, sboxes);
    }
    right ^= p[16 + 1];

    // fuse the block back
    block = ((unsigned long)right) << 32 | left;
    // use this function to avoid endianess issues
    BLOWFISH_WriteResultBlock(&block, &cipherText[global_id]);
}
