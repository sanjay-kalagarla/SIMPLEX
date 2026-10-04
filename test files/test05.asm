; test05.asm - emulator test: illegal opcode
; Assembles fine, but the data word 0x7F decodes as opcode 127,
; so the emulator must stop with "illegal opcode".
ldc 1
data 0x7F
HALT
