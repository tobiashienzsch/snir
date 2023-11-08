define double @func() {
0:
    %0 = double 42
    %1 = double 143
    %2 = fadd double %0 %1
    ret %2
}
