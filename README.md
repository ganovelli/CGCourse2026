# CGCourse2026
repository del corso di Computer Grafica A.A. 25/26- Informatica - Università di pisa

# Build Instructions

This project uses **CMake** to ensure it builds consistently across different
operating systems and IDEs.

Only **CMake** and a **C++ compiler** are required.

---

## Requirements

- **CMake ≥ 3.16**
- A C++17-compatible compiler
  - Windows: Visual Studio
  - Linux: GCC or Clang
  - macOS: Xcode Command Line Tools

---

## Installing CMake

### Windows

**Recommended:** install Visual Studio with  
✔ *Desktop development with C++*

CMake is included automatically.

Alternatively, download CMake from:  
https://cmake.org/download/

After installation, verify:

```bat
cmake --version
```

---

### Linux (Ubuntu / Debian)

```bash
sudo apt update
sudo apt install cmake
```

Verify:

```bash
cmake --version
```

---

### macOS

Using Homebrew:

```bash
brew install cmake
```

Verify:

```bash
cmake --version
```

---

## Building the Project

From the **root directory** of the project:

```bash
cmake -S . -B build
cmake --build build
```

This will:
- configure the project
- compile all examples
- place all build files in the `build/` directory

---

## Notes for Windows Users

- If using the command line, open:
  **“x64 Native Tools Command Prompt for Visual Studio”**
- Alternatively, open the project directly in Visual Studio using:
  **File → Open → Folder**

Do not mix different terminals for the same build directory.

---

## Cleaning the Build

To start from a clean state, delete the `build/` directory and rerun CMake.

---
