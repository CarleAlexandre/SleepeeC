# Sleepee C

___

A lightweight, fast, and portable build tool for C/C++ projects. This tool aims to be much simpler and more intuitive than CMake, while offering powerful features like wildcard-based source discovery and ignore rules.

I also at some point would like to add a way to make it faster and less complicated to compile code

## Why

 - i have a an issue with how we build project in c, i want it to be faster and more streamlined

## How

 - i want to make a tool that come in cli and gui that just by specifiying very basic things about your project just compile it
 nothing more nothing else

 - like just say, i use this compiler i use these flag on x or y os
 and your project structure with release debug and library compilation mode, everything else is done bye initializeing that create a config file and running

 - i would like for it to also enforce certain constrainte for best practice (i will make it a switch for people that don't want them).

---

## ✨ Features

- **Multiple Targets**: Easily build executables, static libraries, or shared libraries.
- **Glob Support**: Use patterns like `**/*.c` to automatically include source files.
- **Ignore Support**: Exclude files or directories from builds using a `sc.ignore` file.
- **Single Config File**: All settings are managed in a straightforward `sc.toml` file.

---

## Exemple Project Structure

```
project/
├── sc.toml #could also be sleepeec.toml
├── sc.ignore #could also be sleepeec.ignore
├── src/
│   └── main.c
└── lib/
    └── math.h
```

---

## Getting Started

### 1. Define Your Build Configuration
Create a `build.toml` file in your project root:

```toml
compiler = "gcc"
cflags = "-Wall -O2"
debug = true

[[target]]
name = "app"
type = "executable"
sources = ["src/**/*.c"]

[[target]]
name = "math"
type = "static"
sources = ["lib/**/*.c"]
```

### 2. Define Files to Ignore
Create a `.buildignore` file to exclude unwanted files:

```
**/tests/**
*.test.c
```

### 3. Run the Build Tool
```bash
./builder
```

---

## Building

```bash
make
```
 or
```bash
gmake
```

---

## 📜 License

MIT License — free for personal and commercial use.

---
