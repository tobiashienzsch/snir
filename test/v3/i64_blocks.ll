; BEGIN_TEST
; name: func
; type: i64
; args: 0
; blocks: 4
; instructions: 10
; return: 9
; END_TEST
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
