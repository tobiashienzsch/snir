define i64 @func() {
0:
  ; nop
  ; nop
1:
  %0 = i64 1
  %1 = i64 2
  %2 = add i64 %0 %1
2:
  %3 = i64 4
  %4 = i64 1
  %5 = sub i64 %3 %4
2:
  %6 = mul i64 %2 %5
  ret i64 %6
}
