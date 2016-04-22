#!/usr/bin/env python3
import random  # randint


def generate_graph(num_vertices, path):
    existing_edges = []
    # num_vertices = 1000
    for i in range(1, num_vertices):
        w = random.randint(0, 10)
        i2 = random.randint(1, num_vertices)
        edge = (i, i2, w)
        # while i2 == i or edge in existing_edges:
        #     i2 = random.randint(1, num_vertices)
        #     edge = (i, i2, w)
        existing_edges.append(edge)

    num_branches = 10
    branches = range(1, num_branches+1)

    with open(path, 'w') as f:
        f.write('{verts} {branches} {edges}\n'.format(verts=num_vertices, branches=num_branches, edges=num_vertices))
        for branch in branches[:-1]:
            f.write('{} '.format(branch))
        f.write('{}\n'.format(branches[-1]))
        for edge1, edge2, w in existing_edges:
            f.write('{} {} {}\n'.format(edge1, edge2, w))

for i in range(10, 10001, 100):
    print('Generating for {}'.format(i))
    generate_graph(i, 'random_graph_{}.in'.format(i))
