# Star-shaped-Cpp-Master

star-shaped cpp data collecting and organizing: assume data is from stm32 slaves and is already processed by a hardware interface; use gtest to simulated that data; code cpp class that will retrieve context from data and assign it to a struct

# CMake
https://cmake.org/download/
zip; extract to User directory

add to Path:
```bash
# Replace with your actual CMake path
$cmakePath = "\\rz-home\arm1259\Download\Apps\cmake-4.2.1-windows-x86_64\bin"

# Add to user PATH
[Environment]::SetEnvironmentVariable("Path", $env:Path + ";$cmakePath", "User")

# Verify it was added
$env:Path = [System.Environment]::GetEnvironmentVariable("Path","User")
```

cmake --version


### subdirectory Cmakelist: /tests/cmakelists
we separate tests cmakelist from the main one not to compile extra files in production stage


# CMake Generator

*CMake is a "meta build system"*. It doesn't compile your code directly, but instead creates the needed files to use another more "native" build system without you having to worry about manually implementing it for each platform.
On Unix (Linux) this tends to be a good ol' Makefile. On Windows it's usually a Visual Studio (NOT VS Code) project. On macOS, an Xcode project.
These aren't the only options though. If you use MSYS2/MinGW you can also use Makefiles on Windows, and CMake can generate a MinGW makefile. Another popular cross platform build system it can generate for is Ninja.


Generators create build files for different build systems; the compilers (MSVC, GCC, Clang) are used by those generators


! check how it works in PC (stack): doesnt work in desktop



# GTest
- generate mock vectors-buffers comming from hardware_interface reading data from slaves via kernel
- TEST() - defines a test case
- TEST_F() - defines a test case with a fixture class
- EXPECT_EQ() - asserts two values are equal
- EXPECT_NE() - asserts two values are not equal
- EXPECT_TRUE() - asserts condition is true
- EXPECT_FALSE() - asserts condition is false
- EXPECT_LT() - asserts first value is less than second
- EXPECT_GT() - asserts first value is greater than second
- EXPECT_FLOAT_EQ() - asserts two float values are approximately equal
- ASSERT_EQ() - fatal assertion (stops test on failure)
- SetUp() - fixture method called before each test
- TearDown() - fixture method called after each test

