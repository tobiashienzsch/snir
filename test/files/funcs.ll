define double @nan() {
0:
  %3 = double 2
  %4 = double 4.0
  %5 = fadd double %3, %4
  ret double %5
}

define float @sin(float %0) {
0:
  %1 = double 42
  %2 = trunc %1 to float
  ret float %2
}

define i64 @ipow(i64 %0, i64 %1) {
0:
  %0 = i64 42
  ret %0
1:
  %1 = i64 43
  ret i64 %1
}
