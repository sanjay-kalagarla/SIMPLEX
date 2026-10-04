; max_element.asm - find the largest value of an array
; locals: 0=n 1=i 2=max 3=p (address of array[i])
; NOTE: only A and B exist as a register stack, so every address is
; computed into a local first instead of being kept in registers.
ldc 0x1000
a2sp
adj -4
ldc n
ldnl 0
stl 0
ldc array
ldnl 0
stl 2
ldc 1
stl 1
loop: ldl 1
ldl 0
sub
brlz body
br finish
body: ldc array
ldl 1
add
stl 3
ldl 3
ldnl 0
ldl 2
sub             ; arr[i] - max
brlz skip
ldl 3
ldnl 0
stl 2
skip: ldl 1
adc 1
stl 1
br loop
finish: ldl 2
ldc maxval
stnl 0
HALT
array: data 5
data 3
data 8
data 1
data 4
n: data 5
maxval: data 0
