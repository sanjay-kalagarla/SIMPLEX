; insertion.asm - insertion sort
; locals: 0=n 1=i 2=j 3=key 4=p 5=cur
ldc 0x1000
a2sp
adj -6
ldc n
ldnl 0
stl 0
ldc 1
stl 1
outer: ldl 1
ldl 0
sub             ; i - n
brlz body
br done
body: ldc array
ldl 1
add
ldnl 0
stl 3           ; key = arr[i]
ldl 1
adc -1
stl 2           ; j = i - 1
inner: ldl 2
brlz place
ldc array
ldl 2
add
ldnl 0
stl 5           ; cur = arr[j]
ldl 3
ldl 5
sub             ; key - cur
brlz shift
br place
shift: ldc array
ldl 2
add
stl 4
ldl 5
ldl 4
stnl 1          ; arr[j+1] = cur
ldl 2
adc -1
stl 2
br inner
place: ldc array
ldl 2
add
stl 4
ldl 3
ldl 4
stnl 1          ; arr[j+1] = key
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
