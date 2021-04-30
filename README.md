# BrainfuckTools

## brainfuck_compiler

Compile with:
> gcc brainfuck.c brainfuck_compiler.c -o brainfuck_compiler

This program compiles text brainfuck files into "optimized" binary files
compatible with brainfuck_runner.
It only takes input via stdin, and output the result via stdout.

## brainfuck_runner

Compile with:
> gcc brainfuck.C brainfuck_runner.c -o brainfuck_runner

This program runs binary files produced by brainfuck_compiler.
It only takes input via stdin, and output the result via stdout.

## Examples

```bash
echo -n ",[.,]" | ./brainfuck_compiler | ./brainfuck_runner "Hello world"
> Hello world
```
