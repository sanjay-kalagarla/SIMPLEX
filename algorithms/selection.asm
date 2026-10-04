; selection.asm - selection sort
; locals: 0=n 1=i 2=j 3=m(index of minimum) 4=cur/p1 5=best/p2
ldc 0x1000
a2sp
adj -6
ldc n
ldnl 0
stl 0
ldc 0
stl 1
outer: ldl 0
adc -1
ldl 1
sub             ; (n-1) - i
brz done
ldl 1
stl 3
ldl 1
adc 1
stl 2
inner: ldl 0
ldl 2
sub             ; n - j
brz nextouter
ldc array
ldl 2
add
ldnl 0
stl 4           ; cur = arr[j]
ldc array
ldl 3
add
ldnl 0
stl 5           ; best = arr[m]
ldl 4
ldl 5
sub             ; cur - best
brlz newmin
br incj
newmin: ldl 2
stl 3
incj: ldl 2
adc 1
stl 2
br inner
nextouter: ldc array
ldl 1
add
stl 4           ; address of arr[i]
ldc array
ldl 3
add
stl 5           ; address of arr[m]
ldl 4
ldnl 0
stl 2           ; tmp = arr[i]
ldl 5
ldnl 0
ldl 4
stnl 0          ; arr[i] = arr[m]
ldl 2
ldl 5
stnl 0          ; arr[m] = tmp
ldl 1
adc 1
stl 1
br outer
done: HALT
array: data 5
data 3
data 8
data 1
data 4
n: data 5
