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
class DataBuffer
{
    public:
        enum MemFlags
        {
            Read = 1,
            Write = 2,
            ReadWrite = Read | Write
        };

        enum LockState
        {
            Unlocked = 0,
            ReadLocked = 1,
            WriteLocked = 2,
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
        DataBufferWriteLock<T> lockWrite();

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
            return mData[idx];
        }

        inline const T& operator[](const size_t idx) const
        {
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
