import os, re

# Config
RE_FLAGS = re.M
HEADER_GUARD = '__LIBX_H__'
MODULES_DIR = 'libx'
MODULE_ORDER = [
    'libx/libx_def',
    'libx/memx/libx_memx',
    'libx/dsx/libx_dsx',
    'libx/genx/libx_genx',
    'libx/ecsx/libx_ecsx',
    'libx/mathx/libx_mathx',
    'libx/filex/libx_filex',
    'libx/libx'
]

def strip_includes(text):
    text = re.sub(r'(^#include\s<\w+/\w+\W\w+(>|\W\w>)$)', '', text, flags=RE_FLAGS)
    return text.strip()

def strip_directives(text) -> str:
    # Remove #ifndef / #define / #endif directives from headers
    text = re.sub(r'^#((ifndef)|(define)|(include))\s((<libx.*>)|(__\w+__))$', '', text, flags=RE_FLAGS)
    text = re.sub(r'#endif\s+//?\s*\w+', '', text, flags=RE_FLAGS)
    return text.strip()

def collect_source(module) -> list[str, str]:
    h_path = f'{module}.h'; c_path = f'{module}.c'
    h_text = open(h_path, 'r').read() if os.path.exists(h_path) else ''
    c_text = open(c_path, 'r').read() if os.path.exists(c_path) else ''
    return [
        strip_directives(strip_includes(h_text)),
        strip_includes(c_text) 
    ]

def gen_monolith(out_dir: str):
    output = [f'#ifndef {HEADER_GUARD}', f'#define {HEADER_GUARD}', '']

    for module in MODULE_ORDER:
        h, _ = collect_source(module)
        output.append(f'// ==== {module}.h ====')
        output.append(h)
        output.append('')

    output.append('#ifdef LIBX_IMPL')
    output.append('')

    for module in MODULE_ORDER:
        if module == "libx/libx_def": continue
        _, c = collect_source(module)
        output.append(f'// ==== {module}.c ====')
        output.append(c)
        output.append('')

    output.append('#endif // LIBX_IMPL')
    output.append(f'#endif // {HEADER_GUARD}')

    out_file = f"{out_dir}{os.sep}libx.h"
    os.makedirs(os.path.dirname(out_file), exist_ok=True)
    with open(out_file, 'w') as header:
        header.write('\n'.join(output))

    print(f'[+] Generated Libx Header At: {out_file}');
