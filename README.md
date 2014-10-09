oclcrypto
==============
*This project is work in progress, I don't recommend it for any use beyond testing.*

A reusable C++11 library for OpenCL hardware accelerated cryptography.

Compilation
-----------

### Build dependencies (Fedora)

```bash
yum install cmake boost-devel ocl-icd-devel opencl-headers
```

### Configure step
```bash
mkdir build
cd build/
cmake ../
```

### Build step
```bash
cd build/
make
```

Running the unit tests
----------------------

```bash
cd build/
ctest -V
```
