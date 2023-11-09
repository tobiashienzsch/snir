define float @func() {
0:
    %0 = float 42
    %1 = double 143
    %2 = fsub float %0, %1
    ret float %2
}
