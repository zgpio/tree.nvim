#!/usr/bin/python
# -*- coding: utf-8 -*-

from jinja2 import Environment, FileSystemLoader
import msgpack, sys, os, subprocess

env = Environment(loader=FileSystemLoader('templates', encoding='utf8'))
tpl = env.get_template('api.hpp')

api_info = subprocess.check_output(["nvim", '--api-info'])
#print msgpack.unpackb(api_info)

unpacked_api = msgpack.unpackb(api_info)

functions = []
for f in unpacked_api['functions']:

    d = {}
    d['name'] = f['name']
    d['return'] = f['return_type']
    
    d['args'] = [{'type': arg[0], 'name': arg[1]} if arg == f['parameters'][-1] else {'type': arg[0], 'name': arg[1] + ", "} for arg in f['parameters']]
    print d['args']
    #print d
    functions.append(d)

api = tpl.render({'functions': functions})
print api.encode('utf-8')

with open(os.path.join("./", "auto"), 'w') as f:
    f.write(api)

