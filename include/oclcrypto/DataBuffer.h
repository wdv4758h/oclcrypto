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

#ifndef OCLCRYPTO_DATA_BUFFER_H_
#define OCLCRYPTO_DATA_BUFFER_H_

#include "oclcrypto/ForwardDecls.h"
#include <CL/cl.h>
#include <string>

namespace oclcrypto
{

/**
 * @brief Represents memory on an OpenCL device
 */
class OCLCRYPTO_EXPORT DataBuffer
{
    public:
        enum MemFlags
        {
            Read = 1 << 0,
            Write = 1 << 1,
            ReadWrite = Read | Write
        };

        /**
         * @param size Size in bytes
         */
        DataBuffer(Device& device, const size_t size, unsigned short memFlags = ReadWrite);

        ~DataBuffer();

        Device& getDevice() const;

        inline size_t getSize() const
        {
            return mSize;
        }

        cl_mem getCLMem() const;

        const cl_mem* getCLMemPtr() const;

        template<typename T>
        inline size_t getArraySize() const
        {
            const size_t size = getSize();
            const size_t elementSize = sizeof(T);

            if (size % elementSize != 0)
                throw std::runtime_error(
                    std::string("This DataBuffer can't represent an array of '") +
                    typeid(T).name() + "' because " + std::to_string(elementSize) +
                    " doesn't divide " + std::to_string(size) + ".");

            return size / elementSize;
        }

        void* mapForReading();
        void unmapForReading(void* buffer);
        void* mapForWriting();
        void unmapForWriting(void* buffer);

        template<typename T>
        DataBufferReadLock<T> lockRead();

        template<typename T>
        DataBufferWriteLock<T> lockWrite();

        // noncopyable
        DataBuffer(const DataBuffer&) = delete;
        DataBuffer& operator=(const DataBuffer&) = delete;

    private:
        Device& mDevice;
        const size_t mSize;
        const unsigned short mMemFlags;

        cl_mem mCLMem;
};

template<typename T>
class DataBufferReadLock
{
    public:
        inline DataBufferReadLock(DataBuffer& buffer):
            mBuffer(buffer),
            mData(nullptr)
        {
            mData = reinterpret_cast<T*>(mBuffer.mapForReading());
        }

        inline ~DataBufferReadLock()
        {
            mBuffer.unmapForReading(mData);
        }

        inline size_t size() const
        {
            return mBuffer.getArraySize<T>();
        }

        inline const T& operator[](const size_t idx) const
        {
#ifndef NDEBUG
            if (idx >= size())
                throw std::out_of_range("Index out of bounds");
#endif

            return mData[idx];
        }

        // noncopyable
        // but move constructible!
        DataBufferReadLock(DataBufferReadLock&& other):
            mBuffer(other.mBuffer),
            mData(other.mData)
        {}

        DataBufferReadLock(const DataBufferReadLock&) = delete;
        DataBufferReadLock& operator=(const DataBufferReadLock&) = delete;

    private:
        DataBuffer& mBuffer;
        T* mData;
};

template<typename T>
DataBufferReadLock<T> DataBuffer::lockRead()
{
    return DataBufferReadLock<T>(*this);
}

template<typename T>
class DataBufferWriteLock
{
    public:
        inline DataBufferWriteLock(DataBuffer& buffer):
            mBuffer(buffer),
            mData(nullptr)
        {
            mData = reinterpret_cast<T*>(mBuffer.mapForWriting());
        }

        inline ~DataBufferWriteLock()
        {
            mBuffer.unmapForWriting(mData);
        }

        inline T& operator[](const size_t idx)
        {
#ifndef NDEBUG
            if (idx >= mBuffer.getArraySize<T>())
                throw std::out_of_range("Index out of bounds");
#endif

            return mData[idx];
        }

        inline const T& operator[](const size_t idx) const
        {
#ifndef NDEBUG
            if (idx >= mBuffer.getArraySize<T>())
                throw std::out_of_range("Index out of bounds");
#endif

            return mData[idx];
        }

        // noncopyable
        // but move constructible!
        DataBufferWriteLock(DataBufferWriteLock&& other):
            mBuffer(other.mBuffer),
            mData(other.mData)
        {}

        DataBufferWriteLock(const DataBufferWriteLock&) = delete;
        DataBufferWriteLock& operator=(const DataBufferWriteLock&) = delete;

    private:
        DataBuffer& mBuffer;
        T* mData;
};

template<typename T>
DataBufferWriteLock<T> DataBuffer::lockWrite()
{
    return DataBufferWriteLock<T>(*this);
}

}

#endif
