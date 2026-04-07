# LiteEngine

A lightweight C++ game engine built on top of OpenGL, designed as a learning platform and foundation for real-time 2D/3D applications. The engine handles windowing, input, rendering, and UI — so you can focus on building things with it. A fully playable **Breakout game** is already included in the Sandbox project as a working example.

---

## Table of Contents

- [Features](#features)
- [Repository Structure](#repository-structure)
- [Prerequisites](#prerequisites)
- [Building the Project](#building-the-project)
  - [Step 1 — Clone the repository](#step-1--clone-the-repository)
  - [Step 2 — Generate Visual Studio project files](#step-2--generate-visual-studio-project-files)
  - [Step 3 — Open the solution in Visual Studio](#step-3--open-the-solution-in-visual-studio)
  - [Step 4 — Set the startup project](#step-4--set-the-startup-project)
  - [Step 5 — Select a build configuration](#step-5--select-a-build-configuration)
  - [Step 6 — Build and run](#step-6--build-and-run)
- [Build Configurations](#build-configurations)
- [Output Directory Layout](#output-directory-layout)
- [Dependencies](#dependencies)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)

---

## Features

- **OpenGL rendering** via Glad (GL loader) and GLFW (windowing & input)
- **Dear ImGui integration** for runtime debug UI and tooling panels
- **GLM** for math — vectors, matrices, and transforms
- **spdlog** for fast, levelled logging across the engine and application
- **Precompiled headers** (`hzpch.h`) for faster incremental build times
- **Sandbox project** for quick iteration and experimentation — includes a fully playable **Breakout game** built using the engine
- Three build configurations: `Debug`, `Release`, and `Dist`
- Premake5 vendored in the repo — no separate install required

---

## Repository Structure

```
LiteEngine/
├── LiteEngine/                  # Core engine — compiled as a static library
│   ├── src/                     # Engine source files and headers
│   │   └── hzpch.h / hzpch.cpp  # Precompiled header
│   └── vendor/                  # All third-party dependencies
│       ├── GLFW/                # Windowing and input
│       ├── Glad/                # OpenGL function loader
│       ├── imgui/               # Dear ImGui (git submodule)
│       ├── glm/                 # Math library
│       ├── spdlog/              # Logging library (git submodule)
│       └── bin/premake/         # Vendored Premake5 binary
│           └── premake5.exe
├── Sandbox/                     # Demo application — contains the Breakout game
│   └── src/
├── .gitmodules                  # Submodule config (spdlog, imgui)
├── GenerateProjects.bat         # One-click project generation script (Windows)
├── premake5.lua                 # Premake build configuration
└── README.md
```

---

## Prerequisites

Before you begin, make sure you have the following installed:

| Requirement | Notes |
|---|---|
| **Windows 10/11** | Only Windows is currently supported |
| **Visual Studio 2022** (or 2019) | Make sure the **Desktop development with C++** workload is installed |
| **Git** | Required to clone and initialise submodules |

> **Note:** Premake5 is already vendored in the repository at `LiteEngine/vendor/bin/premake/premake5.exe`. You do **not** need to install it separately.

---

## Building the Project

### Step 1 — Clone the repository

You must clone **recursively** to pull in the git submodules (spdlog and imgui). Without `--recursive`, the vendor folders for these libraries will be empty and the build will fail.

```bash
git clone --recursive https://github.com/Shishir-Hegde/LiteEngine.git
cd LiteEngine
```

If you already cloned without `--recursive`, initialise the submodules manually:

```bash
git submodule update --init --recursive
```

---

### Step 2 — Generate Visual Studio project files

Run the provided batch script from the root of the repository:

```bat
GenerateProjects.bat
```

This calls the vendored Premake5 binary and targets Visual Studio 2022:

```bat
call vendor\bin\premake\premake5.exe vs2022
```

After this completes, you will see a `LiteEngine.sln` file in the root directory, along with `.vcxproj` files inside the `LiteEngine/` and `Sandbox/` folders.

> **If you are using Visual Studio 2019**, run Premake manually instead:
> ```bat
> vendor\bin\premake\premake5.exe vs2019
> ```

---

### Step 3 — Open the solution in Visual Studio

Open `LiteEngine.sln` in Visual Studio. The solution contains two projects:

- **LiteEngine** — the core engine, compiled as a static library (`.lib`)
- **Sandbox** — the demo application that links against LiteEngine and contains the Breakout game

---

### Step 4 — Set the startup project

By default, Visual Studio may set `LiteEngine` as the startup project. Since `LiteEngine` is a static library (not an executable), you need to change this before you can run anything:

1. In the **Solution Explorer**, right-click on **Sandbox**
2. Select **Set as Startup Project**

---

### Step 5 — Select a build configuration

In the Visual Studio toolbar, choose your desired configuration from the dropdown (next to the green Run button):

- `Debug` — recommended for first-time setup and development
- `Release` — optimised, for testing performance
- `Dist` — stripped distribution build

---

### Step 6 — Build and run

Press **Ctrl+Shift+B** (or go to **Build → Build Solution**) to compile both `LiteEngine` and `Sandbox`.

Then press **F5** (or click the green **Local Windows Debugger** button) to launch the Sandbox and run the Breakout game.

---

## Build Configurations

| Configuration | Defines | Optimisation | Use case |
|---|---|---|---|
| `Debug` | `LE_DEBUG` | Off | Development — full symbols, assertions enabled |
| `Release` | `LE_RELEASE` | On | Testing — optimised but still debuggable |
| `Dist` | `LE_DIST` | On | Distribution — stripped, production-ready build |

---

## Output Directory Layout

Binaries are placed in:

```
bin/<Config>-<OS>-<Architecture>/<ProjectName>/
```

For example, a Debug build on Windows x64 outputs to:

```
bin/Debug-windows-x86_64/Sandbox/Sandbox.exe
bin/Debug-windows-x86_64/LiteEngine/LiteEngine.lib
```

Intermediate object files go to:

```
bin-int/<Config>-<OS>-<Architecture>/<ProjectName>/
```

---

## Dependencies

All dependencies are vendored inside `LiteEngine/vendor/` and are built as part of the solution — no package manager required.

| Library | Source | Purpose |
|---|---|---|
| [GLFW](https://www.glfw.org/) | Vendored source | Window creation, OpenGL context, input events |
| [Glad](https://glad.dav1d.de/) | Vendored source | OpenGL function loader |
| [Dear ImGui](https://github.com/TheCherno/imgui) | Git submodule (TheCherno fork) | Immediate mode debug/editor UI |
| [GLM](https://github.com/g-truc/glm) | Vendored headers | Vectors, matrices, transforms |
| [spdlog](https://github.com/gabime/spdlog) | Git submodule | Fast header-only logging |
| [Premake5](https://premake.github.io/) | Vendored binary | Build system / project file generator |

---

## Troubleshooting

**Submodule folders are empty (imgui or spdlog)**
```bash
git submodule update --init --recursive
```

**`LiteEngine.sln` not found after cloning**
The solution file is not committed — it is generated by Premake. Run `GenerateProjects.bat` first.

**Build errors about missing headers**
Make sure you are building the whole solution (`Ctrl+Shift+B`), not just a single project. `LiteEngine` must be compiled before `Sandbox`.

**LNK4006 linker warnings**
These are expected and harmless. They occur because `__NULL_IMPORT_DESCRIPTOR` is defined in both `opengl32.lib` and `dwmapi.lib`. The build is already configured to suppress these with `/ignore:4006`.

**`premake5.exe` not found or access denied**
Ensure you are running `GenerateProjects.bat` from the **root of the repository**, not from inside a subdirectory.

---

## Contributing

Pull requests and issues are welcome. For significant changes, please open an issue first to discuss what you'd like to change or add.

---

## License

This project is unlicensed. See the repository for details.
