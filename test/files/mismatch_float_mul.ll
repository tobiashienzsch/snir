define float @func() {
0:
    %0 = float 42
    %1 = double 143
    %2 = fmul float %0 %1
    ret %2
}
