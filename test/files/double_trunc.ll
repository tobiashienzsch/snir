define double @func() {
0:
    %0 = double 42.0
    %1 = double 143.0
    %2 = fsub double %0, %1
    %3 = trunc %2 to double
    ret double %3
}
