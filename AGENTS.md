# AGENTS.md

## Cursor Cloud specific instructions

This is a pure C codebase (no build system, no package manager). GCC is the only dependency and is pre-installed in the VM.

### Building and running

Each lab directory contains standalone `.c` files. Compile with `gcc` and run from the respective directory (programs use relative paths for input files):

```
cd lab2 && gcc fila.c -o fila && ./fila
cd lab2 && gcc pilha.c -o pilha && ./pilha
cd lab3 && gcc abb.c -o abb && ./abb
cd lab3 && gcc avl.c -o avl && ./avl
```

### Known issues

- `lab3/avl.c` has a pre-existing bug in `remove_n()`: variable shadowing causes a segfault during the removal phase. The insertion phase works correctly. GCC emits `-Wunused-variable` and `-Wmaybe-uninitialized` warnings pointing to the issue.

### Notes

- There is no test framework, linter, or CI. Validation is done by compiling with `-Wall` and running the programs against their input files.
- Compiled binaries are committed to the repo. After code changes, recompile from the program's directory.
