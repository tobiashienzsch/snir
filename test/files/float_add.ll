define float @func() {
0:
    %0 = float 42
    %1 = float 143
    %2 = fadd float %0, %1
    ret float %2
}
