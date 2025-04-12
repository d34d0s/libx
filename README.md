# ![alt text](https://private-user-images.githubusercontent.com/118138305/414014542-ac634f13-e084-4387-aded-4679eb048cac.png?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3NDQ0OTM2MTcsIm5iZiI6MTc0NDQ5MzMxNywicGF0aCI6Ii8xMTgxMzgzMDUvNDE0MDE0NTQyLWFjNjM0ZjEzLWUwODQtNDM4Ny1hZGVkLTQ2NzllYjA0OGNhYy5wbmc_WC1BbXotQWxnb3JpdGhtPUFXUzQtSE1BQy1TSEEyNTYmWC1BbXotQ3JlZGVudGlhbD1BS0lBVkNPRFlMU0E1M1BRSzRaQSUyRjIwMjUwNDEyJTJGdXMtZWFzdC0xJTJGczMlMkZhd3M0X3JlcXVlc3QmWC1BbXotRGF0ZT0yMDI1MDQxMlQyMTI4MzdaJlgtQW16LUV4cGlyZXM9MzAwJlgtQW16LVNpZ25hdHVyZT0xNWJjOWFmZjNmNThiMGNmZWJlNTA2ZTdkN2JlY2ZjYjI5MjI3NzgwZDRmYzI1ZDY3MWUxZDdhZDI5ZDcwYTg5JlgtQW16LVNpZ25lZEhlYWRlcnM9aG9zdCJ9.a3V0mJyNyS9I5txtUIdeZTDZr9is8euemHsPA-iGpW8) Corex 

**Corex** is a modular, lightweight runtime library for C projects. It provides a suite of foundational APIs for memory management, data structures, generic utilities, entity-component systems, math operations, and file I/O.

**Corex** is designed to be minimal, extensible, and easy to integrate—ideal for custom runtimes, low-level tools, and systems-level development.

---
## Features

- **Modular architecture** – Initialize only the APIs you need via configuration flags
- **Drop-in compatible** – Includes a single-header version for fast integration
- **Runtime metadata** – Track version, usage stats, and build information
- **No platform dependencies** – Fully self-contained, cross-platform C

---

## Getting Started

### 1. Choose a Release

| Release       | Contents                                                                 |
|---------------|--------------------------------------------------------------------------|
| `corex-dev`   | Full development version: headers, and prebuilt `.dll`/`.lib`    |
| `corex`       | Standard release: single-header (`corex.h`) + prebuilt `.dll`            |

Both versions are available as `.zip` and `.tar` archives.

---

### 2. Integration

#### Option A: Drop-in Header

The `corex` release includes a standalone `corex.h` and `corex.dll` that can be integrated directly into any project:

```c
#include "corex.h"

int main() {
    libx_init(COREX_USE_MEMX | COREX_USE_GENX);
    // your code...
    return 0;
}
```

#### Option B: Development Integration

Use the `corex-dev` release if you want access to full source code, headers, and prebuilt binaries:

- Add `corex/` to your compiler's header search path
- Link against the provided `.lib` or `.dll` build artifact.
- Use `corex.h` to bootstrap the library and access the full API

---

## Building From Source

Use your preferred build system or a minimal setup:

```bash
gcc -Iinclude -shared -o corex.dll src/*.c
```

Alternatively, using the provided configuration, the build process can be automated using [`r3make`](https://github.com/r3shape/r3make).

---

## API Overview

Corex currently provides 6 APIs:

| API     | Description                           |
|---------|---------------------------------------|
| `Memx`  | Memory management and arenas          |
| `Dsx`   | Core data structures (arrays, maps)   |
| `Genx`  | Generic tools and buffer utilities    |
| `Ecsx`  | Entity-Component-System runtime       |
| `Mathx` | Math utilities (vectors, transforms)  |
| `Filex` | File I/O and stream management        |

Each module is independently initialized and tracked via runtime metadata.

---

## Corex Contributors

<a href="https://github.com/r3shape/corex/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=r3shape/corex" />
</a>

## License

Corex is released under the MIT License.
