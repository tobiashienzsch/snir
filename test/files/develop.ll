define double @func() {
0:
  ; nop
  ; nop
1:
  %0 = i64 1
  %1 = i64 2
  %2 = add i64 %0, %1
2:
  %3 = double 2
  %4 = double 1.14159265359
  %5 = fadd double %3, %4
  ret double %5
}
