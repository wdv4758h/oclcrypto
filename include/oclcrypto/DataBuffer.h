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

        enum LockState
        {
            Unlocked = 1 << 0,
            ReadLocked = 1 << 1,
            WriteLocked = 1 << 2,
            ReadWriteLocked = ReadLocked | WriteLocked
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

        LockState getLockState() const;

        template<typename T>
        DataBufferReadLock<T> lockRead();

        template<typename T>
        DataBufferWriteLock<T> lockWrite();

        void lockAndReadRawData(void *data);
        void unlockAndWriteRawData(void* data, LockState expectedLockState);
        void discardLock(LockState expectedLockState);

        // noncopyable
        DataBuffer(const DataBuffer&) = delete;
        DataBuffer& operator=(const DataBuffer&) = delete;

    private:
        Device& mDevice;
        const size_t mSize;
        const unsigned short mMemFlags;

        LockState mLockState;

        cl_mem mCLMem;
};

template<typename T>
class DataBufferReadLock
{
    public:
        inline DataBufferReadLock(DataBuffer& buffer, T* data):
            mBuffer(buffer),
            mData(data)
        {}

        inline ~DataBufferReadLock()
        {
            if (mData)
                unlock();
        }

        inline const T& operator[](const size_t idx) const
        {
            if (!mData)
                throw std::runtime_error("This read lock has already been unlocked!");

            if (idx >= mBuffer.getArraySize<T>())
                throw std::out_of_range("Index out of bounds");

            return mData[idx];
        }

        inline void unlock()
        {
            if (!mData)
                throw std::runtime_error(
                    "This DataBufferReadLock has already been unlocked, "
                    "you can't unlock it twice!"
                );

            mBuffer.discardLock(DataBuffer::ReadLocked);
            delete [] mData;
            mData = nullptr;
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
    T* data = new T[getArraySize<T>()];
    lockAndReadRawData(data);
    return DataBufferReadLock<T>(*this, data);
}

template<typename T>
class DataBufferWriteLock
{
    public:
        inline DataBufferWriteLock(DataBuffer& buffer, T* data):
            mBuffer(buffer),
            mData(data),
            mFlushed(false)
        {}

        inline ~DataBufferWriteLock()
        {
            if (!mFlushed)
                flush();

            delete [] mData;
        }

        inline T& operator[](const size_t idx)
        {
            if (idx >= mBuffer.getArraySize<T>())
                throw std::out_of_range("Index out of bounds");

            return mData[idx];
        }

        inline const T& operator[](const size_t idx) const
        {
            if (idx >= mBuffer.getArraySize<T>())
                throw std::out_of_range("Index out of bounds");

            return mData[idx];
        }

        inline void flush()
        {
            if (mFlushed)
                throw std::runtime_error(
                    "This DataBufferWriteLock has already been flushed, "
                    "you can't flush it twice!"
                );

            mBuffer.unlockAndWriteRawData(mData, DataBuffer::WriteLocked);
            mFlushed = true;
        }

        inline void discard()
        {
            if (mFlushed)
                throw std::runtime_error(
                    "This DataBufferWriteLock has already been flushed or discarded, "
                    "you can't discard it any more!"
                );

            mBuffer.discardLock(DataBuffer::WriteLocked);
            mFlushed = true;
        }

        // noncopyable
        // but move constructible!
        DataBufferWriteLock(DataBufferWriteLock&& other):
            mBuffer(other.mBuffer),
            mData(other.mData),
            mFlushed(other.mFlushed)
        {}

        DataBufferWriteLock(const DataBufferWriteLock&) = delete;
        DataBufferWriteLock& operator=(const DataBufferWriteLock&) = delete;

    private:
        DataBuffer& mBuffer;
        T* mData;
        bool mFlushed;
};

template<typename T>
DataBufferWriteLock<T> DataBuffer::lockWrite()
{
    if (mLockState != Unlocked)
        throw std::runtime_error("This DataBuffer has already been locked!");

    mLockState = WriteLocked;

    T* data = new T[getArraySize<T>()];
    return DataBufferWriteLock<T>(*this, data);
}

}

#endif
