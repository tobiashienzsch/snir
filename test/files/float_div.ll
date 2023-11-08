define float @func() {
0:
    %0 = float 42.0
    %1 = float 2.0
    %2 = fdiv float %0 %1
    ret %2
}
