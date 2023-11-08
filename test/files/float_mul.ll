define float @func() {
0:
    %0 = float 42.0
    %1 = float 143.0
    %2 = fmul float %0 %1
    ret %2
}
