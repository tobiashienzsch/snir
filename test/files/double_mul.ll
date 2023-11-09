define double @func() {
0:
    %0 = double 42.0
    %1 = double 143.0
    %2 = fmul double %0, %1
    ret double %2
}
