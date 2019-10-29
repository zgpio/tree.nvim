# -*- coding: utf-8 -*-
import json
from pathlib import Path

self_dir = Path(__file__).parent.resolve()
with open(f"{self_dir}/icons.nerdfont.json", 'r') as f:
    load_dict = json.load(f)

# print(load_dict)

# NOTE: mac builtin terminal: webpack/vue 乱码
with open(f"{self_dir}/transform.cpp", "w") as f:
    for name, val in load_dict.items():
        if name == 'icons':
            f.write('unordered_map<string, array<QString, 2>> icons = {\n')
            for k, v in val.items():
                code = v['code']
                color = v['color']
                print(k, code, color)
                line = f'    {{ "{k}", {{"{code}", "{color}"}} }},\n'
                f.write(line)
            f.write('};\n')
        else:
            f.write('\n')
            f.write(f'unordered_map<string, string> {name} = {{\n')
            for k, v in val.items():
                line = f'    {{ "{k}", "{v}" }},\n'
                f.write(line)
            f.write('};\n')
