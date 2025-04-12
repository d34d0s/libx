import os, re

# Config
RE_FLAGS = re.M
HEADER_GUARD = '__COREX_H__'
MODULES_DIR = 'corex'
MODULE_ORDER = [
    'corex/corex_def',
    'corex/memx/corex_memx',
    'corex/dsx/corex_dsx',
    'corex/genx/corex_genx',
    'corex/ecsx/corex_ecsx',
    'corex/mathx/corex_mathx',
    'corex/filex/corex_filex',
    'corex/corex'
]

def strip_includes(text):
    text = re.sub(r'(^#include\s<\w+/\w+\W\w+(>|\W\w>)$)', '', text, flags=RE_FLAGS)
    return text.strip()

def strip_directives(text) -> str:
    # Remove #ifndef / #define / #endif directives from headers
    text = re.sub(r'^#((ifndef)|(define)|(include))\s((<corex.*>)|(__\w+__))$', '', text, flags=RE_FLAGS)
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

    output.append('#ifdef COREX_IMPL')
    output.append('')

    for module in MODULE_ORDER:
        if module == "corex/corex_def": continue
        _, c = collect_source(module)
        output.append(f'// ==== {module}.c ====')
        output.append(c)
        output.append('')

    output.append('#endif // COREX_IMPL')
    output.append(f'#endif // {HEADER_GUARD}')

    out_file = f"{out_dir}{os.sep}corex.h"
    os.makedirs(os.path.dirname(out_file), exist_ok=True)
    with open(out_file, 'w') as header:
        header.write('\n'.join(output))

    print(f'[+] Generated Corex Header At: {out_file}');
