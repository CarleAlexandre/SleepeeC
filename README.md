# Sleepee C

___

A lightweight, fast, and portable build tool for C/C++ projects. This tool aims to be much simpler and more intuitive than CMake, while offering powerful features like wildcard-based source discovery and ignore rules.

## Why

 - i have a an issue with how we build project in c, i want it to be faster and more streamlined

## How

 - i want to make a tool that come in cli and gui that just by specifiying very basic things about your project just compile it
 nothing more nothing else

 - like just say, i use this compiler i use these flag on x or y os
 and your project structure with release debug and library compilation mode, everything else is done bye initializeing that create a config file and running

 - i would like for it to also enforce certain constrainte for best practice (i will make it a switch for people that don't want them).

### WARNING

	i use IA to create a draft template of this README, for now what is after is ia slope don't refere to it for now

---

## ✨ Features

- **Multiple Targets**: Easily build executables, static libraries, or shared libraries.
- **Glob Support**: Use patterns like `**/*.cpp` to automatically include source files.
- **Ignore Support**: Exclude files or directories from builds using a `.buildignore` file.
- **Single Config File**: All settings are managed in a straightforward `build.toml` file.
- **Cross-Platform**: Built with C++17 and standard libraries to work on any major OS.

---

## Project Structure

```
project/
├── build.toml
├── .buildignore
├── src/
│   └── main.cpp
├── lib/
│   └── math.cpp
└── builder/
    └── main.cpp  # build tool source
```

---

## Getting Started

### 1. Define Your Build Configuration
Create a `build.toml` file in your project root:

```toml
compiler = "g++"
cflags = "-Wall -O2"
debug = true

[[target]]
name = "app"
type = "executable"
sources = ["src/**/*.cpp"]

[[target]]
name = "math"
type = "static"
sources = ["lib/**/*.cpp"]
```

### 2. Define Files to Ignore
Create a `.buildignore` file to exclude unwanted files:

```
**/tests/**
*.test.cpp
```

### 3. Run the Build Tool
```bash
./builder
```

---

## Building

```bash
g++ -std=c++17 -Ipath/to/toml++ -o builder main.cpp
```

---

## 📜 License

MIT License — free for personal and commercial use.

---
