define double @nan() {
0:
  %3 = double 2
  %4 = double 1.14159265359
  %5 = fadd double %3 %4
  ret %5
}

define float @sin(float %0) {
0:
  %1 = double 42
  %2 = trunc %1 as float
  ret %2
}

define i64 @ipow(i64 %0, i64 %1) {
0:
  %0 = i64 42
  ret %0
1:
  %1 = i64 43
  ret %1
}
