# SIMPLEX

Two-pass assembler (`asm.c`) and emulator (`emu.c`) for the SIMPLEX
instruction set, written in ISO C89 for the CS2206 mini project.

Author: Sanjay Kalagarla (2401CS15)

## Build

```
gcc -std=c89 -pedantic -W -Wall -Wpointer-arith -Wwrite-strings -Wstrict-prototypes asm.c -o asm
gcc -std=c89 -pedantic -W -Wall -Wpointer-arith -Wwrite-strings -Wstrict-prototypes emu.c -o emu
```

## Use

```
./asm bubble.asm          # writes bubble.obj and bubble.lst (or bubble.log on errors)
./emu bubble.obj 128      # writes bubble.log; 128 = words of memory to dump (default 64)
```

## Machine

Registers A and B (a two-deep stack), PC and SP, all 32 bits.
Each instruction is one 32-bit word: low 8 bits are the opcode, upper 24
bits are a signed operand: `word = (operand << 8) | opcode`.

| Mnemonic | Opcode | Operand | Effect |
|---|---|---|---|
| data | - | value | reserve a word holding value |
| ldc | 0 | value | B = A; A = value |
| adc | 1 | value | A = A + value |
| ldl | 2 | offset | B = A; A = mem[SP + offset] |
| stl | 3 | offset | mem[SP + offset] = A; A = B |
| ldnl | 4 | offset | A = mem[A + offset] |
| stnl | 5 | offset | mem[A + offset] = B |
| add | 6 | - | A = B + A |
| sub | 7 | - | A = B - A |
| shl | 8 | - | A = B << A |
| shr | 9 | - | A = B >> A |
| adj | 10 | value | SP = SP + value |
| a2sp | 11 | - | SP = A; A = B |
| sp2a | 12 | - | B = A; A = SP |
| call | 13 | offset | B = A; A = PC; PC = PC + offset |
| return | 14 | - | PC = A; A = B |
| brz | 15 | offset | if A == 0: PC = PC + offset |
| brlz | 16 | offset | if A < 0: PC = PC + offset |
| br | 17 | offset | PC = PC + offset |
| HALT | 18 | - | stop the emulator |
| SET | - | value | give the label on this line the value instead of the PC |

PC is incremented before an instruction executes, so a label used with
`br`, `brz`, `brlz` or `call` becomes `label - (address of the branch + 1)`.
A plain number given to those instructions is used as the offset itself.

## Assembler

- Pass 1 builds the symbol table; pass 2 emits the object file and listing.
- Errors are collected with line numbers and written to `<name>.log`; when
  there are errors no `.obj` or `.lst` is produced.
- Errors: duplicate label, undefined label, invalid label name, unknown
  mnemonic, invalid number, missing operand, unexpected operand, extra text
  on a line, `SET` without a label, operand wider than 24 bits, label too
  long, symbol table overflow, over-long line (>255 characters, only when
  code is cut off; long comments are ignored).
- Warning: label defined but never used.
- Listing format: `address  machine-word  mnemonic operand`, plus an
  `address label:` line for every label.

## Emulator

Loads the object file, runs fetch / decode / execute, and writes a trace,
the final registers and a memory dump to `<name>.log`.
It stops on `HALT` or on an error: illegal opcode, PC out of range, memory
access out of range, shift count outside 0..31, or more than 100000 steps.

## Programs and tests

| File | What it shows |
|---|---|
| test01.asm | valid program from the spec |
| test02.asm | all assembler errors (9 reported) |
| test03.asm | `SET` |
| test04.asm | provided program; fills `result` with row 10 of Pascal's triangle |
| test05.asm | emulator: illegal opcode |
| test06.asm | emulator: out-of-range memory read |
| bubble.asm, insertion.asm, selection.asm | sort {5,3,8,1,4} to {1,3,4,5,8} |
| max_element.asm | maximum of the same array (8) |

Each program has its `.lst`, `.obj` and `.log` next to it. The `.log` of a
program that assembled is the emulator trace; `test02.log` is the assembler
error log.
