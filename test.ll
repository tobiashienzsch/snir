; ./test/v3/void.ll
define void @func() {
0:
  ret void
}

; ./test/v3/i64_xor.ll
define i64 @func() {
0:
  %1 = i64 42
  %2 = i64 143
  %3 = xor i64 %1, %2
  ret i64 %3
}

; ./test/v3/i64_add.ll
define i64 @func() {
0:
  %1 = i64 42
  %2 = i64 143
  %3 = add i64 %1, %2
  ret i64 %3
}

; ./test/v3/i64_and.ll
define i64 @func() {
0:
  %1 = i64 42
  %2 = i64 143
  %3 = and i64 %1, %2
  ret i64 %3
}

; ./test/v3/i64_args_1.ll
define i64 @func(i64 %0) {
1:
  ret i64 %0
}

; ./test/v3/i64_args_2.ll
define i64 @func(i64 %0, i64 %1) {
2:
  %3 = add i64 %0, %1
  ret i64 %3
}

; ./test/v3/i64_blocks.ll
define i64 @func() {
0:
  ; nop
  ; nop
1:
  %2 = i64 1
  %3 = i64 2
  %4 = add i64 %2, %3
5:
  %6 = i64 4
  %7 = i64 1
  %8 = sub i64 %6, %7
9:
  %10 = mul i64 %4, %8
  ret i64 %10
}

; ./test/v3/i64_branch.ll
define i64 @func() {
0:
  br label 1
2:
  %3 = i64 9
  ret i64 %3
}

; ./test/v3/i64_const.ll
define i64 @func() {
0:
  %1 = i64 42
  ret i64 %1
}

; ./test/v3/i64_div.ll
define i64 @func() {
0:
  %1 = i64 42
  %2 = i64 2
  %3 = div i64 %1, %2
  ret i64 %3
}

; ./test/v3/i64_icmp_eq_1.ll
define i64 @func() {
0:
  %1 = i64 42
  %2 = i64 143
  %3 = icmp eq i64 %1, %2
  ret i64 %3
}

; ./test/v3/i64_icmp_eq_2.ll
define i64 @func() {
0:
  %1 = i64 42
  %2 = i64 42
  %3 = icmp eq i64 %1, %2
  ret i64 %3
}

; ./test/v3/i64_icmp_ne_1.ll
define i64 @func() {
0:
  %1 = i64 42
  %2 = i64 143
  %3 = icmp ne i64 %1, %2
  ret i64 %3
}

; ./test/v3/i64_icmp_ne_2.ll
define i64 @func() {
0:
  %1 = i64 42
  %2 = i64 42
  %3 = icmp ne i64 %1, %2
  ret i64 %3
}

; ./test/v3/i64_mod.ll
define i64 @func() {
0:
  %1 = i64 42
  %2 = i64 3
  %3 = mod i64 %1, %2
  ret i64 %3
}

; ./test/v3/i64_mul.ll
define i64 @func() {
0:
  %1 = i64 42
  %2 = i64 143
  %3 = mul i64 %1, %2
  ret i64 %3
}

; ./test/v3/i64_or.ll
define i64 @func() {
0:
  %1 = i64 42
  %2 = i64 143
  %3 = or i64 %1, %2
  ret i64 %3
}

; ./test/v3/i64_shl.ll
define i64 @func() {
0:
  %1 = i64 42
  %2 = i64 2
  %3 = shl i64 %1, %2
  ret i64 %3
}

; ./test/v3/i64_shr.ll
define i64 @func() {
0:
  %1 = i64 42
  %2 = i64 2
  %3 = shr i64 %1, %2
  ret i64 %3
}

; ./test/v3/i64_sub.ll
define i64 @func() {
0:
  %1 = i64 42
  %2 = i64 143
  %3 = sub i64 %1, %2
  ret i64 %3
}

; ./test/v3/double_add.ll
define double @func() {
0:
  ; nop
  %1 = double 42
  %2 = double 143
  %3 = fadd double %1, %2
  ret double %3
}

; ./test/v3/double_div.ll
define double @func() {
0:
  %1 = double 42
  %2 = double 2
  %3 = fdiv double %1, %2
  ret double %3
}

; ./test/v3/double_mul.ll
define double @func() {
0:
  %1 = double 42
  %2 = double 143
  %3 = fmul double %1, %2
  ret double %3
}

; ./test/v3/double_sub.ll
define double @func() {
0:
  %1 = double 42
  %2 = double 143
  %3 = fsub double %1, %2
  ret double %3
}

; ./test/v3/double_trunc.ll
define double @func() {
0:
  %1 = double 42
  %2 = double 143
  %3 = fsub double %1, %2
  %4 = trunc %3 to double
  ret double %4
}

; ./test/v3/float_add.ll
define float @func() {
0:
  %1 = float 42
  %2 = float 143
  %3 = fadd float %1, %2
  ret float %3
}

; ./test/v3/float_div.ll
define float @func() {
0:
  %1 = float 42
  %2 = float 2
  %3 = fdiv float %1, %2
  ret float %3
}

; ./test/v3/float_mul.ll
define float @func() {
0:
  %1 = float 42
  %2 = float 143
  %3 = fmul float %1, %2
  ret float %3
}

; ./test/v3/float_sub.ll
define float @func() {
0:
  %1 = float 42
  %2 = float 143
  %3 = fsub float %1, %2
  ret float %3
}

; ./test/v3/float_trunc.ll
define float @func() {
0:
  %1 = float 42
  %2 = float 143
  %3 = fsub float %1, %2
  %4 = trunc %3 to float
  ret float %4
}
