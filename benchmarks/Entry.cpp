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

#include "ResultsAggregator.h"

void OpenCL_DataBuffer_Benchmarks(ResultsAggregator& results);
void AES_ECB_Benchmarks(ResultsAggregator& results);
void AES_CTR_Benchmarks(ResultsAggregator& results);
void BLOWFISH_ECB_Benchmarks(ResultsAggregator& results);

int main(int argc, char** argv)
{
    ResultsAggregator results;

    OpenCL_DataBuffer_Benchmarks(results);
    AES_ECB_Benchmarks(results);
    AES_CTR_Benchmarks(results);
    BLOWFISH_ECB_Benchmarks(results);

    results.print();
}
