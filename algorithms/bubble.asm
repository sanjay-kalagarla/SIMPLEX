; bubble_fixed.asm - SIMPLEX bubble sort (uses locals because the stack is only A,B)
; locals: 0=n 1=i 2=j 3=p (address of array[j]) 4=tmp
ldc 0x1000
a2sp
adj -5
ldc n
ldnl 0
stl 0           ; n
ldl 0
adc -1
stl 1           ; i = n-1
outer: ldl 1
brz done        ; i == 0 -> finished
ldc 0
stl 2           ; j = 0
inner: ldl 2
ldl 1
sub             ; A = j - i
brlz body       ; j < i -> compare
br nextouter
body: ldc array
ldl 2
add
stl 3           ; p = array + j
ldl 3
ldnl 1          ; arr[j+1]
ldl 3
ldnl 0          ; B = arr[j+1], A = arr[j]
sub             ; A = arr[j+1] - arr[j]
brlz swap       ; arr[j+1] < arr[j]
br noswap
swap: ldl 3
ldnl 0
stl 4           ; tmp = arr[j]
ldl 3
ldnl 1          ; A = arr[j+1]
ldl 3           ; B = arr[j+1], A = p
stnl 0          ; arr[j] = arr[j+1]
ldl 4
ldl 3
stnl 1          ; arr[j+1] = tmp
noswap: ldl 2
adc 1
stl 2           ; j++
br inner
nextouter: ldl 1
adc -1
stl 1           ; i--
br outer
done: HALT
array: data 5
data 3
data 8
data 1
data 4
n: data 5
