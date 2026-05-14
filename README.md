# Vertex Cover Solver

A C++ implementation of a parameterized Minimum Vertex Cover solver using branching rules, reduction rules, and a Branch-and-Bound (BaB) approach.

## Features

- Branch-and-Bound optimization
- Reduction and branching rules
- Greedy heuristic upper bound
- Adjacency matrix graph representation
- Recursive search procedure
- Solution validation support

## Build

```bash
g++ -O2 -std=c++17 VC_BaB_improved -o vc
```

## Run

```bash
./vc
```

## Input Format

```text
n m
u1 v1
u2 v2
...
```

- `n` = number of vertices
- `m` = number of edges

## Output Format

```text
k
v1
v2
...
```

Where `k` is the size of the vertex cover.

## Notes

- Uses a greedy heuristic to obtain an initial upper bound.
- Solutions were validated using `validator.py`.
- Different compiler optimizations were tested for performance improvements.
