oclcrypto
==============
A reusable C++11 library for OpenCL hardware accelerated cryptography.

Compilation on Linux
--------------------

### Build dependencies (Fedora)

```bash
# if you do not have build tools installed already
yum install @development-tools

yum install cmake boost-devel ocl-icd-devel opencl-headers
```
(Replace yum with dnf if you use Fedora 22 or newer.)

### Build dependencies (Debian or Ubuntu)

```bash
# if you do not have build tools installed already
apt-get install build-essential

apt-get install cmake libboost-all-dev ocl-icd-opencl-dev opencl-headers
```

### Configure step
Feel free to replace *build/* with a build directory of your choice. Using the source directory is discouraged and may not work, only out-of-source builds are supported.

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

### Install step
```bash
cd build/
sudo make install
```

The command above will install headers into **$prefix/include/oclcrypto** and libraries into **$prefix/lib(64)**. There is no automatic uninstall step. You are recomended to use the install step while packaging.

### Running the unit tests
```bash
cd build/
ctest -V
```

Compilation on Windows
--------------------

### Build dependencies
Windows unfortunately lacks a well supported package manager, all dependencies have to be downloaded manually.

- Microsoft Visual Studio - version 2013 is recommended - https://www.visualstudio.com/
- cmake 2.6+ and cmake-gui - http://www.cmake.org/download/
- Boost libraries precompiled for Windows - http://www.boost.org/doc/libs/release/more/getting_started/windows.html
- NVIDIA CUDA Toolkit 5.0+ - https://developer.nvidia.com/cuda-toolkit

### Configure step
Run cmake-gui, select the repository as the source directory and select directory of your choice as the build directory. Selecting source directory as the build directory is discouraged and unsupported, do an out-of-source build instead.

In cmake-gui, select Configure, walk through the wizard dialogs and then click Generate. Visual Studio solution and project files will be generated inside the build folder.

### Build step
Open the Visual Studio solution, select a desirable configuration - Debug or Release - and click Build.

### Install step
It is not customary to run the install step on Windows system. Instead, it is recommended to bundle the built DLL files with your application.

### Running the unit tests
Double-clicking oclcrypto-tests will work but the output will be lost after tests finish. We therefore recommend running the tests from a terminal emulator of your choice. **cmd.exe** will also work.

```bash
cd build
oclcrypto-tests.exe
```

Using the API
-------------
Browse the *tests/* folder for sample usage of the API.
