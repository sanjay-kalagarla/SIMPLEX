; test06.asm - emulator test: memory access out of bounds
; 0x7FFFF is far beyond the 65536-word memory, so ldnl must be rejected.
ldc 0x7FFFF
ldnl 0
HALT
