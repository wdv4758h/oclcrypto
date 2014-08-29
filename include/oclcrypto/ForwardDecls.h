/*
 * Copyright (C) 2014 Martin Preisler <martin@preisler.me>
 *
 * This file is part of oclcrypto.
 *
 * oclcrypto is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 */

#ifndef OCLCRYPTO_FORWARD_DECLS_H_
#define OCLCRYPTO_FORWARD_DECLS_H_

#include "oclcrypto/Config.h"

#include <vector>
#include <memory>

namespace oclcrypto
{

class Device;
class DeviceAllocation;
class DeviceManager;
class Task;

typedef std::vector<char> Buffer;

typedef std::shared_ptr<DeviceAllocation*> DeviceAllocationPtr;
typedef std::shared_ptr<Buffer> BufferPtr;
typedef std::shared_ptr<const Buffer> ConstBufferPtr;

}

#endif
