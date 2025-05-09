# ![alt text](https://github.com/r3shape/swarm/raw/main/koncept/assets/r3-sticker.png) Sane Software Development Kit 
| **A modular, lightweight runtime library.**


**SSDK** is designed to be minimal, extensible, and easy to integrate—ideal for custom runtimes, low-level tools, and systems-level development.

---
## Features

- **Modular architecture** – Initialize only the APIs you need
- **No platform dependencies** – Fully self-contained, cross-platform C
- **Runtime metadata** – Track module size, version, function calls, and usage stats

---

## API Overview
| API     | Description                           |
|---------|---------------------------------------|
| `SaneMemory`  | Memory management and arenas          |
| `SaneData`    | Core data structures (arrays, maps)   |
| `SaneLog`     | Logging tools utilities               |
| `SaneECS`     | Entity-Component-System runtime       |
| `SaneMath`    | Math utilities (vectors, transforms)  |
| `SaneFile`    | File I/O and stream management        |

| <b>NOTE:</b> Each module is independently initialized and tracked via runtime metadata.

---

## Getting Started

### 1. Choose a Release

| Release       | Contents                                                                 |
|---------------|--------------------------------------------------------------------------|
| `SSDK-dev`   | Full development version: headers, and prebuilt `.dll`    |
| `SSDK`       | Standard release: single-header (`SSDK.h`) + prebuilt `.dll`            |

| <b>NOTE:</b> Both versions are available as `.zip` and `.tar` archives.

---

### 2. Development Integration

Use the `SSDK-dev` release if you want access to all headers, and prebuilt binaries:

- Add SSDK's `include` directory to your compiler's header search path
- Link against the provided `.dll` build artifact.
- Use `SSDK.h` to bootstrap the library and access the full API

---

### 3. Write Some Code
```c
#include "SSDK.h"

int main() {
  ssdkInitMemory();
    ssdkInitData(); // dependent on SaneMemory
    ssdkInitLog();
    // your code...
    ssdkExitLog();
    ssdkExitData();
    ssdkExitMemory(); // must be exited after dependent modules
    return 0;
}
```

---

## Building From Source

### Option 1: Use your preferred build system or a minimal setup:

```bash
gcc -shared -o SSDK.dll src/SSDK/*.c
```

### Option 2: The build process can be automated using [`r3make`](https://github.com/r3shape/r3make).

```bash
r3make -nf -v
```
| <b>NOTE:</b> This command builds the SSDK library

```bash
r3make -nf -v -be -r -t tests
```
| <b>NOTE:</b> This command builds, and runs the SSDK library tests

---

## SSDK Contributors

<a href="https://github.com/r3shape/SSDK/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=r3shape/SSDK" />
</a>

## License

SSDK is released under the MIT License.
