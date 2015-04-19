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

#include "ResultsAggregator.h"
#include <iostream>
#include <iomanip>

ResultsAggregator::ResultsAggregator()
{}

ResultsAggregator::~ResultsAggregator()
{}

void ResultsAggregator::addResult(const std::string& task, size_t problemSize, double totalTime)
{
    mResults[task][problemSize] = totalTime;
}

void ResultsAggregator::print()
{
    std::cout << std::fixed << std::setw( 11 ) << std::setprecision( 2 );
    for (TaskResultsMap::const_iterator it = mResults.begin();
         it != mResults.end(); ++it)
    {
        const TaskResults& results = it->second;
        std::cout << "\"" << it->first << "\";";

        for (TaskResults::const_iterator it2 = results.begin();
             it2 != results.end(); ++it2)
        {
            std::cout << it2->first << ";";
        }

        std::cout << std::endl;

        std::cout << "kB/s;";

        for (TaskResults::const_iterator it2 = results.begin();
             it2 != results.end(); ++it2)
        {
            std::cout << (it2->first * 0.001) / it2->second << ";";
        }

        std::cout << std::endl;

        std::cout << "seconds;";

        for (TaskResults::const_iterator it2 = results.begin();
             it2 != results.end(); ++it2)
        {
            std::cout << it2->second << ";";
        }

        std::cout << std::endl;
        std::cout << std::endl;
    }
}
