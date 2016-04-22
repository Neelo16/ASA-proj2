#!/usr/bin/env python3
import os
import re

files = [e for e in os.listdir('.') if e.endswith('.time')]
data = []

size_getter = re.compile('random_graph_(\d+).time')

for path in files:
    with open(path, 'r') as f:
        f.readline()
        important_part = f.readline().split('\t')[1].replace('s\n', '').replace('0m', '')
    data.append((int(size_getter.match(path).group(1)), important_part))

with open('test_results.dat', 'w') as f:
    for vertices, time in sorted(data, key=lambda x: x[0]):
        f.write('{}\t{}\n'.format(vertices*2, time))
