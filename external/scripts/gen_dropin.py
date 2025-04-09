import os
import re

# Config
MODULES_DIR = 'libx'  # Change this to your source root
OUTPUT_FILE = 'release/libx.h'
MODULE_ORDER = [
    'memx/libx_memx',
    'dsx/libx_dsx',
    'genx/libx_genx',
    'ecsx/libx_ecsx',
    'mathx/libx_mathx',
    'filex/libx_filex',
]


HEADER_GUARD = '__LIBX_H__'

def clean_includes(text):
    return re.sub(r'#include\s+"[^"]+"', '', text)

def strip_guard(text):
    # Remove #ifndef / #define / #endif from headers
    text = re.sub(r'#ifndef\s+\w+\n#define\s+\w+\n', '', text)
    text = re.sub(r'#endif\s+//?\s*\w+', '', text)
    return text.strip()

def collect_source(module):
    h_path = os.path.join(MODULES_DIR, f'{module}.h')
    c_path = os.path.join(MODULES_DIR, f'{module}.c')
    h_text = open(h_path, 'r').read() if os.path.exists(h_path) else ''
    c_text = open(c_path, 'r').read() if os.path.exists(c_path) else ''
    return (strip_guard(clean_includes(h_text)), c_text)

def generate_libx_header():
    output = [f'#ifndef {HEADER_GUARD}', f'#define {HEADER_GUARD}', '', '// libx amalgamated header', '']

    for module in MODULE_ORDER:
        h, _ = collect_source(module)
        output.append(f'// ==== {module}.h ====')
        output.append(h)
        output.append('')

    output.append('#ifdef LIBX_IMPL')
    output.append('')

    for module in MODULE_ORDER:
        _, c = collect_source(module)
        output.append(f'// ==== {module}.c ====')
        output.append(c)
        output.append('')

    output.append('#endif // LIBX_IMPL')
    output.append(f'#endif // {HEADER_GUARD}')

    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    with open(OUTPUT_FILE, 'w') as out_file:
        out_file.write('\n'.join(output))

    print(f'[+] Generated header-only libx to {OUTPUT_FILE}')

if __name__ == '__main__':
    generate_libx_header()
