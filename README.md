icsograph
=======================================

About
---------------------------------------

This program is a designer for knotian knotworks, using the mathematical
correlation between knots and graphs and more.

The software is under development therefore some features may change without notice and there may be some bugs.

Contacts
---------------------------------------

frédéric fichant <fredfichant@gmail.com>
[icsograph Website](https://webknot.web.app/)

If you have any suggestion, criticism, feature request or bug report please let me know.

License
---------------------------------------

GPLv3 or later, see COPYING.

Dependencies
---------------------------------------

* Qt Framework, should work with 4.7 or later (Qt 5.15+ recommended)
* CMake 3.5 or later
* C++ compiler (MSVC 2019+ on Windows, Clang on macOS, GCC on Linux)

Getting the latest sources
---------------------------------------

```bash
git clone https://github.com/fredfichant/icsograf.git
cd icsograf
```

Installation and Build Instructions
---------------------------------------

### Windows

1. **Install Prerequisites**
   - Visual Studio Community (with C++ tools): https://visualstudio.microsoft.com/downloads/
   - CMake (3.5+): https://cmake.org/download/
   - Qt Framework (5.15+): https://www.qt.io/download

2. **Configure and Build**
   ```powershell
   # Set Qt path (adjust to your Qt installation)
   $QtPath = "C:\Qt\5.15.2\msvc2019_64"
   
   # Configure
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$QtPath"
   
   # Build
   cmake --build build --config Release -j2
   ```

3. **Run the Application**
   ```powershell
   .\build\bin\Release\icsograf.exe
   ```

4. **Package (Optional)**
   ```powershell
   cd build
   cpack -G ZIP
   ```

### macOS

1. **Install Prerequisites**
   ```bash
   # Using Homebrew
   brew install cmake qt@5
   ```

2. **Configure and Build**
   ```bash
   # Configure with Qt5 from Homebrew
   CMAKE_PREFIX_PATH=$(brew --prefix qt@5) cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
   
   # Build
   cmake --build build -j$(sysctl -n hw.ncpu)
   ```

3. **Run the Application**
   ```bash
   ./build/bin/icsograf
   ```

4. **Package (Optional - creates DMG)**
   ```bash
   cd build
   cpack -G DragNDrop
   # Creates: icsograf-1.0.0-Darwin.dmg
   ```

### Linux

1. **Install Prerequisites**

   **Ubuntu/Debian:**
   ```bash
   sudo apt-get update
   sudo apt-get install -y build-essential cmake \
     qtbase5-dev qttools5-dev qttools5-dev-tools \
     libqt5svg5-dev libqt5printsupport5 libqt5sql5-sqlite
   ```

   **Fedora/RedHat:**
   ```bash
   sudo dnf install gcc-c++ cmake qt5-qtbase-devel qt5-qttools-devel \
     qt5-qtsvg-devel qt5-qtsql
   ```

   **Arch Linux:**
   ```bash
   sudo pacman -S base-devel cmake qt5-base qt5-tools qt5-svg
   ```

2. **Configure and Build**
   ```bash
   # Configure
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
   
   # Build
   cmake --build build -j$(nproc)
   ```

3. **Run the Application**
   ```bash
   ./build/bin/icsograf
   ```

4. **Install System-wide (Optional)**
   ```bash
   cd build
   sudo cmake --install .
   # Now you can run: icsograf
   ```

5. **Package (Optional - creates DEB, RPM, or TGZ)**
   ```bash
   cd build
   cpack
   # or specific format:
   # cpack -G DEB   # Debian package
   # cpack -G RPM   # RPM package
   # cpack -G TGZ   # Tarball
   ```

Quick build (all platforms)
---------------------------------------

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j1
```
