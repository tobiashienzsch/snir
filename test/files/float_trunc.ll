define float @func() {
0:
    %0 = float 42.0
    %1 = float 143.0
    %2 = fsub float %0 %1
    %3 = trunc %2 as float
    ret float %3
}
