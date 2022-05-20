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
            f.write('enum Icon {\n')
            for k in val.keys():
                line = f'  {k},\n'
                f.write(line)
            f.write('};\n')

            f.write('pair<string, string> icons[] = {\n')
            for k, v in val.items():
                code = v['code']
                # TODO: remove #
                color = v['color']
                print(k, code, color)
                line = f'    {{"{code}", "{color}"}},\n'
                f.write(line)
            f.write('};\n')
        else:
            f.write('\n')
            f.write(f'unordered_map<string, Icon> {name} = {{\n')
            for k, v in val.items():
                line = f'    {{ "{k}", {v} }},\n'
                f.write(line)
            f.write('};\n')
