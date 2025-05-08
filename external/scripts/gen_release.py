import os, json, shutil, subprocess

SEP = os.sep
CWD = os.getcwd()
CALL_DIR = os.getcwd()
RELEASE_VER = None

README = """
Sane Software Development Kit. A modular, lightweight runtime library

You can find the latest release and additional information at:
https://github.com/r3shape/SSDK

This library is distributed under the terms of the MIT license,
available in [LICENSE.txt](LICENSE.txt).

# Using this package
This package contains the `SSDK` dynamic library built for x64 Windows.

To use this package, simply replace an existing 64-bit SSDK.dll with the one included here.

# Development packages
If you're looking for packages containing headers and static archives, check for this package:
-  SSDK-dev-2025.0.1-mingw-w64.zip (compiled using mingw-w64)

Happy Coding!

Izaiyah Stokes (d34d0s.dev@gmail.com)
"""

LICENSE = """
MIT License

Copyright (c) 2025 izaiyah stokes

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""

def os_path(path: str = None) -> str:
    return None if path is None else path.replace("/", SEP).replace("\\", SEP).strip()

def fetch_files(extension: str, directory: str) -> list:
    matched = []
    for root, _, files in os.walk(directory):
        for f in files:
            if f.endswith(extension):
                matched.append(os.path.join(root, f))
    return matched

def fetch_files_multi(exts: list[str], directory: str) -> list:
    matched = []
    for ext in exts:
        matched += fetch_files(ext, directory)
    return matched

def gen_package(dir: str, dev: bool=False):
    dll_path = os.path.join("build", f"SSDK.dll")

    if not os.path.exists(dll_path):
        print("[-] Missing DLL build artifact:", dll_path)
        return
    
    build_dir = os.path.join(dir, "build")
    
    os.makedirs(build_dir, exist_ok=True)

    shutil.copyfile(dll_path, os.path.join(build_dir, f"SSDK.dll"))

    if dev:
        inc_dir = os.path.join(dir, os_path("include/SSDK"))
        os.makedirs(inc_dir, exist_ok=True)
        # Copy public headers
        for root, _, files in os.walk(os_path("include/SSDK")):
            for f in files:
                if f.endswith(".h"):
                    src = os.path.join(root, f)
                    rel = os.path.relpath(root, os_path("include/SSDK"))
                    dst_folder = os.path.join(inc_dir, rel)
                    os.makedirs(dst_folder, exist_ok=True)
                    shutil.copyfile(src, os.path.join(dst_folder, f))

    with open(os.path.join(dir, "README.md"), "w") as f:
        f.write(README.strip())

    with open(os.path.join(dir, "LICENSE.txt"), "w") as f:
        f.write(LICENSE.strip())

    with open(os.path.join(dir, "manifest.json"), "w") as mf:
        json.dump({
        "name": "SSDK",
        "version": RELEASE_VER,
        "type": "dll",
        "platform": "mingw-w64",
        "files": os.listdir(dir)
        }, mf, indent=4)

def gen_release() -> None:
    print("[*] Generating release...")

    # Build DLL first
    subprocess.call(["r3make", "-nf", "-v", "-t", "main"])

    # Get version from version file
    version_path = None
    for root, dirs, files in os.walk(CALL_DIR):
        if root.endswith("SSDK") and "version" in files:
            version_path = os.path.join(root, "version")
            break

    if not version_path:
        print("[-] Error: version file not found.")
        return

    with open(version_path, "r") as f:
        RELEASE_VER = f.read().strip()

    release_base = os_path(f"{CALL_DIR}/release")
    std_dir = os_path(f"{release_base}/SSDK-{RELEASE_VER}-mingw-w64")
    dev_dir = os_path(f"{release_base}/SSDK-{RELEASE_VER}-dev-mingw-w64")

    os.makedirs(std_dir, exist_ok=True)
    os.makedirs(dev_dir, exist_ok=True)

    # === Std release ===
    print("[+] Creating standard release...")
    gen_package(std_dir)
    
    # === Dev release ===
    print("[+] Creating dev release...")
    gen_package(dev_dir, 1)
    

    print(f"[✓] Std release generated at: {std_dir}")
    print(f"[✓] Dev release generated at: {dev_dir}")

if __name__ == "__main__":
    gen_release()