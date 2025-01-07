# SAT Solver

I implemented a SAT solver in Python and then rewrote it in C to increase its speed. I used unit propagation and pure literal elimination, plus other heuristics described in the READMEs in the `c/` and `python/` directories.

To run the Python SAT solver, use:
```
cd python
make run INPUT=<input file> OUTPUT=<output file>
```

To build and run the C SAT solver, use:
```
cd c
make build
make run INPUT=<input file> OUTPUT=<output file>
```

The input must use [DIMACS format](https://jix.github.io/varisat/manual/0.2.0/formats/dimacs.html).

The output will have the following format:
```
s SATISFIABLE
v <the variable assignments> 0
```

```
s UNSATISFIABLE
```