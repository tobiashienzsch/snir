; BEGIN_TEST
; error: failed to parse 'foo label %1' as an instruction
; END_TEST
define i64 @func() {
0:
  foo label %1
}
